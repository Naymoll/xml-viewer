#include "dialogs/codeeditordialog.h"
#include "ui_codeeditordialog.h"

#include <QPushButton>

CodeEditorDialog::CodeEditorDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::CodeEditorDialog) {
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CodeEditorDialog::editor);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &CodeEditorDialog::close);

    connect(ui->textEditorInput,  &QLineEdit::textChanged, this, &CodeEditorDialog::verify);
    connect(ui->fileformatsInput, &QLineEdit::textChanged, this, &CodeEditorDialog::verify);
    connect(ui->encodingInput,    &QLineEdit::textChanged, this, &CodeEditorDialog::verify);
}

CodeEditorDialog::~CodeEditorDialog() {
    delete ui;
}

void CodeEditorDialog::verify() {
    QString textEditor = ui->textEditorInput->text();
    QString fileFormats = ui->fileformatsInput->text();
    QString encoding = ui->encodingInput->text();

    bool isEmpty = textEditor.isEmpty() || fileFormats.isEmpty() || encoding.isEmpty();
    if (!isEmpty) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    } else {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

void CodeEditorDialog::editor() {
    QString textEditor = ui->textEditorInput->text();
    QString fileFormats = ui->fileformatsInput->text();
    QString encoding = ui->encodingInput->text();

    auto entry = CodeEditor(-1,
                            textEditor,
                            fileFormats,
                            encoding,
                            ui->hasIntellisenseButton->isChecked(),
                            ui->hasPluginsButton->isChecked(),
                            ui->canCompileButton->isChecked());
    emit success(std::move(entry));
    accept();
}
