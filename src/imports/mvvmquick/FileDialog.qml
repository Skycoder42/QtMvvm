import QtQuick 2.10
import QtQuick.Controls 2.3
import Qt.labs.platform 1.0 as Labs
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1

/*! @brief A file dialog implementation based on the labs file dialog
 *
 * @extends Qt.labs.platform.FileDialog
 * @extends FileChooser
 *
 * @details It is used internally by the DialogPresenter to create a file dialog for the file
 * dialog message type
 *
 * @note On Android, the dialog extends the FileChooser. For all other platforms, it extends
 * the @ref Qt.labs.platform.FileDialog "Qt labs FileDialog" - an experimental module, and
 * thus might not always function perfectly or break after Qt updates
 */
Labs.FileDialog {
	id: _fileDialog

	/*! @brief The QtMvvm::MessageConfig to configure the dialog with
	 *
	 * @default{`undefined`}
	 *
	 * Various properties are bound to the config to create the dialogs appearance from it.
	 *
	 * @accessors{
	 *	@memberAc{msgConfig}
	 *  @notifyAc{msgConfigChanged()}
	 * }
	 *
	 * @sa QtMvvm::MessageConfig
	 */
	property var msgConfig
	/*! @brief The QtMvvm::MessageResult to report the result to
	 *
	 * @default{`undefined`}
	 *
	 * The dialogs results are reported back to the core app via this result
	 *
	 * @accessors{
	 *	@memberAc{msgResult}
	 *  @notifyAc{msgResultChanged()}
	 * }
	 *
	 * @sa QtMvvm::MessageResult
	 */
	property MessageResult msgResult
	/*! @brief A QStringList of mime type names to use as filter
	 *
	 * @default{<i>Empty</i>}
	 *
	 * Can be set as view property in the message config to limit the selectable files to the
	 * ones that match the given list of mimetypes. Unlike the android variant, normal file
	 * dialogs do not accept wildcard mime types.
	 *
	 * @accessors{
	 *	@memberAc{mimeTypes}
	 *  @notifyAc{mimeTypesChanged()}
	 * }
	 */
	property var mimeTypes

	//! Is emitted when the dialog has been closed for any reason
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
