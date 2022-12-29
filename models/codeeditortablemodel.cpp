#include "codeeditortablemodel.h"

#include <QFont>
#include <utility>

CodeEditorTableModel::CodeEditorTableModel(QObject *parent)
        : QAbstractTableModel(parent), data_(), worker_("QSQLITE", "db.sqlite") {

    connect(this, &CodeEditorTableModel::needToLoad, &worker_, &DatabaseWorker::load, Qt::QueuedConnection);
    connect(this, &CodeEditorTableModel::needToClear, &worker_, &DatabaseWorker::clear, Qt::QueuedConnection);
    connect(this, &CodeEditorTableModel::needToImport, &worker_, &DatabaseWorker::import, Qt::QueuedConnection);

    connect(&worker_, &DatabaseWorker::loaded, this, &CodeEditorTableModel::loadCallback, Qt::QueuedConnection);
    connect(&worker_, &DatabaseWorker::inserted, this, &CodeEditorTableModel::insertCallback, Qt::QueuedConnection);
    connect(&worker_, &DatabaseWorker::error, this, &CodeEditorTableModel::error, Qt::QueuedConnection);

    connect(this, &CodeEditorTableModel::toInsert, &worker_, &DatabaseWorker::insert, Qt::QueuedConnection);
    connect(this, &CodeEditorTableModel::toUpdate, &worker_, &DatabaseWorker::update, Qt::QueuedConnection);
    connect(this, &CodeEditorTableModel::toRemove, &worker_, &DatabaseWorker::remove, Qt::QueuedConnection);

    emit needToLoad();
}

QVariant CodeEditorTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return "Text editor";
            case 1:
                return "File formats";
            case 2:
                return "Encodings";
            case 3:
                return "Has intellisense";
            case 4:
                return "Has plugins";
            case 5:
                return "Can compile";
        }
    }

    if (role == Qt::FontRole && orientation == Qt::Horizontal) {
        QFont font;
        font.setBold(true);
        return font;
    }

    if (role == Qt::TextAlignmentRole && orientation == Qt::Horizontal) {
        return int(Qt::AlignCenter);
    }

    return {};
}

QVariant CodeEditorTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};
    if (!checkIndex(index)) return {};

    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignCenter);
    }

    if (role == Qt::DisplayRole) {
        auto &entry = data_[index.row()];
        switch (index.column()) {
            case 0:
                return entry.textEditor;
            case 1:
                return entry.fileFormats;
            case 2:
                return entry.encoding;
            case 3:
                return entry.hasIntellisense;
            case 4:
                return entry.hasPlugins;
            case 5:
                return entry.canCompile;
        }
    }

    return {};
}

bool CodeEditorTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role != Qt::EditRole) return false;
    if (!checkIndex(index)) return false;

    if (value.toString().isEmpty()) {
        emit error("Value must be not empty");
        return false;
    }

    auto &editor = data_[index.row()];
    switch (index.column()) {
        case 0:
            editor.textEditor = value.toString();
            break;
        case 1:
            editor.fileFormats = value.toString();
            break;
        case 2:
            editor.encoding = value.toString();
            break;
        case 3:
            editor.hasIntellisense = value.toBool();
            break;
        case 4:
            editor.hasPlugins = value.toBool();
            break;
        case 5:
            editor.canCompile = value.toBool();
            break;
        default:
            return false;
    }

    emit toUpdate(editor);
    return true;
}

int CodeEditorTableModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : data_.size();
}

int CodeEditorTableModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 6;
}

Qt::ItemFlags CodeEditorTableModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void CodeEditorTableModel::import(QVector<CodeEditor> editors) {
    emit needToImport(std::move(editors));
}

void CodeEditorTableModel::clear() {
    beginResetModel();
    data_.clear();
    endResetModel();

    emit needToClear();
}

void CodeEditorTableModel::insert(CodeEditor editor) {
    emit toInsert(std::move(editor));
}

void CodeEditorTableModel::remove(QModelIndex index) {
    if (!checkIndex(index)) return;

    int row = index.row();
    beginRemoveRows(QModelIndex(), row, row);
    int id = data_[row].id;
    data_.remove(row);
    endRemoveRows();

    emit toRemove(id);
}

void CodeEditorTableModel::exportTo(QDir dir) {
    auto editors = data_;
    editors.detach(); // Создаем копию данных
    emit exportData(dir, std::move(editors));
}

void CodeEditorTableModel::loadCallback(QVector<CodeEditor> editors) {
    beginResetModel();
    data_ = std::move(editors);
    endResetModel();
}

void CodeEditorTableModel::insertCallback(CodeEditor entry) {
    beginInsertRows(QModelIndex(), data_.size(), data_.size());
    data_.append(std::move(entry));
    endInsertRows();
}
