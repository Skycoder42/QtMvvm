import QtQuick 2.10
import de.skycoder42.QtMvvm.Core 1.1
import QtTest 1.1

Item {
	id: root

	TestCase {
		name: "Binding"

		QtObject {
			id: obj

			property int prop1: 42
			property int prop2: 0
		}

		MvvmBinding {
			id: bind
			viewModel: obj
			view: obj
			viewModelProperty: "prop1"
			viewProperty: "prop2"
			type: MvvmBinding.SingleInit
		}

		function test_1_SingleInit() {
			verify(bind.isValid());
			compare(obj.prop1, 42);
			compare(obj.prop2, 42);
			obj.prop1 = 24;
			compare(obj.prop2, 42);
			obj.prop2 = 44;
			compare(obj.prop1, 24);
		}

		function test_2_OneWayToView() {
			bind.type = MvvmBinding.OneWayToView
			obj.prop1 = 33;
			compare(obj.prop2, 33);
			obj.prop2 = 44;
			compare(obj.prop1, 33);
		}

		function test_3_OneWayToViewModel() {
			bind.type = MvvmBinding.OneWayToViewModel
			obj.prop2 = 45;
			compare(obj.prop1, 45);
			obj.prop1 = 22;
			compare(obj.prop2, 45);
		}

		function test_4_TwoWay() {
			bind.type = MvvmBinding.TwoWay
			obj.prop1 = 78;
			compare(obj.prop2, 78);
			obj.prop2 = 87;
			compare(obj.prop1, 87);
		}
	}
}
