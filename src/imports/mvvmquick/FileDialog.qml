import QtQuick 2.10
import QtQuick.Controls 2.3
import Qt.labs.platform 1.0 as Labs
import de.skycoder42.QtMvvm.Core 1.0
import de.skycoder42.QtMvvm.Quick 1.0

Labs.FileDialog {
	id: _fileDialog

	property var msgConfig
	property MessageResult msgResult
	property var mimeTypes

	signal closed()

	title: msgConfig.title
	modality: Qt.WindowModal
	folder: msgConfig.defaultValue
	fileMode: {
		if(msgConfig.subType == "open")
			return Labs.FileDialog.OpenFile;
		else if(msgConfig.subType == "files")
			return Labs.FileDialog.OpenFiles;
		else if(msgConfig.subType == "save")
			return Labs.FileDialog.SaveFile;
		else {
			return Labs.FileDialog.OpenFile; //fallback
		}
	}
	nameFilters: QuickPresenter.mimeTypeFilters(mimeTypes ? mimeTypes : msgConfig.viewProperties["mimeTypes"])

	Component.onCompleted: {
		if(msgResult)
			msgResult.setCloseTarget(_fileDialog, "reject()");
	}

	onAccepted: {
		if(msgResult) {
			if(msgConfig.subType == "files")
				msgResult.result = _fileDialog.files;
			else
				msgResult.result = _fileDialog.file;
			msgResult.complete(MessageConfig.Ok);
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
