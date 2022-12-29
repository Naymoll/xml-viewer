#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QTableView>
#include <QDir>

#include "dialogs/progressdialog.h"
#include "data/codeeditor.h"
#include "dialogs/codeeditordialog.h"
#include "workers/xmlworker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

signals:

    void toImport(QVector<CodeEditor> entries);

    void toRemove(QModelIndex index);

    void toParse(QDir dir);
    void exportTo(QDir dir);

public slots:

    void importEditors();
    void exportEditors();

    void showMessage(QString error);

    void remove();

private:
    Ui::MainWindow *ui;

    ProgressDialog progressDialog_;
    CodeEditorDialog insertDialog_;
    XmlWorker parserWorker_;
};

#endif // MAINWINDOW_H
