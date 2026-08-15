#include "notesmanager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>

#include <algorithm>

NotesManager::NotesManager(QObject *parent)
	: QObject(parent)
{
	QDir().mkpath(notesDirPath());

	m_watcher.addPath(notesDirPath());
	connect(&m_watcher, &QFileSystemWatcher::directoryChanged,
		this, &NotesManager::refresh);

	refresh();
}

QString NotesManager::notesDirPath() const
{
	// QStandardPaths rather than Qt.labs.platform on the QML side - this
	// needs resolving on the C++ side anyway to create the directory, and
	// it's the same XDG "Documents" location either way.
	const QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	return QDir(docs).filePath(QStringLiteral("cutie-notes"));
}

QVariantList NotesManager::notes() const
{
	return m_notes;
}

QVariantMap NotesManager::noteEntry(const QString &fileName) const
{
	QFileInfo info(QDir(notesDirPath()).filePath(fileName));

	// First line is the title; up to three more make up the preview shown
	// on the card. Deliberately not reading the whole file here - refresh()
	// runs on every directory change, so this stays cheap even with a lot
	// of notes.
	QString firstLine;
	QStringList previewLines;
	QFile file(info.filePath());
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream stream(&file);
		firstLine = stream.readLine();
		while (!stream.atEnd() && previewLines.size() < 3)
			previewLines.append(stream.readLine());
		file.close();
	}

	QVariantMap entry;
	entry["id"] = fileName;
	entry["title"] = firstLine.trimmed().isEmpty() ? tr("New note") : firstLine.trimmed();
	entry["preview"] = previewLines.join(QStringLiteral(" ")).trimmed();
	entry["modified"] = info.lastModified();
	return entry;
}

void NotesManager::refresh()
{
	QVariantList result;

	// Sorted explicitly by lastModified rather than relying on
	// QDir::Time - that flag's default order (oldest vs. newest first)
	// isn't worth trusting from memory when it's this easy to just sort
	// the QFileInfoList directly.
	QDir dir(notesDirPath());
	QFileInfoList files = dir.entryInfoList(QStringList() << QStringLiteral("*.txt"), QDir::Files);
	std::sort(files.begin(), files.end(), [](const QFileInfo &a, const QFileInfo &b) {
		return a.lastModified() > b.lastModified();
	});

	for (const QFileInfo &info : files)
		result.append(noteEntry(info.fileName()));

	if (result != m_notes) {
		m_notes = result;
		Q_EMIT notesChanged();
	}
}

QString NotesManager::createNote()
{
	const QString id = QStringLiteral("note-%1.txt")
		.arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss-zzz")));

	QFile file(QDir(notesDirPath()).filePath(id));
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return QString();
	file.close();

	refresh();
	return id;
}

QString NotesManager::loadNote(const QString &id) const
{
	QFile file(QDir(notesDirPath()).filePath(id));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return QString();

	QTextStream stream(&file);
	return stream.readAll();
}

bool NotesManager::saveNote(const QString &id, const QString &content)
{
	QFile file(QDir(notesDirPath()).filePath(id));
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QTextStream stream(&file);
	stream << content;
	file.close();

	refresh();
	return true;
}

bool NotesManager::deleteNote(const QString &id)
{
	const bool ok = QFile::remove(QDir(notesDirPath()).filePath(id));
	if (ok)
		refresh();
	return ok;
}
