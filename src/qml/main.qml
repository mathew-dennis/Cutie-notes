import Cutie
import CutieNotes
import QtQuick
import "Formatting.js" as Formatting

CutieWindow {
	id: mainWindow
	width: 400
	height: 800
	visible: true
	title: qsTr("Notes")

	property var noteViewComponent: Qt.createComponent("NoteView.qml")

	function openNote(id) {
		if (mainWindow.noteViewComponent.status === Component.Ready)
			mainWindow.pageStack.push(mainWindow.noteViewComponent, { noteId: id });
	}

	function newNote() {
		var id = NotesManager.createNote();
		if (id.length > 0)
			mainWindow.openNote(id);
	}

	initialPage: CutiePage {
		width: mainWindow.width
		height: mainWindow.height

		CutiePageHeader {
			id: header
			title: mainWindow.title
			width: parent.width
		}

		// ── Fixed 2-column note grid ─────────────────────────────────────
		GridView {
			id: notesGrid
			anchors.top: header.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			anchors.margins: 8
			cellWidth: width / 2
			cellHeight: 150
			clip: true
			model: NotesManager.notes

			delegate: Item {
				id: cardRoot
				width: notesGrid.cellWidth
				height: notesGrid.cellHeight

				// Background is a separate item from the text, not a
				// parent - so its low opacity doesn't wash out the text
				// sitting on top of it.
				Rectangle {
					id: cardBg
					anchors.fill: parent
					anchors.margins: 6
					radius: 12
					color: Atmosphere.secondaryAlphaColor
					opacity: 0.8
				}

				Column {
					anchors.fill: cardBg
					anchors.margins: 12
					spacing: 4

					CutieLabel {
						width: parent.width
						text: modelData.title
						font.bold: true
						font.pixelSize: 16
						elide: Text.ElideRight
						maximumLineCount: 1
						color: Atmosphere.textColor
					}

					CutieLabel {
						width: parent.width
						text: modelData.preview
						wrapMode: Text.WordWrap
						maximumLineCount: 2
						elide: Text.ElideRight
						opacity:0.9
						font.pixelSize: 13
						color: Atmosphere.textColor
					}

					CutieLabel {
						width: parent.width
						text: Formatting.formatDate(modelData.modified)
						opacity: 0.9
						font.pixelSize: 11
						color: Atmosphere.textColor
					}
				}

				MouseArea {
					anchors.fill: parent
					onClicked: mainWindow.openNote(modelData.id)
				}
			}
		}

		CutieLabel {
			anchors.centerIn: parent
			visible: notesGrid.count === 0
			text: qsTr("No notes yet - tap + to add one")
			opacity: 0.5
		}

		// ── Floating action button ────────────────────────────────────────
		Rectangle {
			id: fab
			width: 56
			height: 56
			radius: 28
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			anchors.margins: 20
			color: "transparent"
			border.width: 2
			border.color: Atmosphere.textColor

			CutieLabel {
				anchors.centerIn: parent
				text: "+"
				font.pixelSize: 28
				color: Atmosphere.textColor
			}

			MouseArea {
				anchors.fill: parent
				onClicked: mainWindow.newNote()
			}
		}
	}
}
