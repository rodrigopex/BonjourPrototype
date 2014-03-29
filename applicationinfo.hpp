#ifndef VERSION_H
#define VERSION_H
/*
 * Auto-generated by the build_counter.py script.
 * Any changes made by user WILL BE LOST!
 */

#include <QObject>
//#include <bb/cascades/QmlDocument>
#include <QDir>
#define APPLICATION_VERSION_MAJOR 0
#define APPLICATION_VERSION_MINOR 2
#define APPLICATION_VERSION_MAINTENANCE 0
#define APPLICATION_VERSION_BUILD 1465

//using namespace bb::cascades;

class ApplicationInfo: public QObject {
    Q_OBJECT
public:
    ApplicationInfo(QObject * parent = 0): QObject(parent) {
    }
	Q_INVOKABLE QString version() {
	    return QString("0.2.0.1465");
	}
	Q_INVOKABLE QString currentPath() {
        return QString("file://" + QDir::currentPath());
    }
    Q_INVOKABLE QString homePath() {
        return QString("file://" + QDir::homePath());
    }
    Q_INVOKABLE QString tempPath() {
        return QString("file://" + QDir::tempPath());
    }
//    static void registerQmlTypes() {
//        qmlRegisterType<ApplicationInfo>("app.info", 1, 0, "ApplicationInfo");
//    }
//    static void addToRootContext(QString qmlName, QObject * obj) {
//        QDeclarativeEngine *engine = QmlDocument::defaultDeclarativeEngine();
//        QDeclarativeContext *rootContext = engine->rootContext();
//        rootContext->setContextProperty(qmlName, obj);
//    }
};

#endif //VERSION_H
