import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

MsgBoxBase {
	id: _inputDialog

	readonly property url inputUrl: QuickPresenter.inputViewFactory.getInputUrl(msgConfig.subType, msgConfig.viewProperties)

	ColumnLayout {
		anchors.fill: parent

		Label {
			id: _contentLabel
			text: msgConfig.text.replace(/<\/p>/g, "</p><br/>")  //needed because qml does not put space between paragraphs...
			visible: text != ""
			Layout.fillWidth: true
			wrapMode: Text.Wrap
			onLinkActivated: Qt.openUrlExternally(link)
		}

		ProgressBar {
			id: _loadProgress
			visible: _inputViewLoad.status == Loader.Loading
			Layout.fillWidth: true
			value: _inputViewLoad.progress
		}

		Label {
			id: _errorLabel
			visible: _inputViewLoad.status == Loader.Error || inputUrl == ""
			Layout.fillWidth: true
			text: qsTr("<i>Failed to load input view</i>")
			color: "#800000"
			wrapMode: Text.Wrap
		}

		Loader {
			id: _inputViewLoad
			asynchronous: false
			clip: true
			visible: _inputViewLoad.item

			Layout.preferredWidth: _inputViewLoad.item ? _inputViewLoad.item.implicitWidth : 0
			Layout.fillHeight: true
			Layout.fillWidth: true

			onLoaded: {
				msgResult.result = Qt.binding(function() {
					return item.inputValue;
				})
				item.forceActiveFocus();
			}
		}
	}

	Component.onCompleted: {
		var props = msgConfig.viewProperties;
		props["inputValue"] = msgConfig.defaultValue;
		_inputViewLoad.setSource(inputUrl, props);
	}
}
