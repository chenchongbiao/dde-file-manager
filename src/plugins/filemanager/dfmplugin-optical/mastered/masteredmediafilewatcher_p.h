/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangsheng<zhangsheng@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             xushitong<xushitong@uniontech.com>
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
#ifndef MASTEREDMEDIAFILEWATCHER_P_H
#define MASTEREDMEDIAFILEWATCHER_P_H

#include "dfmplugin_optical_global.h"
#include "dfm-base/interfaces/abstractfilewatcher.h"
#include "dfm-base/interfaces/private/abstractfilewatcher_p.h"

#include <QPointer>

DPOPTICAL_BEGIN_NAMESPACE

class MasteredMediaFileWatcher;
class MasteredMediaFileWatcherPrivate : public DFMBASE_NAMESPACE::AbstractFileWatcherPrivate
{
    friend MasteredMediaFileWatcher;

public:
    explicit MasteredMediaFileWatcherPrivate(const QUrl &fileUrl, MasteredMediaFileWatcher *qq);

public:
    bool start() override;
    bool stop() override;

    AbstractFileWatcherPointer proxyStaging;
    AbstractFileWatcherPointer proxyOnDisk;
};

DPOPTICAL_END_NAMESPACE

#endif   // MASTEREDMEDIAFILEWATCHER_P_H
