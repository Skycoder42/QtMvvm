import QtQuick 2.10
import QtQuick.Controls 2.3
import Qt.labs.platform 1.0 as Labs
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0

Labs.FolderDialog {
	id: _folderDialog

	property var msgConfig
	property MessageResult msgResult

	signal closed()

	title: msgConfig.title
	modality: Qt.WindowModal
	folder: msgConfig.defaultValue

	Component.onCompleted: {
		if(msgResult)
			msgResult.setCloseTarget(_folderDialog, "reject()");
	}

	onAccepted: {
		if(msgResult) {
			msgResult.complete(MessageConfig.Ok, _folderDialog.currentFolder);
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
