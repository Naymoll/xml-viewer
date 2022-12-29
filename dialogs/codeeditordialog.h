#ifndef CODEEDITORDIALOG_H
#define CODEEDITORDIALOG_H

#include <QDialog>
#include "data/codeeditor.h"

namespace Ui {
    class CodeEditorDialog;
}

class CodeEditorDialog : public QDialog {
Q_OBJECT

public:
    explicit CodeEditorDialog(QWidget *parent = nullptr);

    ~CodeEditorDialog() override;

signals:
    void success(CodeEditor codeEditor);

public slots:
    void verify();
    void editor();

private:
    Ui::CodeEditorDialog *ui;
};

#endif
