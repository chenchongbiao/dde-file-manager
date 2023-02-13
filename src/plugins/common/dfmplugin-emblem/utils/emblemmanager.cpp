// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "emblemmanager.h"
#include "emblemhelper.h"
#include "events/emblemeventsequence.h"

#include "dfm-base/interfaces/abstractfileinfo.h"
#include "dfm-base/base/schemefactory.h"
#include "dfm-base/utils/fileutils.h"

#include <QPainter>

DFMBASE_USE_NAMESPACE
DFMGLOBAL_USE_NAMESPACE
DPEMBLEM_USE_NAMESPACE

EmblemManager::EmblemManager(QObject *parent)
    : QObject(parent),
      helper(new EmblemHelper(this))
{
}

EmblemManager *EmblemManager::instance()
{
    static EmblemManager ins;
    return &ins;
}

bool EmblemManager::paintEmblems(int role, const QUrl &url, QPainter *painter, QRectF *paintArea)
{
    if (role != kItemIconRole)
        return false;

    const QList<QRectF> &paintRects = EmblemManager::instance()->caculatePaintRectList(*paintArea);
    const QList<QIcon> &emblems = EmblemManager::instance()->fetchEmblems(url);

    for (int i = 0; i < qMin(paintRects.count(), emblems.count()); ++i) {
        if (emblems.at(i).isNull())
            continue;

        emblems.at(i).paint(painter, paintRects.at(i).toRect());
    }

    return false;
}

QList<QIcon> EmblemManager::fetchEmblems(const QUrl &url) const
{
    AbstractFileInfoPointer info = InfoFactory::create<AbstractFileInfo>(url);
    if (!info)
        return {};

    QList<QIcon> emblemList;

    emblemList = helper->getSystemEmblems(info);

    if (FileUtils::isGvfsFile(url))
        return emblemList;

    // add gio emblem icons
    const auto &gioEmblemsMap = helper->getGioEmblems(info);
    QMap<int, QIcon>::const_iterator iter = gioEmblemsMap.begin();
    while (iter != gioEmblemsMap.end()) {
        if (iter.key() == emblemList.count()) {
            emblemList.append(iter.value());
            ++iter;
            continue;
        }

        if (iter.key() > emblemList.count()) {
            emblemList.append(QIcon());
            continue;
        }

        if (emblemList.at(iter.key()).isNull())
            emblemList.replace(iter.key(), iter.value());

        ++iter;
    }

    // add custom emblem icons
    EmblemEventSequence::instance()->doFetchCustomEmblems(url, &emblemList);

    // add extension lib emblem icons
    EmblemEventSequence::instance()->doFetchExtendEmblems(url, &emblemList);

    return emblemList;
}

QList<QRectF> EmblemManager::caculatePaintRectList(const QRectF &paintArea) const
{
    QSizeF baseEmblemSize = paintArea.size() / 3;
    baseEmblemSize.setWidth(qBound(kMinEmblemSize, baseEmblemSize.width(), kMaxEmblemSize));
    baseEmblemSize.setHeight(qBound(kMinEmblemSize, baseEmblemSize.width(), kMaxEmblemSize));

    double offset = paintArea.width() / 8;
    const QSizeF &offsetSize = baseEmblemSize / 2;

    QList<QRectF> list;
    list.append(QRectF(QPointF(paintArea.right() - offset - offsetSize.width(),
                               paintArea.bottom() - offset - offsetSize.height()),
                       baseEmblemSize));
    list.append(QRectF(QPointF(paintArea.left() + offset - offsetSize.width(), list.first().top()), baseEmblemSize));
    list.append(QRectF(QPointF(list.at(1).left(), paintArea.top() + offset - offsetSize.height()), baseEmblemSize));
    list.append(QRectF(QPointF(list.first().left(), list.at(2).top()), baseEmblemSize));

    return list;
}
