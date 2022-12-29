#include "progressdialog.h"
#include "ui_progressdialog.h"

#include <QPushButton>
#include <QProgressBar>

ProgressDialog::ProgressDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::ProgressDialog) {
    ui->setupUi(this);
}

ProgressDialog::~ProgressDialog() {
    delete ui;
}


void ProgressDialog::setValue(int progress) {
    ui->progressBar->setValue(progress);
}

void ProgressDialog::append(QString message) {
    ui->logs->append(message);
}

void ProgressDialog::start(int max) {
    ui->progressBar->reset();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(max);
    exec();
}
