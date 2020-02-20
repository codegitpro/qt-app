#pragma once

#include "../../general.h"
#include "../../caching_engine.h"

#include <QtCore>
#include <QtGui>
#include <tricks/qqmlhelpers.h>
#include <core/directory.h>

namespace ai
{
    
QML_ENUM_CLASS (directory_type, file = 0, folder)
QML_ENUM_CLASS (locality, remote = 0, local)

namespace role
{
    enum role_name : int
    {
        type = Qt::UserRole,
        name = Qt::UserRole + 1,
        progress = Qt::UserRole + 2,
        local = Qt::UserRole + 3
    };
}

struct directory_item_model : QAbstractListModel, cache_callback
{
    Q_OBJECT

public:
    explicit directory_item_model(QObject* parent = nullptr);
    ~directory_item_model();
    int rowCount(const QModelIndex &parent) const final;
    QVariant data(const QModelIndex &index, int role) const final;
protected:
    QHash<int, QByteArray> roleNames() const final;
private:
    QHash<int, QByteArray> role_names_;

    arr<dir_item> folders_;
    arr<file_item> files_;

    void folder_change(p4 folder_id, const directory&) final;
    void file_change(p4 file_id, const directory&) final;
    void report(severity, const str&) final;

};

}
