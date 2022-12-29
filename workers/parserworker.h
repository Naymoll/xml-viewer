#ifndef PARSERWORKER_H
#define PARSERWORKER_H

#include <QObject>
#include <QDir>
#include <QThread>
#include "data/codeeditor.h"

class ParserWorker : public QObject {
Q_OBJECT
public:
    ParserWorker(const ParserWorker &other) = delete;

    ParserWorker &operator=(const ParserWorker &other) = delete;

    explicit ParserWorker(QObject *parent = nullptr);

    ~ParserWorker() override;

signals:
    void started(int max);

    void finished(QVector<CodeEditor> editors);

    void progress(int value);

    void log(QString message);

public slots:
    void parse(QDir dir);

private slots:
    QVariant parseFile(const QString &path);

    void cleanup();

private:
    QThread worker_;
};

#endif // PARSERWORKER_H
