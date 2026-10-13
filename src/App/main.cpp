#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlEngine>

#include <QThread>
#include <QObject>

#include "application.h"
#include "src/Components/ProjectManager/projectmanager.h"
#include "src/Components/BlockManager/blockmanager.h"
#include "src/Components/SchemaManager/schemamanager.h"
#include "src/Components/Logger/logger.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setOrganizationName(QStringLiteral("SmartTechInnovation"));
    app.setApplicationName("STIx Config");
    app.setApplicationVersion(QStringLiteral(PROJECT_VERSION_STR));

    Application.begin();
    Application.Organisation    = app.organizationName();
    Application.ApplicationName = app.applicationName();

    QQuickStyle::setStyle("Basic");

    qmlRegisterSingletonInstance("STI.Application",    1, 0, "Application",    &Application);
    qmlRegisterSingletonInstance("STI.ProjectManager", 1, 0, "ProjectManager", &S_ProjectManager);
    qmlRegisterSingletonInstance("STI.Logger",         1, 0, "Logger",         &S_Logger);
    qmlRegisterUncreatableMetaObject(Log::staticMetaObject, "STI.Logger", 1, 0, "Log", "Log is an enum namespace");
    qmlRegisterSingletonInstance("STI.BlockManager",   1, 0, "BlockManager",   &S_BlockManager);
    qmlRegisterSingletonInstance("STI.SchemaManager",  1, 0, "SchemaManager",  &S_SchemaManager);

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
