#!/usr/bin/env python3
# Usage: qsettingstranslator.py <in> <out>

import sys
try:
	from defusedxml.ElementTree import parse
except ImportError:
	from xml.etree.ElementTree import parse


def create_settings_ts(infile, outfile):
	strings = []
	tree = parse(infile)
	for elem in tree.iter():
		if elem.tag == "SearchKey":
			strings.append(elem.text)
		else:
			if "title" in elem.attrib:
				strings.append(elem.attrib["title"])
			if "tooltip" in elem.attrib:
				strings.append(elem.attrib["tooltip"])
			if "tr" in elem.attrib and elem.attrib["tr"].lower() == "true":
				strings.append(elem.text)
			if "ztr" in elem.attrib and elem.attrib["ztr"].lower() == "true": # lazy translations
				strings.append(elem.text)
			if "trdefault" in elem.attrib and elem.attrib["trdefault"].lower() == "true":
				strings.append(elem.attrib["default"])
			# legaxy keys, because of a typo...
			if "ts" in elem.attrib and elem.attrib["ts"].lower() == "true":
				strings.append(elem.text)
			if "tsdefault" in elem.attrib and elem.attrib["tsdefault"].lower() == "true":
				strings.append(elem.attrib["default"])

	with open(outfile, "w") as file:
		file.write("#include <QCoreApplication>\n\n")
		file.write("void dummyfn() {\n")
		for string in strings:
			string = string.replace("\\", "\\\\")
			string = string.replace("\"", "\\\"")
			file.write("\tQCoreApplication::translate(\"qtmvvm_settings_xml\", \"{}\");\n".format(string))
		file.write("}\n")


if __name__ == '__main__':
	if len(sys.argv) != 3:
		print("Usage: qsettingstranslator.py <in> <out>", file=sys.stderr)
		exit(1)
	else:
		create_settings_ts(*sys.argv[1:3])
