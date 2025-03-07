// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "recenteventreceiver.h"
#include "utils/recentmanager.h"

#include <dfm-base/dfm_event_defines.h>

#include <dfm-framework/dpf.h>

#include <QDir>
#include <QTimer>

Q_DECLARE_METATYPE(QDir::Filters);
Q_DECLARE_METATYPE(QString *)

DFMBASE_USE_NAMESPACE
using namespace dfmplugin_recent;

RecentEventReceiver *RecentEventReceiver::instance()
{
    static RecentEventReceiver ins;
    return &ins;
}

void RecentEventReceiver::initConnect()
{
    dpfSignalDispatcher->subscribe("dfmplugin_titlebar", "signal_InputAdddressStr_Check", instance(), &RecentEventReceiver::handleAddressInputStr);
    dpfSignalDispatcher->subscribe(GlobalEventType::kChangeCurrentUrl, RecentEventReceiver::instance(), &RecentEventReceiver::handleWindowUrlChanged);
}

void RecentEventReceiver::handleAddressInputStr(quint64 winId, QString *str)
{
    Q_UNUSED(winId)

    if (str->startsWith(RecentHelper::scheme())) {
        str->clear();
        str->append(RecentHelper::scheme() + ":/");
    }
}

void RecentEventReceiver::handleWindowUrlChanged(quint64 winId, const QUrl &url)
{
    if (url.scheme() == RecentHelper::scheme()) {
        QTimer::singleShot(0, this, [=] {
            QDir::Filters f = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System | QDir::Hidden;
            dpfSlotChannel->push("dfmplugin_workspace", "slot_View_SetFilter", winId, f);
        });
    }
}

RecentEventReceiver::RecentEventReceiver(QObject *parent)
    : QObject(parent)
{
}
