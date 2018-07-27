import QtQuick 2.10
import QtTest 1.1
import de.skycoder42.QtMvvm.Core.test 4.2

Item {
	id: root

	property string changeData: ""
	Connections {
		target: TestSettings.parentNode.parentEntryGroup
		onLeafEntryChanged: {
			changeData = value;
		}
	}

	property bool ready: false
	property int deepData: 0
	Connections {
		target: ready ? TestSettings.listNode[0].someNode : null
		onDeepChildChanged: {
			deepData = value;
		}
	}

	TestCase {
		name: "QmlSettings"

		function test_0_instance() {
			verify(TestSettings);
			verify(TestSettings.accessor);
		}

		function test_1_defaults() {
			compare(TestSettings.emptyEntry, false);
			compare(TestSettings.advancedEntry, "Hello World");
			compare(TestSettings.codeEntry, "file:///path/to/something");
			compare(TestSettings.parentNode.fullChildNode.replaceEntry, 42);
			compare(TestSettings.parentNode.parentEntry, true);
			compare(TestSettings.parentNode.parentEntryGroup.nodeWithCodeEntry, 43);
			compare(TestSettings.parentNode.parentEntryGroup.leafEntry, qsTr("translate me"));
			compare(TestSettings.variantEntry, undefined);
			compare(TestSettings.simpleListEntry, [42]);
		}

		function test_2_changeSignals() {
			changeData = "";
			TestSettings.parentNode.parentEntryGroup.leafEntry = "test44";
			compare(TestSettings.parentNode.parentEntryGroup.leafEntry, "test44");
			compare(changeData, "test44");
		}

		function test_3_listEntry() {
			// test reading
			compare(TestSettings.listNode.length, 0);
			verify(TestSettings.listNode_push());
			compare(TestSettings.listNode.length, 1);

			compare(TestSettings.listNode[0].simpleChild, false);
			verify(TestSettings.listNode[0].someNode)
			compare(TestSettings.listNode[0].someNode.deepChild, 22);
			compare(TestSettings.listNode[0].someNode.deepParent, "___");
			verify(TestSettings.listNode[0].someNode.deepParentGroup)
			compare(TestSettings.listNode[0].someNode.deepParentGroup.simpleChild, true);
			compare(TestSettings.listNode[0].childList.length, 0);

			ready = true;
			deepData = 0;
			TestSettings.listNode[0].someNode.deepChild = 99;
			compare(TestSettings.listNode[0].someNode.deepChild, 99);
			compare(deepData, 99);
			deepData = 0;

			compare(TestSettings.listNode.length, 1);
			var newEntry = TestSettings.listNode_push();
			verify(newEntry);
			compare(TestSettings.listNode.length, 2);
			compare(TestSettings.listNode[1], newEntry);
			compare(newEntry.simpleChild, false);
			compare(TestSettings.listNode[1].simpleChild, false);
			newEntry.simpleChild = true;
			compare(newEntry.simpleChild, true);
			compare(TestSettings.listNode[1].simpleChild, true);
			compare(TestSettings.listNode[0].simpleChild, false);

			var newEntry2 = TestSettings.listNode_push_deferred();
			verify(newEntry2);
			compare(TestSettings.listNode.length, 2);
			verify(!TestSettings.listNode[2]);
			compare(newEntry2.simpleChild, false);
			newEntry2.simpleChild = true;
			compare(newEntry2.simpleChild, true);
			compare(TestSettings.listNode[0].simpleChild, false);
			compare(TestSettings.listNode.length, 2);
			TestSettings.listNode.push(newEntry2);
			compare(TestSettings.listNode.length, 3);
			compare(TestSettings.listNode[2], newEntry2);
			compare(deepData, 0);

			TestSettings.listNode = [];
			compare(TestSettings.listNode.length, 0);
			verify(!TestSettings.listNode[0]);
		}
	}

}
