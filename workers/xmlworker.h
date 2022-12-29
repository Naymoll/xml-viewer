#ifndef XMLWORKER_H
#define XMLWORKER_H

#include <QObject>
#include <QDir>
#include <QThread>
#include "data/codeeditor.h"

class XmlWorker : public QObject {
Q_OBJECT
public:
    XmlWorker(const XmlWorker &other) = delete;

    XmlWorker &operator=(const XmlWorker &other) = delete;

    explicit XmlWorker(QObject *parent = nullptr);

    ~XmlWorker() override;

signals:
    void started(int max);

    void imported(QVector<CodeEditor> editors);
    void exported();

    void progress(int value);

    void log(QString message);

public slots:
    void importEditors(QDir dir);
    void exportEditors(QDir dir, QVector<CodeEditor> editors);

private slots:
    QVariant importFile(const QString &path);

    void cleanup();

private:
    QThread worker_;
};

#endif // XMLWORKER_H
