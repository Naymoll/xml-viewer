#include <QFileDialog>
#include "mainwindow.h"
#include "models/codeeditortablemodel.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), progressDialog_(this), insertDialog_(this), parserWorker_() {
    ui->setupUi(this);

    auto model = new CodeEditorTableModel(this);
    ui->tableView->setModel(model);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, &MainWindow::toRemove, model, &CodeEditorTableModel::remove);
    connect(this, &MainWindow::toParse, &parserWorker_, &ParserWorker::parse);

    connect(&insertDialog_, &CodeEditorDialog::success, model, &CodeEditorTableModel::insert);
    connect(ui->insertAction, &QAction::triggered, &insertDialog_, &QDialog::exec);
    connect(ui->removeAction, &QAction::triggered, this, &MainWindow::remove);
    connect(ui->importAction, &QAction::triggered, this, &MainWindow::import);
    connect(ui->clearAction, &QAction::triggered, model, &CodeEditorTableModel::clear);

    connect(&parserWorker_, &ParserWorker::started, &progressDialog_, &ProgressDialog::start);
    connect(&parserWorker_, &ParserWorker::progress, &progressDialog_, &ProgressDialog::setValue);
    connect(&parserWorker_, &ParserWorker::log, &progressDialog_, &ProgressDialog::append);
    connect(&parserWorker_, &ParserWorker::finished, model, &CodeEditorTableModel::import);

    connect(model, &CodeEditorTableModel::error, this, &MainWindow::showMessage);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::import() {
    QDir dir = QFileDialog::getExistingDirectory(this, "Open directory");
    emit toParse(dir);
}

void MainWindow::showMessage(QString error) {
    const int FIVE_SECOND_TIMEOUT = 5000;
    ui->statusBar->showMessage(error, FIVE_SECOND_TIMEOUT);
}

void MainWindow::remove() {
    auto selectionModel = ui->tableView->selectionModel();
    auto rows = selectionModel->selectedRows();
    if (rows.isEmpty()) return;

    emit toRemove(rows.first());
}

