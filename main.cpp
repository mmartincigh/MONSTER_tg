#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QFile>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "applicationmanager.h"
#include "applicationutils.h"

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
#endif

    qInstallMessageHandler(LogManager::messageHandler);
    qRegisterMetaType<Enums::State>("Enums::State");
    qmlRegisterType<Enums>("MONSTER.ThumbnailGenerator_GUI", 1, 0, "Enums");

    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QApplication application(argc, argv);
    application.setOrganizationName(ApplicationUtils::COMPANY_NAME);
    application.setOrganizationDomain(ApplicationUtils::COMPANY_WEBSITE);
    application.setApplicationName(ApplicationUtils::APPLICATION_NAME);
    application.setApplicationVersion(ApplicationUtils::APPLICATION_VERSION);

    QPixmap pixmap(":/images/application/splashScreen");
    QSplashScreen splash_screen(pixmap);
    splash_screen.show();
    application.processEvents();

    QFile log_file(LogManager::LOG_FILENAME);
    log_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Append);
    QTextStream text_stream(&log_file);
    text_stream << LogManager::LOG_HEADER << endl << flush;
    log_file.close();

    ApplicationManager application_manager(&application);
    application_manager.initialize();

    QQmlApplicationEngine engine;
    QObject::connect(&engine, SIGNAL(quit()), &application, SLOT(quit()));

    QQmlComponent component(&engine);
    QQmlContext *context = engine.rootContext();
    context->setContextProperty("application", &application);
    context->setContextProperty("applicationManager", &application_manager);
    context->setContextProperty("sourceManager", application_manager.sourceManager());
    context->setContextProperty("destinationManager", application_manager.destinationManager());
    context->setContextProperty("thumbnailGenerator", application_manager.thumbnailGenerator());
    context->setContextProperty("fileManager", application_manager.fileManager());
    context->setContextProperty("windowManager", application_manager.windowManager());
    engine.addImageProvider("imageProvider", application_manager.imageProvider());
    component.loadUrl(QUrl("qrc:/qml/main.qml"));
    if (!component.isReady())
    {
        qCritical() << "GUI> ERROR:" << qPrintable(component.errorString().trimmed());

        return EXIT_FAILURE;
    }
    QObject *root = component.beginCreate(context);
    QObject::connect(root, SIGNAL(completed()), &splash_screen, SLOT(close()));
    QObject::connect(root, SIGNAL(completed()), &application_manager, SLOT(onCompleted()));
    component.completeCreate();

    return application.exec();
}
