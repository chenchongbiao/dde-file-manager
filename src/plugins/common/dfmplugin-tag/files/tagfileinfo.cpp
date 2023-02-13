// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tagfileinfo.h"
#include "utils/tagmanager.h"
#include "tagfileinfo_p.h"

#include "dfm-base/interfaces/private/abstractfileinfo_p.h"
#include "dfm-base/base/schemefactory.h"
#include "dfm-base/utils/fileutils.h"

DFMBASE_USE_NAMESPACE
using namespace dfmplugin_tag;

TagFileInfo::TagFileInfo(const QUrl &url)
    : AbstractFileInfo(url), d(new TagFileInfoPrivate(url, this))
{
    dptr.reset(d);
    if (!localFilePath().isEmpty())
        setProxy(InfoFactory::create<AbstractFileInfo>(QUrl::fromLocalFile(localFilePath())));
}

TagFileInfo::~TagFileInfo()
{
}

bool TagFileInfo::exists() const
{
    if (d->proxy) {
        // TODO(liuyangming): handle path in sql
        if (!localFilePath().startsWith("/home/")
            && !localFilePath().startsWith(FileUtils::bindPathTransform("/home/", true))
            && !localFilePath().startsWith("/media/"))
            return false;
        return d->proxy->exists();
    }

    QUrl rootUrl;
    rootUrl.setScheme(TagManager::scheme());
    if (urlOf(UrlInfoType::kUrl) == rootUrl)
        return true;

    const QMap<QString, QColor> &tagMap = TagManager::instance()->getAllTags();

    return tagMap.contains(tagName());
}

QFileDevice::Permissions TagFileInfo::permissions() const
{
    if (d->proxy)
        return d->proxy->permissions();

    return QFile::ReadGroup | QFile::ReadOwner | QFile::ReadUser | QFile::ReadOther
            | QFile::WriteGroup | QFile::WriteOwner | QFile::WriteUser | QFile::WriteOther;
}

bool TagFileInfo::isAttributes(const OptInfoType type) const
{
    switch (type) {
    case FileIsType::kIsDir:
        if (d->proxy)
            return d->proxy->isAttributes(type);

        return true;
    case FileIsType::kIsReadable:
        if (d->proxy)
            return d->proxy->isAttributes(type);

        return true;
    case FileIsType::kIsWritable:
        if (d->proxy)
            return d->proxy->isAttributes(type);

        return true;
    default:
        return AbstractFileInfo::isAttributes(type);
    }
}

QString TagFileInfo::nameOf(const NameInfoType type) const
{
    switch (type) {
    case NameInfoType::kFileName:
    case NameInfoType::kFileCopyName:
        return d->fileName();
    default:
        return AbstractFileInfo::nameOf(type);
    }
}

QString TagFileInfo::displayOf(const DisPlayInfoType type) const
{
    if (DisPlayInfoType::kFileDisplayName == type)
        return d->fileName();
    return AbstractFileInfo::displayOf(type);
}

AbstractFileInfo::FileType TagFileInfo::fileType() const
{
    if (d->proxy)
        return d->proxy->fileType();

    return FileType::kDirectory;
}

QIcon TagFileInfo::fileIcon()
{
    if (d->proxy)
        return d->proxy->fileIcon();

    return QIcon::fromTheme("folder");
}

QString TagFileInfo::localFilePath() const
{
    return urlOf(UrlInfoType::kUrl).fragment(QUrl::FullyDecoded);
}

QString TagFileInfo::tagName() const
{
    return d->fileName();
}

TagFileInfoPrivate::TagFileInfoPrivate(const QUrl &url, AbstractFileInfo *qq)
    : AbstractFileInfoPrivate(url, qq)
{
}

TagFileInfoPrivate::~TagFileInfoPrivate()
{
}

QString TagFileInfoPrivate::fileName() const
{
    if (proxy)
        return proxy->nameOf(NameInfoType::kFileName);

    return url.path().mid(1, url.path().length() - 1);
}
