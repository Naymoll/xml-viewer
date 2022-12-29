#ifndef CODEEDITORTABLEMODEL_H
#define CODEEDITORTABLEMODEL_H

#include "data/codeeditor.h"
#include "workers/databaseworker.h"

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QObject>
#include <QVector>

class CodeEditorTableModel : public QAbstractTableModel {
Q_OBJECT
public:
    explicit CodeEditorTableModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

signals:
    void needToLoad();

    void needToClear();

    void needToImport(QVector<CodeEditor> editors);

    void toInsert(CodeEditor editor);

    void toUpdate(CodeEditor editor);

    void toRemove(int id);

    void error(QString text);

public slots:
    void import(QVector<CodeEditor> editors);

    void clear();

    void insert(CodeEditor editor);

    void remove(QModelIndex index);

private slots:
    void loadCallback(QVector<CodeEditor> editors);

    void insertCallback(CodeEditor editor);

private:
    QVector<CodeEditor> data_;
    DatabaseWorker worker_;
};

#endif // CODEEDITORTABLEMODEL_H
