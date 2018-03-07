import QtQuick 2.10
import QtQuick.Controls 2.3

MsgBoxBase {
	id: _msgBox

	iconVisible: msgConfig.subType != "about"
	iconSource: {
		var base = "qrc:/de/skycoder42/qtmvvm/quick/icons/ic_%1.svg";
		switch(String(msgConfig.subType)) {
		case "information":
			base = base.arg("info");
			break;
		case "question":
			base = base.arg("help");
			break;
		case "warning":
			base = base.arg("warning");
			break;
		case "critical":
			base = base.arg("error");
			break;
		case "about":
		default:
			return "";
		}
		return base;
	}

	Label {
		id: _contentLabel
		text: msgConfig.text.replace(/<\/p>/g, "</p><br/>")  //needed because qml does not put space between paragraphs...
		visible: text != ""
		anchors.fill: parent
		wrapMode: Text.Wrap
		onLinkActivated: Qt.openUrlExternally(link)
	}
}
