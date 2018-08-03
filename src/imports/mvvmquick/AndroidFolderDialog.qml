import QtQuick 2.10
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1

FileChooser {
	id: _folderChooser

	property var msgConfig
	property MessageResult msgResult

	signal closed()

	title: msgConfig.title
	folderUrl: msgConfig.defaultValue
	type: FileChooser.OpenDocumentTree

	onAccepted: {
		if(msgResult) {
			msgResult.complete(MessageConfig.Ok, result);
			msgResult = null;
		}
		closed();
	}

	onRejected: {
		if(msgResult) {
			msgResult.complete(MessageConfig.Cancel);
			msgResult = null;
		}
		closed();
	}
}
