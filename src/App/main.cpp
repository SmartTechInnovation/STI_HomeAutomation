#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlEngine>

#include <QThread>
#include <QObject>

#include "application.h"
#include "src/Components/ProjectManager/projectmanager.h"
#include "src/Components/BlockManager/blockmanager.h"
#include "src/Components/PeripheryManager/peripherymanager.h"
#include "src/Components/Logger/logger.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setOrganizationName(QStringLiteral("SmartTechInnovation"));
    app.setApplicationName("STIx Config");
    app.setApplicationVersion(QStringLiteral(PROJECT_VERSION_STR));

    Application_class AppRuntime;

    AppRuntime.begin();

    QQuickStyle::setStyle("Basic");

    qmlRegisterSingletonInstance("STI.ProjectManager", 1, 0,   "ProjectManager",   &S_ProjectManager);
    qmlRegisterSingletonInstance("STI.Logger",         1, 0,   "Logger",           &S_Logger);
    qmlRegisterUncreatableMetaObject(Log::staticMetaObject, "STI.Logger", 1, 0, "Log", "Log is an enum namespace");
    qmlRegisterSingletonInstance("STI.BlockManager",   1, 0,   "BlockManager",     &S_BlockManager);
    qmlRegisterSingletonInstance("STI.PeripheryManager", 1, 0, "PeripheryManager", &S_PeripheryManager);

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("STI_HomeAutomation", "Main");

    return app.exec();
}
