import QtQuick 2.10
import QtTest 1.1
import de.skycoder42.QtMvvm.Core.test 4.2

Item {
	id: root

	Component {
		id: listElement
		TestSettings_ListElement_8 {}
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
			compare(TestSettings.listEntryGroup.dummyChild, false);
		}

		function test_2_listEntry() {
			// test reading
			compare(TestSettings.listEntry.length, 3);
			verify(TestSettings.listEntry[0]);
			compare(TestSettings.listEntry[0].value, "test1");
			verify(TestSettings.listEntry[1]);
			compare(TestSettings.listEntry[1].value, "test2");
			verify(TestSettings.listEntry[2]);
			compare(TestSettings.listEntry[2].value, "test3");
			verify(!TestSettings.listEntry[3]);

			// test appending
			var elem = listElement.createObject(root, {value: "baum42"});
			verify(elem);
			compare(elem.value, "baum42");
			TestSettings.listEntry.push(elem);
			compare(TestSettings.listEntry.length, 4);
			verify(TestSettings.listEntry[3]);
			compare(TestSettings.listEntry[3].value, "baum42");
			verify(!TestSettings.listEntry[4]);

			// test resetting
			var elem2 = TestSettings.create_listEntry_element("newElem");
			verify(elem2);
			compare(elem2.value, "newElem");
			TestSettings.listEntry = [elem2]
			compare(TestSettings.listEntry.length, 1);
			verify(TestSettings.listEntry[0]);
			compare(TestSettings.listEntry[0].value, "newElem");
			verify(!TestSettings.listEntry[1]);
		}
	}

}
