#include "Tools.h"

#if CORE_BUILD_TYPE(TOOLS)

#include <iostream>
#include <string>
#include <map>

#include <Windows.h>

#include "Core/File/File.h"
#include "Core/Util/String.h"
#include "Engine/Engine.h"
#include "Engine/Assets/MeshAsset.h"

static int CreateMeshX();

typedef int(*ToolFunc)();

struct ToolCommand {
	ToolCommand() {}
	ToolCommand(ToolFunc func, std::string desc) :
		mFunc(func),
		mDesc(desc)
	{}

	ToolFunc mFunc = nullptr;
	std::string mDesc = "";
};

static std::map<std::string, ToolCommand> ToolCommands {
	{ "meshx", ToolCommand(CreateMeshX, "Exports a mesh file as a .meshx file.") }
};

int ToolsMain()
{
	// Initialize
	if (playground::InitializeEngine() == false) {
		return EXIT_FAILURE;
	}

	std::cout << "Available Commands:" << std::endl;
	for (auto &it : ToolCommands) {
		std::cout << "\t" << it.first << ": " << it.second.mDesc << std::endl;
	}

	std::cout << std::endl;

EnterCommand:
	std::cout << "Enter Command: ";

	std::string command;
	std::cin >> command;

	int ret = 1;
	if (ToolCommands.find(command) != ToolCommands.end()) {
		ret = ToolCommands[command].mFunc();
	} else {
		std::cout << "Failed to find '" << command << "' command" << std::endl;
		goto EnterCommand;
	}

	// Shutdown
	playground::ShutdownEngine();

	system("pause");

	return ret;
}


static int CreateMeshX()
{
	std::string inFile = playground::OpenFileDialog("X:\\projects\\NAEngine\\NAGame\\Game\\data\\meshes");

	std::string calculateTangent;
	std::cout << "Calculate tangent vector (Y/N)?: ";
	std::cin >> calculateTangent;
	
	const bool success = playground::ConvertMeshObjToMeshx(inFile, calculateTangent == "y");

	return success ? 0 : 1;
}

#endif