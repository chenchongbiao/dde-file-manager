/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuyangming<liuyangming@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             max-lv<lvwujun@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FILEMODELMANAGER_H
#define FILEMODELMANAGER_H

#include "dfmplugin_workspace_global.h"

#include <QAbstractItemView>

namespace dfmplugin_workspace {

class FileViewModel;
class FileSortFilterProxyModel;
class FileModelManager : public QObject
{
    Q_OBJECT
public:
    static FileModelManager *instance();
    FileSortFilterProxyModel *createProxyModel(QAbstractItemView *view);

    void refRootData(const QUrl &url);
    void derefRootData(const QUrl &url);

    bool needCacheData(const QString &scheme);
    void registerDataCache(const QString &scheme);

private:
    explicit FileModelManager(QObject *parent = nullptr);

    FileViewModel *sourceDataModel = nullptr;

    // scheme in cacheDataSchemes will cache data in source model
    QList<QString> cacheDataSchemes {};
    QMap<QUrl, int> dataRefMap {};
};

}

#endif   // FILEMODELMANAGER_H
