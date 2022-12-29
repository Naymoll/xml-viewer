#include <QDomDocument>
#include <optional>
#include <QVariant>
#include <QTextStream>

#include "xmlworker.h"

static QStringList validTags = {"texteditor", "fileformats", "encoding",
                                "hasintellisense", "hasplugins", "cancompile"};

// Такую реализацию с потоком увидел здесь https://www.youtube.com/watch?v=SncJ3D-fO7g
XmlWorker::XmlWorker(QObject *parent)
        : QObject(parent), worker_() {

    moveToThread(&worker_);
    worker_.start();
}

QVariant XmlWorker::importFile(const QString &path) {
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

void XmlWorker::importEditors(QDir dir) {
    QStringList files = dir.entryList({"*.xml"}, QDir::Filter::Files);

    if (files.isEmpty()) return;
    emit started(files.size());

    QVector<CodeEditor> entries;
    int progress_value = 0;
    for (auto &it: files) {
        emit progress(++progress_value);

        QString path = dir.filePath(it);
        auto result = importFile(path);

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

        emit log(it + ": imported");
    }

    emit imported(std::move(entries));
}

void XmlWorker::exportEditors(QDir dir, QVector<CodeEditor> editors) {
    auto createElement = [](QDomDocument &doc, const QString &tag, const QVariant &value) {
        QDomElement textElement = doc.createElement(tag);
        QDomText tagValue = doc.createTextNode(value.toString());
        textElement.appendChild(tagValue);
        return textElement;
    };

    emit started(editors.size());
    int progress_value = 0;
    for (auto &editor: editors) {
        emit progress(++progress_value);

        QString fileName = QString::number(editor.id) + "_" + editor.textEditor + ".xml";
        QString path = dir.filePath(fileName);

        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            emit log(path + ": can't create file");
            continue;
        }

        QDomDocument doc;
        QDomElement root = doc.createElement("root");
        doc.appendChild(root);

        auto textEditor = createElement(doc, validTags[0], editor.textEditor);
        root.appendChild(textEditor);

        auto fileFormats = createElement(doc, validTags[1], editor.fileFormats);
        root.appendChild(fileFormats);

        auto encoding = createElement(doc, validTags[2], editor.encoding);
        root.appendChild(encoding);

        auto hasIntellisense = createElement(doc, validTags[3], editor.hasIntellisense);
        root.appendChild(hasIntellisense);

        auto hasPlugins = createElement(doc, validTags[4], editor.hasPlugins);
        root.appendChild(hasPlugins);

        auto canCompile = createElement(doc, validTags[5], editor.canCompile);
        root.appendChild(canCompile);

        QTextStream stream(&file);
        stream << doc;
        file.close();

        emit log(path + ": exported");
    }

    emit exported();
}

XmlWorker::~XmlWorker() {
    QMetaObject::invokeMethod(this, &XmlWorker::cleanup);
    worker_.wait();
}

void XmlWorker::cleanup() {
    worker_.quit();
}
