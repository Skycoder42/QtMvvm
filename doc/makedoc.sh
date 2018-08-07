#!/bin/bash
# $1: $$SRCDIR
# $2: $$VERSION
# $3: $$[QT_INSTALL_BINS]
# $4: $$[QT_INSTALL_HEADERS]
# $5: $$[QT_INSTALL_DOCS]
# $pwd: dest dir
set -e

scriptDir=$(dirname "$0")
destDir="$(pwd)"
srcDir=$1
version=$2
verTag=$(echo "$version" | sed -e 's/\.//g')
qtBins=$3
qtHeaders=$4
qtDocs=$5
doxyTemplate="$srcDir/Doxyfile"
doxyRes=Doxyfile.generated
readme="$destDir/README.md"
doxme="$scriptDir/doxme.py"

python3 "$doxme" "$srcDir/../README.md"

cat "$doxyTemplate" > $doxyRes
echo "PROJECT_NUMBER = \"$version\"" >> $doxyRes
echo "INPUT += \"$readme\"" >> $doxyRes
echo "USE_MDFILE_AS_MAINPAGE = \"$readme\"" >> $doxyRes
echo "OUTPUT_DIRECTORY = \"$destDir\"" >> $doxyRes
echo "QHP_NAMESPACE = \"de.skycoder42.qtmvvm.$verTag\"" >> $doxyRes
echo "QHP_CUST_FILTER_NAME = \"Mvvm $version\"" >> $doxyRes
echo "QHP_CUST_FILTER_ATTRS = \"qtmvvm $version\"" >> $doxyRes
echo "QHG_LOCATION = \"$qtBins/qhelpgenerator\"" >> $doxyRes
echo "INCLUDE_PATH += \"$qtHeaders\"" >> $doxyRes
echo "GENERATE_TAGFILE = \"$destDir/qtmvvm/qtmvvm.tags\"" >> $doxyRes
if [ "$DOXY_STYLE" ]; then
	echo "HTML_STYLESHEET = \"$DOXY_STYLE\"" >> $doxyRes
fi
if [ "$DOXY_STYLE_EXTRA" ]; then
	echo "HTML_EXTRA_STYLESHEET = \"$DOXY_STYLE_EXTRA\"" >> $doxyRes
fi

for tagFile in $(find "$qtDocs" -name *.tags); do
	case $(basename "$tagFile") in
		qtjsonserializer.tags)
			echo "TAGFILES += \"$tagFile=https://skycoder42.github.io/QtJsonSerializer\"" >> $doxyRes
			;;
		qtdatasync.tags)
			echo "TAGFILES += \"$tagFile=https://skycoder42.github.io/QtDataSync\"" >> $doxyRes
			;;
		qtservice.tags)
			echo "TAGFILES += \"$tagFile=https://skycoder42.github.io/QtService\"" >> $doxyRes
			;;
		qtmvvm.tags|qtquickcontrols.tags)
			# skipped
			;;
		*)
			echo "TAGFILES += \"$tagFile=https://doc.qt.io/qt-5\"" >> $doxyRes
			;;
	esac
done

cd "$srcDir"
doxygen "$destDir/$doxyRes"
