import QtQuick 2.10
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1

FileChooser {
	id: _fileChooser

	property var msgConfig
	property MessageResult msgResult

	signal closed()

	title: msgConfig.title
	folderUrl: msgConfig.defaultValue
	type: {
		if(msgConfig.subType == "open")
			return FileChooser.OpenDocument;
		else if(msgConfig.subType == "files")
			return FileChooser.OpenMultipleDocuments;
		else if(msgConfig.subType == "save")
			return FileChooser.CreateDocument;
		else if(msgConfig.subType == "get") //special value for android only
			return FileChooser.GetContent;
		else {
			return FileChooser.OpenDocument;
		}
	}

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
