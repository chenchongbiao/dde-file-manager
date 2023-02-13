// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "config.h"   //cmake

#include <DApplication>
#include <QDir>
#include <QTextCodec>

#include <dfm-framework/dpf.h>
#include <dfm-io/dfmio_register.h>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

#ifdef DFM_ORGANIZATION_NAME
#    define ORGANIZATION_NAME DFM_ORGANIZATION_NAME
#else
#    define ORGANIZATION_NAME "deepin"
#endif

static constexpr char kDialogPluginInterface[] { "org.deepin.plugin.filedialog" };
static constexpr char kFmPluginInterface[] { "org.deepin.plugin.filemanager" };
static constexpr char kCommonPluginInterface[] { "org.deepin.plugin.common" };

static constexpr char kDialogCorePluginName[] { "filedialogplugin-core" };
static constexpr char kDialogCoreLibName[] { "libfiledialogplugin-core.so" };
static constexpr char kDFMCorePluginName[] { "dfmplugin-core" };
static constexpr char kDFMCoreLibName[] { "libdfmplugin-core.so" };

static void initLog()
{
    dpfLogManager->registerConsoleAppender();
    dpfLogManager->registerFileAppender();
}

static void initEnv()
{
    // Note: x11 flag!!!
    qputenv("QT_QPA_PLATFORM", "xcb");

    //for qt5platform-plugins load DPlatformIntegration or DPlatformIntegrationParent
    if (qEnvironmentVariableIsEmpty("XDG_CURRENT_DESKTOP")) {
        qputenv("XDG_CURRENT_DESKTOP", "Deepin");
    }

    if (qEnvironmentVariable("CLUTTER_IM_MODULE") == QStringLiteral("fcitx")) {
        setenv("QT_IM_MODULE", "fcitx", 1);
    }
}

static bool singlePluginLoad(const QString &pluginName, const QString &libName)
{
    auto plugin = DPF_NAMESPACE::LifeCycle::pluginMetaObj(pluginName);
    if (plugin.isNull())
        return false;
    if (!plugin->fileName().contains(libName))
        return false;
    if (!DPF_NAMESPACE::LifeCycle::loadPlugin(plugin))
        return false;

    return true;
}

static bool pluginsLoad()
{
    dpfCheckTimeBegin();

    static const QStringList blackNameList {
        "dfmplugin-burn",
        "dfmplugin-dirshare",
        "dfmplugin-myshares",
        "dfmplugin-propertydialog",
        "dfmplugin-trashcore",
        "dfmplugin-trash",
        "dfmplugin-smbbrowser",
        "dfmplugin-filepreview",
        "dfmplugin-vault"
    };

    QString pluginsDir(qApp->applicationDirPath() + "/../../plugins");
    QStringList pluginsDirs;
    if (!QDir(pluginsDir).exists()) {
        qInfo() << QString("Path does not exist, use path : %1").arg(DFM_PLUGIN_COMMON_CORE_DIR);
        pluginsDirs << QString(DFM_PLUGIN_COMMON_CORE_DIR)
                    << QString(DFM_PLUGIN_FILEMANAGER_CORE_DIR)
                    << QString(DFM_PLUGIN_COMMON_EDGE_DIR)
                    << QString(DFM_PLUGIN_FILEMANAGER_EDGE_DIR);
    } else {
        pluginsDirs.push_back(pluginsDir + "/filemanager");
        pluginsDirs.push_back(pluginsDir + "/common");
        pluginsDirs.push_back(pluginsDir);
    }

    qDebug() << "using plugins dir:" << pluginsDirs;
    DPF_NAMESPACE::LifeCycle::initialize({ kDialogPluginInterface,
                                           kFmPluginInterface,
                                           kCommonPluginInterface },
                                         pluginsDirs, blackNameList);

    qInfo() << "Depend library paths:" << DApplication::libraryPaths();
    qInfo() << "Load plugin paths: " << dpf::LifeCycle::pluginPaths();

    // read all plugins in setting paths
    if (!DPF_NAMESPACE::LifeCycle::readPlugins())
        return false;

    // We should make sure that the core plugin is loaded first
    if (!singlePluginLoad(kDialogCorePluginName, kDialogCoreLibName)) {
        qWarning() << "Load" << kDialogCorePluginName << "failed";
        return false;
    }
    if (!singlePluginLoad(kDFMCorePluginName, kDFMCoreLibName)) {
        qWarning() << "Load" << kDFMCorePluginName << "failed";
        return false;
    }

    // load plugins without core
    if (!DPF_NAMESPACE::LifeCycle::loadPlugins())
        return false;

    dpfCheckTimeEnd();

    return true;
}

int main(int argc, char *argv[])
{
    initEnv();

    // Fixed the locale codec to utf-8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    DApplication a(argc, argv);
    a.setOrganizationName(ORGANIZATION_NAME);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.setQuitOnLastWindowClosed(false);
    a.setProperty("GTK", true);   // see: FileDialogHandle::winId()

    {
        // load translation
        auto appName = a.applicationName();
        a.setApplicationName("dde-file-manager");
        a.loadTranslator();
        a.setApplicationName(appName);
    }

    DPF_NAMESPACE::backtrace::installStackTraceHandler();
    initLog();
    DFMIO::dfmio_init();

    if (!pluginsLoad()) {
        qCritical() << "Load pugin failed!";
        abort();
    }

    int ret { a.exec() };
    DPF_NAMESPACE::LifeCycle::shutdownPlugins();
    return ret;
}
