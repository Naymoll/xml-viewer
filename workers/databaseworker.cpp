#include "databaseworker.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QVariantList>

// Такую реализацию с потоком увидел здесь https://www.youtube.com/watch?v=SncJ3D-fO7g
DatabaseWorker::DatabaseWorker(const QString &type, const QString &name, QObject *parent)
        : QObject(parent), worker_() {
    db_ = QSqlDatabase::addDatabase(type);
    db_.setDatabaseName(name);

    moveToThread(&worker_);
    connect(&worker_, &QThread::started, this, &DatabaseWorker::start);

    worker_.start();
}

DatabaseWorker::~DatabaseWorker() {
    QMetaObject::invokeMethod(this, &DatabaseWorker::cleanup);
    worker_.wait();
}

void DatabaseWorker::cleanup() {
    db_.close();
    worker_.quit();
}

bool DatabaseWorker::start() {
    if (!db_.open()) {
        emit error(db_.lastError().text());
        return false;
    }

    QString str = "CREATE TABLE IF NOT EXISTS code_editors ("
                  "id               INTEGER PRIMARY KEY NOT NULL, "
                  "text_editor      TEXT NOT NULL, "
                  "file_formats     TEXT NOT NULL, "
                  "encoding         TEXT NOT NULL, "
                  "has_intellisense BOOLEAN NOT NULL, "
                  "has_plugins      BOOLEAN NOT NULL, "
                  "can_compile      BOOLEAN NOT NULL "
                  ")";

    QSqlQuery query(db_);
    if (!query.exec(str)) {
        emit error(query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseWorker::insert(CodeEditor editor) {
    QString str = "INSERT INTO code_editors ("
                  "text_editor, file_formats, encoding, "
                  "has_intellisense, has_plugins, can_compile) "
                  "VALUES (?, ?, ?, ?, ?, ?)";

    QSqlQuery query(db_);
    if (!query.prepare(str)) {
        emit error(query.lastError().text());
        return false;
    }

    query.bindValue(0, editor.textEditor);
    query.bindValue(1, editor.fileFormats);
    query.bindValue(2, editor.encoding);
    query.bindValue(3, editor.hasIntellisense);
    query.bindValue(4, editor.hasPlugins);
    query.bindValue(5, editor.canCompile);

    if (!query.exec()) {
        emit error(query.lastError().text());
        return false;
    }


    int id = query.lastInsertId().toInt();
    editor.id = id;
    emit inserted(editor);
    return true;
}

bool DatabaseWorker::update(CodeEditor editor) {
    QString str = "UPDATE code_editors "
                  "SET text_editor = ?, "
                  "file_formats = ?, "
                  "encoding = ?, "
                  "has_intellisense = ?, "
                  "has_plugins = ?, "
                  "can_compile = ? "
                  "WHERE id = ?";

    QSqlQuery query(db_);
    if (!query.prepare(str)) {
        emit error(query.lastError().text());
        return false;
    }

    query.bindValue(0, editor.textEditor);
    query.bindValue(1, editor.fileFormats);
    query.bindValue(2, editor.encoding);
    query.bindValue(3, editor.hasIntellisense);
    query.bindValue(4, editor.hasPlugins);
    query.bindValue(5, editor.canCompile);
    query.bindValue(6, editor.id);

    if (!query.exec()) {
        emit error(query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseWorker::remove(int id) {
    QString str = "DELETE FROM code_editors WHERE id = ?";

    QSqlQuery query(db_);
    query.prepare(str);
    query.bindValue(0, id);
    if (!query.exec()) {
        emit error(query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseWorker::load() {
    QString str = "SELECT * FROM code_editors";

    QSqlQuery query(db_);
    if (!query.exec(str)) {
        emit error(query.lastError().text());
        return false;
    }

    QSqlRecord record = query.record();
    int idIndex = record.indexOf("id");
    int textEditorIndex = record.indexOf("text_editor");
    int fileFormatsIndex = record.indexOf("file_formats");
    int encodingIndex = record.indexOf("encoding");
    int hasIntellisenseIndex = record.indexOf("has_intellisense");
    int hasPluginsIndex = record.indexOf("has_plugins");
    int canCompileIndex = record.indexOf("can_compile");

    QVector<CodeEditor> editors;
    while (query.next()) {
        int id = query.value(idIndex).toInt();
        QString textEditor = query.value(textEditorIndex).toString();
        QString fileFormats = query.value(fileFormatsIndex).toString();
        QString encoding = query.value(encodingIndex).toString();
        bool hasIntellisense = query.value(hasIntellisenseIndex).toBool();
        bool hasPlugins = query.value(hasPluginsIndex).toBool();
        bool canCompile = query.value(canCompileIndex).toBool();

        auto editor = CodeEditor(id, textEditor, fileFormats, encoding, hasIntellisense, hasPlugins, canCompile);
        editors.push_back(editor);
    }

    emit loaded(editors);
    return true;
}

bool DatabaseWorker::import(QVector<CodeEditor> editors) {
    QString str = "INSERT INTO code_editors ("
                  "text_editor, file_formats, encoding, "
                  "has_intellisense, has_plugins, can_compile) "
                  "VALUES (?, ?, ?, ?, ?, ?)";

    QSqlQuery query(db_);
    if (!query.prepare(str)) {
        emit error(query.lastError().text());
        return false;
    }

    QVariantList textEditorValues;
    QVariantList fileFormatsValues;
    QVariantList encodingValues;
    QVariantList hasIntellisensevalues;
    QVariantList hasPluginsValues;
    QVariantList canCompilevalues;

    for (auto &editor: editors) {
        textEditorValues << editor.textEditor;
        fileFormatsValues << editor.fileFormats;
        encodingValues << editor.encoding;
        hasIntellisensevalues << editor.hasIntellisense;
        hasPluginsValues << editor.hasPlugins;
        canCompilevalues << editor.canCompile;
    }

    query.addBindValue(textEditorValues);
    query.addBindValue(fileFormatsValues);
    query.addBindValue(encodingValues);
    query.addBindValue(hasIntellisensevalues);
    query.addBindValue(hasPluginsValues);
    query.addBindValue(canCompilevalues);

    if (!query.execBatch()) {
        emit error(query.lastError().text());
        return false;
    }

    // Проще сразу заново загрузить все данные из бд.
    return load();
}

bool DatabaseWorker::clear() {
    QSqlQuery query(db_);
    if (!query.exec("DELETE FROM code_editors")) {
        emit error(query.lastError().text());
        return false;
    }

    return true;
}

