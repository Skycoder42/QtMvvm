import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Core 1.1
import de.skycoder42.QtMvvm.Quick 1.1

MsgBoxBase {
	id: _progressDialog

	closePolicy: Popup.NoAutoClose
	autoHandleBtns: false

	readonly property ProgressControl progressControl: msgConfig.defaultValue
	property string format: "%p%"

	property int _cancelAction: MessageConfig.NoButton
	readonly property bool _allowClose: !progressControl || !msgResult

	onButtonClicked: tryCancel(button)

	Component.onCompleted: {
		if(msgResult)
			msgResult.setCloseTarget(_progressDialog, "finish()");
	}

	onClosed: {
		if(progressControl)
			Qt.callLater(progressControl.notifyClosed);
	}

	function finish() {
		if(msgResult) {
			msgResult.complete(_cancelAction);
			msgResult = null;
		}
		close();
	}

	function tryCancel(button) {
		if(_cancelAction == MessageConfig.NoButton) {
			_cancelAction = button;
			footer.enabled = false;
			if(progressControl)
				progressControl.requestCancel(button);
			else
				finish();
		}
	}

	function closeAction() {
		tryCancel(MessageConfig.Cancel);
		return true;
	}

	Connections {
		target: progressControl
		onChangeLabel: _contentLabel.text = text;
		onCloseRequested: finish()
	}

	GridLayout {
		columns: 2
		anchors.fill: parent

		Label {
			id: _contentLabel
			text: msgConfig.text.replace(/<\/p>/g, "</p><br/>")  //needed because qml does not put space between paragraphs...
			visible: text != ""
			Layout.fillWidth: true
			Layout.columnSpan: 2
			wrapMode: Text.Wrap
			onLinkActivated: Qt.openUrlExternally(link)
		}

		ProgressBar {
			id: _loadProgress
			Layout.fillWidth: true
			Layout.columnSpan: progressControl.indeterminate ? 2 : 1
			indeterminate: progressControl.indeterminate
			from: progressControl.minimum
			to: progressControl.maximum
			value: progressControl.progress
		}

		Label {
			id: _progressLabel
			visible: !progressControl.indeterminate
			text: format
				.replace("%p", Math.round(((progressControl.progress - progressControl.minimum) / (progressControl.maximum - progressControl.minimum)) * 100.0))
				.replace("%v", progressControl.progress)
				.replace("%m", progressControl.maximum - progressControl.minimum)
		}
	}
}
