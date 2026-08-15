#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include <QTranslator>

#include "notesmanager.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	QGuiApplication app(argc, argv);
	QString locale = QLocale::system().name();
	QTranslator translator;
	(void)translator.load(QString(":/i18n/cutie-notes_") + locale);
	app.installTranslator(&translator);

	// Plain C++ singleton, exposed the same way DriveManager/FileOperations
	// are in cutie-explorer, so QML reaches it as `NotesManager.notes` /
	// `NotesManager.createNote(...)` without instantiating anything.
	qmlRegisterSingletonType<NotesManager>("CutieNotes", 1, 0, "NotesManager",
		[](QQmlEngine *, QJSEngine *) -> QObject * { return new NotesManager(); });

	QQmlApplicationEngine engine;
	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(
		&engine, &QQmlApplicationEngine::objectCreated, &app,
		[url](QObject *obj, const QUrl &objUrl) {
			if (!obj && url == objUrl)
				QCoreApplication::exit(-1);
		},
		Qt::QueuedConnection);
	engine.load(url);
	return app.exec();
}
