import Cutie
import CutieNotes
import QtQuick
import QtQuick.Controls

CutiePage {
	id: noteView

	// Set by whoever pushes this page (see main.qml's openNote()).
	property string noteId: ""

	// Guards scheduleSave() against the text assignment in
	// Component.onCompleted itself triggering a save.
	property bool loaded: false

	Component.onCompleted: {
		textArea.text = NotesManager.loadNote(noteView.noteId);
		noteView.loaded = true;
	}

	// Saves are debounced rather than firing on every keystroke, to avoid
	// hammering the filesystem while typing.
	Timer {
		id: saveTimer
		interval: 800
		onTriggered: NotesManager.saveNote(noteView.noteId, textArea.text)
	}

	function scheduleSave() {
		if (noteView.loaded)
			saveTimer.restart();
	}

	// Belt-and-braces for a fast back-swipe cutting off the debounce timer,
	// and discards an untouched new note instead of leaving a blank card
	// behind - same idea as most notes apps. noteId is blanked by the
	// explicit Delete action below so this doesn't re-save/re-delete after
	// that already ran.
	Component.onDestruction: {
		if (noteView.noteId.length === 0)
			return;
		if (textArea.text.trim().length === 0)
			NotesManager.deleteNote(noteView.noteId);
		else
			NotesManager.saveNote(noteView.noteId, textArea.text);
	}

	CutiePageHeader {
		id: header
		title: qsTr("Note")
		width: parent.width

		CutieButton {
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			anchors.rightMargin: 15
			icon.name: "edit-delete-symbolic"
			background: null
			onClicked: deleteMenu.open()
		}

		CutieMenu {
			id: deleteMenu
			CutieMenuItem {
				text: qsTr("Delete note")
				onTriggered: {
					NotesManager.deleteNote(noteView.noteId);
					noteView.noteId = "";
					mainWindow.pageStack.pop();
				}
			}
		}
	}

	Flickable {
		anchors.top: header.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		contentHeight: textArea.height
		clip: true

		TextArea {
			id: textArea
			width: parent.width
			wrapMode: TextArea.Wrap
			placeholderText: qsTr("Note")
			background: null
			onTextChanged: noteView.scheduleSave()
		}
	}
}
