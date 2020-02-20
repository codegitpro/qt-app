#include "directory_item_model.h"
#include "qt_plat.h"

namespace ai
{
    

directory_item_model::directory_item_model(QObject* parent) : QAbstractListModel (parent)
{
    role_names_[role::type] = "type";
    role_names_[role::local] = "local";
    role_names_[role::progress] = "progress";
    role_names_[role::name] = "name";

//    folder_segments_ = new text_map();
//    file_segments_ = new text_map();

//    file_segments_->push_back("hans.txt");
//    names_.push_back(0);
//    types_.push_back(directory_type::file);
//    localities_.push_back(locality::remote);
//    progresses_.push_back(70);

//    file_segments_->push_back("james.txt");
//    names_.push_back(1);
//    types_.push_back(directory_type::file);
//    localities_.push_back(locality::remote);
//    progresses_.push_back(30);
}

directory_item_model::~directory_item_model() {}

QHash<int, QByteArray> directory_item_model::roleNames() const
{
    return role_names_;
}

int directory_item_model::rowCount(const QModelIndex&) const
{
    return static_cast<int>(folders_.size() + files_.size());
}

QVariant directory_item_model::data(const QModelIndex &index, int role) const
{
    sz row = static_cast<sz>(index.row());
    if(folders_.size() + files_.size() <= row)
        return QVariant();

    if (row < folders_.size())
    {
        auto& folder = folders_[row];
        switch(role)
        {
        case role::name: return QString::fromStdString(folder.name);
        case role::local: return locality::local;
        case role::progress: return 0;
        case role::type: return directory_type::folder;
        }
    }
    else
    {
        auto& file = files_[row - folders_.size()];
        switch(role)
        {
        case role::name: return QString::fromStdString(file.name);
        case role::local: return locality::local;
        case role::progress: return file.percentage;
        case role::type: return directory_type::file;
        }
    }
    return QVariant();
}


void directory_item_model::folder_change(p4 folder_id, const directory& dir)
{
    auto folders = dir.list_directories(folder_id);
    auto files = dir.list_files(folder_id);

    run_on_main_thread([this, folders = move(folders), files = move(files)]()
    {
        emit beginResetModel();
        folders_.clear();
        files_.clear();
        emit endResetModel();

        auto count = files.size() + folders.size();
        if (!count)
            return;
        emit beginInsertRows(QModelIndex(), 0, count-1);
        folders_ = move(folders);
        files_ = move(files);
        emit endInsertRows();
    });
}

void directory_item_model::file_change(p4 file_id, const directory& dir)
{
    run_on_main_thread([this, file_id, &dir]()
    {
    });

}

void directory_item_model::report(severity, const str&)
{

}

}