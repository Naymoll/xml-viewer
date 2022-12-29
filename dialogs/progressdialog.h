#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QObject>
#include <QDialog>

namespace Ui {
    class ProgressDialog;
}

class ProgressDialog : public QDialog {
Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);

    ~ProgressDialog() override;

public slots:

    void setValue(int progress);

    void append(QString message);

    void start(int max);

private:
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
