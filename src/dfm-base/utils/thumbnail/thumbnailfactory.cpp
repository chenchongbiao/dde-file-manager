// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "thumbnailfactory.h"
#include "thumbnailcreators.h"
#include "thumbnailhelper.h"

#include <dfm-base/base/schemefactory.h>
#include <dfm-base/utils/universalutils.h>
#include <dfm-base/base/device/deviceproxymanager.h>

#include <QGuiApplication>

using namespace dfmbase;
DFMGLOBAL_USE_NAMESPACE

ThumbnailFactory::ThumbnailFactory(QObject *parent)
    : QObject(parent),
      thread(new QThread),
      worker(new ThumbnailWorker)
{
    registerThumbnailCreator(Mime::kTypeImageVDjvu, ThumbnailCreators::djvuThumbnailCreator);
    registerThumbnailCreator(Mime::kTypeImageVDMultipage, ThumbnailCreators::djvuThumbnailCreator);
    registerThumbnailCreator(Mime::kTypeTextPlain, ThumbnailCreators::textThumbnailCreator);
    registerThumbnailCreator(Mime::kTypeAppPdf, ThumbnailCreators::pdfThumbnailCreator);
    registerThumbnailCreator(Mime::kTypeAppVRRMedia, ThumbnailCreators::videoThumbnailCreatorFfmpeg);
    registerThumbnailCreator("image/*", ThumbnailCreators::imageThumbnailCreator);
    registerThumbnailCreator("audio/*", ThumbnailCreators::audioThumbnailCreator);
    registerThumbnailCreator("video/*", ThumbnailCreators::videoThumbnailCreator);

    init();
}

ThumbnailFactory::~ThumbnailFactory()
{
    if (thread->isRunning())
        onAboutToQuit();
}

void ThumbnailFactory::init()
{
    Q_ASSERT(qApp->thread() == QThread::currentThread());

    connect(qApp, &QGuiApplication::aboutToQuit, this, &ThumbnailFactory::onAboutToQuit);

    connect(this, &ThumbnailFactory::addTask, worker.data(), &ThumbnailWorker::onTaskAdded, Qt::QueuedConnection);
    connect(worker.data(), &ThumbnailWorker::thumbnailCreateFinished, this, &ThumbnailFactory::produceFinished, Qt::QueuedConnection);
    connect(worker.data(), &ThumbnailWorker::thumbnailCreateFailed, this, &ThumbnailFactory::produceFailed, Qt::QueuedConnection);

    worker->moveToThread(thread.data());
    thread->start();
}

void ThumbnailFactory::joinThumbnailJob(const QUrl &url, ThumbnailSize size)
{
    emit addTask(url, size);
}

bool ThumbnailFactory::registerThumbnailCreator(const QString &mimeType, ThumbnailCreator creator)
{
    Q_ASSERT(creator);
    return worker->registerCreator(mimeType, creator);
}

void ThumbnailFactory::onAboutToQuit()
{
    worker->stop();
    thread->quit();
    thread->wait(3000);
}
