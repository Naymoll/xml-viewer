#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

#include "data/codeeditor.h"

#include <memory>

#include <QObject>
#include <QThread>
#include <QSqlDatabase>

class DatabaseWorker : public QObject {
Q_OBJECT
public:
    DatabaseWorker() = delete;

    DatabaseWorker(const DatabaseWorker &other) = delete;

    DatabaseWorker &operator=(const DatabaseWorker &other) = delete;

    ~DatabaseWorker() override;

    explicit DatabaseWorker(const QString &type, const QString &name, QObject *parent = nullptr);

signals:
    void inserted(CodeEditor editor);

    void loaded(QVector<CodeEditor> editors);

    void error(QString error);

public slots:
    bool insert(CodeEditor editor);

    bool update(CodeEditor editor);

    bool remove(int id);

    bool load();

    bool import(QVector<CodeEditor> editors);

    bool clear();

private slots:
    bool start();

    void cleanup();

private:
    QSqlDatabase db_;
    QThread worker_;
};

#endif // DATABASEWORKER_H
