import qbs
import qbs.FileInfo

Module {
	Depends { name: "Qt.mvvmcore" }

	property string qsettingsgeneratorName: "qsettingsgenerator"

	version: Qt.mvvmcore.version

	FileTagger {
		fileTags: ["settings-xml"]
		patterns: ["*.settings.xml"]
	}

	Rule {
		inputs: ["settings-xml"]

		Artifact {
			filePath: input.baseName + ".h"
			fileTags: ["hpp"]
		}
		Artifact {
			filePath: input.baseName + ".cpp"
			fileTags: ["cpp"]
		}

		prepare: {
			var cmd = new Command();
			cmd.description = "generating settings class" + input.fileName;
			cmd.highlight = "codegen";
			cmd.program = FileInfo.joinPaths(product.moduleProperty("Qt.core", "binPath"),
											 product.moduleProperty("Qt.restbuilder", "qsettingsgeneratorName"));
			cmd.arguments = [
				"--in", input.filePath,
				"--header", outputs["hpp"][0].filePath,
				"--impl", outputs["cpp"][0].filePath
			];
			return cmd;
		}
	}
}
