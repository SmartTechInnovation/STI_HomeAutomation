#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlEngine>

#include <QThread>
#include <QObject>

#include "application.h"
#include "src/Components/BlockManager/blockmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Application_class AppRuntime;

    AppRuntime.begin();

    QQuickStyle::setStyle("Basic");

    app.setOrganizationName(QStringLiteral("Smart Tech Innovation"));
    app.setApplicationName("STI Home Automation");

    /* BlockManager disponibil in QML: import STI.Blocks -> BlockManager */
    qmlRegisterSingletonInstance("STI.BlockManager", 1, 0, "BlockManager", &S_BlockManager);

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
