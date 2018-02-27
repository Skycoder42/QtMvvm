#!/usr/bin/env python3
# Usage: qsettingstranslator.py <in> <out>

import sys
import os
from xml.etree.ElementTree import Element, parse

if len(sys.argv) != 3:
	print("Usage: qsettingstranslator.py <in> <out>", file=sys.stderr)
	exit(1)

infile = sys.argv[1]
outfile = sys.argv[2]

strings = set()
tree = parse(infile)
root = Element("TS")
for elem in tree.iter():
	if elem.tag == "SearchKey":
		strings.add(elem.text)
	else:
		if "title" in elem.attrib:
			strings.add(elem.attrib["title"])
		if "tooltip" in elem.attrib:
			strings.add(elem.attrib["tooltip"])
		if "ts" in elem.attrib and elem.attrib["ts"].lower() == "true":
			strings.add(elem.text)
		if "tsdefault" in elem.attrib and elem.attrib["tsdefault"].lower() == "true":
			strings.add(elem.attrib["default"])

with open(outfile, "w") as file:
	file.write("#include <QCoreApplication>\n\n")
	file.write("void dummyfn() {\n")
	for string in strings:
		string = string.replace("\\", "\\\\")
		string = string.replace("\"", "\\\"")
		file.write("\tQCoreApplication::translate(\"{}\", \"{}\");\n".format(os.path.basename(infile), string))
	file.write("}\n")
