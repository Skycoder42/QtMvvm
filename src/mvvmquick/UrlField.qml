import QtQuick 2.10
import QtQuick.Controls 2.3
import de.skycoder42.QtMvvm.Quick.Private 1.0

TextField {
	id: _edit
	property alias inputValue: _edit.text
	property alias allowedSchemes: _validator.allowedSchemes

	selectByMouse: true
	validator: UrlValidator {
		id: _validator
	}
}
