#pragma once

#include <QFileSystemWatcher>
#include <QObject>
#include <QVariantList>

// Backs the note grid on the main page. Notes are plain .txt files under
// ~/Documents/cutie-notes - no database, no custom format, so they're
// readable/editable/backed-up like any other document. The first line of
// each file is its title; QFileSystemWatcher picks up changes made outside
// the app (e.g. syncing the folder) and re-scans automatically, same idea
// as DriveManager watching /media et al. in cutie-explorer.
class NotesManager : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QVariantList notes READ notes NOTIFY notesChanged)

public:
	explicit NotesManager(QObject *parent = nullptr);

	QVariantList notes() const;

	// Creates an empty note file and returns its id (file name without
	// path), ready to hand straight to NoteView. Empty string on failure.
	Q_INVOKABLE QString createNote();

	// Full file contents for id, or empty string if it doesn't exist.
	Q_INVOKABLE QString loadNote(const QString &id) const;

	// Overwrites id's file with content.
	Q_INVOKABLE bool saveNote(const QString &id, const QString &content);

	Q_INVOKABLE bool deleteNote(const QString &id);

	// Re-reads the notes directory. Called automatically on startup and on
	// any change to the watched directory; exposed to QML too in case a
	// manual refresh is ever needed (mirrors DriveManager.refresh()).
	Q_INVOKABLE void refresh();

Q_SIGNALS:
	void notesChanged();

private:
	QString notesDirPath() const;
	QVariantMap noteEntry(const QString &fileName) const;

	QFileSystemWatcher m_watcher;
	QVariantList m_notes;
};
