#include <QDomDocument>
#include <optional>
#include <QVariant>

#include "parserworker.h"

static QStringList validTags = {"texteditor", "fileformats", "encoding",
                                "hasintellisense", "hasplugins", "cancompile"};

// Такую реализацию с потоком увидел здесь https://www.youtube.com/watch?v=SncJ3D-fO7g
ParserWorker::ParserWorker(QObject *parent)
        : QObject(parent), worker_() {

    moveToThread(&worker_);
    worker_.start();
}

QVariant ParserWorker::parseFile(const QString &path) {
    QDomDocument doc(path);
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) return {path + ": " + file.errorString()};

    QString errorMsg;
    bool isSet = doc.setContent(&file, &errorMsg);
    file.close();

    if (!isSet) return {path + ": " + errorMsg};

    QDomElement root = doc.documentElement();
    if (root.tagName() != "root") return {path + ": there is no root tag"};

    auto isBool = [](const QString &value) {
        auto lower = value.toLower();
        return lower == "true" || lower == "false";
    };

    QHash<QString, QVariant> tags;
    QDomNode node = root.firstChild();
    while (!node.isNull()) {
        QDomElement element = node.toElement();
        auto tag = element.tagName();
        auto value = element.text();

        int index = validTags.indexOf(tag);
        if (index == -1) return {path + ": invalid tag"};

        // После 2 индекса идут bool'ы
        if (value.isEmpty() || (index > 2 && !isBool(value))) return {path + ": tag value is empty or invalid"};

        tags.insert(tag, value);
        node = node.nextSibling();
    }

    return tags;
}

void ParserWorker::parse(QDir dir) {
    QStringList files = dir.entryList({"*.xml"}, QDir::Filter::Files);

    if (files.isEmpty()) return;
    emit started(files.size());

    QVector<CodeEditor> entries;
    int progress_value = 0;
    for (auto &it: files) {
        emit progress(++progress_value);

        QString path = dir.filePath(it);
        auto result = parseFile(path);

        if (result.type() == QVariant::String) {
            emit log(result.toString());
            continue;
        }

        QHash<QString, QVariant> tags = result.toHash();
        if (tags.size() != validTags.size()) {
            emit log(it + ": not enough tags");
            continue;
        }

        QString textEditor = tags[validTags[0]].toString();
        QString fileFormats = tags[validTags[1]].toString();
        QString encoding = tags[validTags[2]].toString();
        bool hasIntellisense = tags[validTags[3]].toBool();
        bool hasPlugins = tags[validTags[4]].toBool();
        bool canCompile = tags[validTags[5]].toBool();

        CodeEditor entry(-1, textEditor, fileFormats, encoding, hasIntellisense, hasPlugins, canCompile);
        entries.append(entry);

        emit log(it + ": finished");
    }

    emit finished(std::move(entries));
}

ParserWorker::~ParserWorker() {
    QMetaObject::invokeMethod(this, &ParserWorker::cleanup);
    worker_.wait();
}

void ParserWorker::cleanup() {
    worker_.quit();
}
