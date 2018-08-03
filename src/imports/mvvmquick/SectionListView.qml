import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import de.skycoder42.QtMvvm.Quick 1.1

ListView {
	id: _listView

	property SettingsUiBuilder builder

	section.property: "group"
	section.labelPositioning: ViewSection.InlineLabels
	section.delegate: ListSection {
		title: section
	}

	delegate: Loader {
		id: loaderDelegate
		width: parent.width
		height: item ? item.implicitHeight : 0

		onLoaded: {
			if(loaderDelegate.item && typeof loaderDelegate.item.showInput !== "undefined") {
				loaderDelegate.item.showInput.connect(function(key, title, type, defaultValue, props){
					builder.showDialog(key, title, type, defaultValue, props);
				});
			}
		}

		Component.onCompleted: loaderDelegate.setSource(delegateUrl, properties);
	}
}
