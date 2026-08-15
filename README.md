# Cutie Notes

<img src="cutie-notes.svg" width="100px">

A quick-notes app for Cutie Shell. Fixed two-column grid of notes on the
main page, tap a card to open it full-screen for editing via `pageStack`,
FAB to add a new one.

## Building and installing

```
mkdir build
cd build
cmake ..
make
sudo make install
```

## Architecture

- Notes are plain `.txt` files under `~/Documents/cutie-notes` - no
  database, no custom format, so they're readable/editable/backed-up like
  any other document.
- `NotesManager` (C++ singleton, same pattern as `DriveManager` in
  cutie-explorer) watches that directory with `QFileSystemWatcher` and
  exposes a `notes` list (`id`, `title`, `preview`, `modified`) built by
  reading just the first few lines of each file, not the whole thing.
- A note's title is its first line; the next few lines become the card's
  preview. An empty file shows as "New note" until something's typed.
- `NoteView.qml` loads the full file content on open, debounces saves
  (800ms after the last keystroke) rather than saving on every keypress,
  and also saves/deletes on `Component.onDestruction` as a backstop for a
  fast back-swipe cutting off the debounce timer.
- An untouched new note (still empty when you leave it) is deleted rather
  than left behind as a blank card.
- Grid is a fixed 2-column `GridView`, not masonry - cards are a constant
  150px tall with title (1 line) + preview (2 lines, elided) + date.

## Things to verify when you compile

- `mainWindow.pageStack.push(component, {noteId: id})` /
  `mainWindow.pageStack.pop()` - copied from how `FolderView.qml` in
  cutie-explorer does it, including reaching the `mainWindow` id declared
  in `main.qml` from a page pushed onto the stack. Same trick, not
  independently re-verified here.
- `Atmosphere.secondaryAlphaColor` as the card background - used in
  cutie-explorer for a divider line and a usage-bar track, so it should
  read as a neutral tone at low opacity, but worth eyeballing against the
  screenshot you shared.
- `TextArea { background: null }` to drop QQC2's default TextArea chrome -
  same trick as `background: null` on `CutieButton` elsewhere, not
  confirmed on `TextArea` specifically.
- `qml6-module-cutie` is the only QML module dependency listed in
  `debian/control` - `QtQuick.Controls` (used for `TextArea`/`CutieMenu`'s
  underlying `Menu`) wasn't listed as a separate dependency in
  cutie-explorer's control file either despite being used there, so
  presumably it's pulled in transitively; flagging in case that assumption
  is wrong.
- `Component.onDestruction` firing reliably on pop (vs. the page being
  cached/reused) - assumed based on default `StackView` behavior, not
  tested against Cutie's actual `PageStack`.

## Not yet implemented

- Search.
- Pin / color / label.
- Multi-select delete.
- Any masonry-style variable-height layout (you asked for the simplest
  fixed grid instead).

## Troubleshooting
None
