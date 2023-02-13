// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEVICEUTILS_H
#define DEVICEUTILS_H

#include "dfm-base/dfm_base_global.h"

#include <QString>

#include <dfm-mount/base/dmountutils.h>

namespace dfmbase {

namespace BlockAdditionalProperty {
inline constexpr char kClearBlockProperty[] { "ClearBlockDeviceInfo" };
inline constexpr char kAliasGroupName[] { "LocalDiskAlias" };
inline constexpr char kAliasItemName[] { "Items" };
inline constexpr char kAliasItemUUID[] { "uuid" };
inline constexpr char kAliasItemAlias[] { "alias" };
}   // namespace BlockAdditionalProperty

inline constexpr char kBlockDeviceIdPrefix[] { "/org/freedesktop/UDisks2/block_devices/" };

/*!
 * \brief The DeviceUtils class
 * this class provide some util functions.
 */
class DeviceUtils
{
public:
    static QString getBlockDeviceId(const QString &deviceDesc);
    static QString getMountInfo(const QString &in, bool lookForMpt = true);
    static QUrl getSambaFileUriFromNative(const QUrl &url);
    static QString errMessage(DFMMOUNT::DeviceError err);
    static QString convertSuitableDisplayName(const QVariantMap &devInfo);
    static QString convertSuitableDisplayName(const QVariantHash &devInfo);
    static QString formatOpticalMediaType(const QString &media);
    static bool isAutoMountEnable();
    static bool isAutoMountAndOpenEnable();
    static bool isWorkingOpticalDiscDev(const QString &dev);
    static bool isWorkingOpticalDiscId(const QString &id);

    static bool isSamba(const QUrl &url);
    static bool isFtp(const QUrl &url);
    static bool isExternalBlock(const QUrl &url);

    static QMap<QString, QString> fstabBindInfo();

    static QString nameOfSystemDisk(const QVariantMap &datas);
    static QString nameOfOptical(const QVariantMap &datas);
    static QString nameOfEncrypted(const QVariantMap &datas);
    static QString nameOfDefault(const QString &label, const quint64 &size);
    static QString nameOfSize(const quint64 &size);

    static bool checkDiskEncrypted();
    static QStringList encryptedDisks();

    static bool isSubpathOfDlnfs(const QString &path);

private:
    static bool hasMatch(const QString &txt, const QString &rex);
};

}

#endif   // DEVICEUTILS_H
