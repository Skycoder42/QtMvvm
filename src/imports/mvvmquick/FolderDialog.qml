import QtQuick 2.10
import QtQuick.Controls 2.3
import Qt.labs.platform 1.0 as Labs
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief A folder dialog implementation based on the labs folder dialog
 *
 * @extends Qt.labs.platform.FolderDialog
 * @extends FileChooser
 *
 * @details It is used internally by the DialogPresenter to create a folder dialog for the file
 * dialog message type
 *
 * @note On Android, the dialog extends the FileChooser. For all other platforms, it extends
 * the @ref Qt.labs.platform.FolderDialog "Qt labs FolderDialog" - an experimental module, and
 * thus might not always function perfectly or break after Qt updates
 */
Labs.FolderDialog {
	id: _folderDialog

	//! @copydoc FileDialog::msgConfig
	property var msgConfig
	//! @copydoc FileDialog::msgResult
	property MessageResult msgResult

	//! @copydoc FileDialog::closed
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
