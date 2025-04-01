#include "pch.h"
#include "SerialisePlant.h"

void PlantSerializer::LoadPresets(const std::string aFilePath, std::unordered_map<std::string, sPlantParams>& aPresets)
{
	std::ifstream file(aFilePath);
#ifdef _DEBUG
	if (!file)
	{
		std::cout << "Plant::LoadPreset failed to open file located at: " << aFilePath << std::endl;
		return; // Add this to avoid processing a non-existent file
	}
#endif

	std::string string;
	std::string currentPreset;
	sPlantParams currentParams;
	bool hasCurrentPreset = false;

	while (std::getline(file, string))
	{
		// Remove leading whitespace
		size_t startPos = string.find_first_not_of(" \t\r\n");
		if (startPos != std::string::npos)
		{
			string = string.substr(startPos);
		}
		else
		{
			continue;
		}

		if (string.empty()) continue;

		//check for preset header
		if (string.front() == '[' && string.back() == ']')
		{
			// Save the previous preset if it exists
			if (hasCurrentPreset && !currentPreset.empty())
			{
				aPresets[currentPreset] = currentParams;
			}

			currentPreset = string.substr(1, string.size() - 2);
			currentParams = sPlantParams();
			hasCurrentPreset = true;
		}
		//parse preset values
		else if (hasCurrentPreset)
		{
			std::istringstream iss(string);
			std::string key, value;
			if (std::getline(iss, key, '=') && std::getline(iss, value))
			{
				size_t keyStartPos = key.find_first_not_of(" \t\r\n");
				if (keyStartPos != std::string::npos)
				{
					key = key.substr(keyStartPos);
				}
				else
				{
					key.clear();
				}

				size_t valueStartPos = value.find_first_not_of(" \t\r\n");
				if (valueStartPos != std::string::npos)
				{
					value = value.substr(valueStartPos);
				}
				else
				{
					value.clear();
				}

				// Rest of your parsing code
				if (key == "initialWidth")
				{
					currentParams.initialWidth = std::stof(value);
				}
				else if (key == "initialLength")
				{
					currentParams.initialLength = std::stof(value);
				}
				else if (key == "scaleFactorWidth")
				{
					currentParams.generalScaleFactorWidth = std::stof(value);
				}
				else if (key == "scaleFactorHeight")
				{
					currentParams.generalScaleFactorHeight = std::stof(value);
				}
				else if (key == "rotationA")
				{
					std::istringstream rotationStream(value);
					char comma;
					rotationStream >>
						currentParams.rotationA.x >> comma >>
						currentParams.rotationA.y >> comma >>
						currentParams.rotationA.z;
				}
				else if (key == "rotationB")
				{
					std::istringstream rotationStream(value);
					char comma;
					rotationStream >>
						currentParams.rotationB.x >> comma >>
						currentParams.rotationB.y >> comma >>
						currentParams.rotationB.z;
				}
				else if (key == "maxDepth")
				{
					currentParams.lSystem.SetDepth(std::stoi(value));
				}
				else if (key == "additionalWidthModifier")
				{
					currentParams.additionalWidthModifier = std::stof(value);
				}
				else if (key == "additionalHeightModifier")
				{
					currentParams.additionalHeightModifier = std::stof(value);
				}
				else if (key == "additionalTurningAngleIncrement")
				{
					currentParams.additionalTurningAngleIncrement = std::stof(value);
				}
				else if (key == "axiom")
				{
					currentParams.lSystem.SetAxiom(value);
				}
				else if (key == "ruleF")
				{
					currentParams.lSystem.SetRule(eRules::F, value);
				}
				else if (key == "ruleX")
				{
					currentParams.lSystem.SetRule(eRules::X, value);
				}
				else if (key == "ruleY")
				{
					currentParams.lSystem.SetRule(eRules::Y, value);
				}
				else if (key == "ruleZ")
				{
					currentParams.lSystem.SetRule(eRules::Z, value);
				}
				else if (key == "ruleW")
				{
					currentParams.lSystem.SetRule(eRules::W, value);
				}
				else if (key == "useLeaves")
				{
					currentParams.useLeaves = std::stoi(value);
				}
				else if (key == "leafRotation")
				{
					std::istringstream rotationStream(value);
					char comma;
					rotationStream >>
						currentParams.leafRotation.x >> comma >>
						currentParams.leafRotation.y >> comma >>
						currentParams.leafRotation.z;
				}
				else if (key == "leafSize")
				{
					currentParams.leafSize = std::stof(value);
				}
				else if (key == "randomLeafRotation")
				{
					currentParams.randomLeafRotation = std::stof(value);
				}
				else if (key == "randomLeafScale")
				{
					currentParams.randomLeafScale = std::stof(value);
				}
				else if (key == "branchSpikyness")
				{
					currentParams.branchSpikyness = std::stof(value);
				}
			}
		}
	}

	if (hasCurrentPreset && !currentPreset.empty())
	{
		aPresets[currentPreset] = currentParams;
	}
}

bool PlantSerializer::TrySavePreset(const std::string aFilePath, const std::string aPresetName)
{
	bool fileExists = false;

	std::ifstream file(aFilePath);
	fileExists = file.is_open();
	if (fileExists)
	{
		std::string presetName;
		while (std::getline(file, presetName))
		{
			size_t startPos = presetName.find_first_not_of(" \t\r\n");
			if (startPos != std::string::npos)
			{
				presetName = presetName.substr(startPos);
			}
			else
			{
				continue;
			}

			if (presetName.empty())
			{
				continue;
			}

			if (presetName.front() == '[' && presetName.back() == ']')
			{
				std::string existingPreset = presetName.substr(1, presetName.size() - 2);
				if (existingPreset == aPresetName)
				{
#ifdef _DEBUG
					std::cout << "name already exists" << std::endl;
#endif
					myOverwrite = false;
					return true;
				}
			}
		}

		return false;
	}
	return true;
}

void PlantSerializer::SavePreset(const std::string aFilePath, const std::string& aPresetName, sPlantParams& aPlantParam)
{
	std::ifstream file(aFilePath);
	if (!file.is_open())
	{
#ifdef _DEBUG
		std::cout << "Failed to open file for reading at: " << aFilePath << std::endl;
#endif
		return;
	}

	std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	// Find the preset name
	size_t presetStartPos = fileContent.find("[" + aPresetName + "]");
	if (presetStartPos == std::string::npos)
	{
		// Preset doesn't exist
		std::ofstream outFile(aFilePath, std::ios::app);
		if (!outFile.is_open())
		{
#ifdef _DEBUG
			std::cout << "Failed to open file for writing at: " << aFilePath << std::endl;
#endif
			return;
		}

		outFile << std::endl;
		outFile << "[" << aPresetName << "]" << std::endl;
		outFile << "initialWidth=" << aPlantParam.initialWidth << std::endl;
		outFile << "initialLength=" << aPlantParam.initialLength << std::endl;
		outFile << "scaleFactorWidth=" << aPlantParam.generalScaleFactorWidth << std::endl;
		outFile << "scaleFactorHeight=" << aPlantParam.generalScaleFactorHeight << std::endl;
		outFile << "rotationA=" << aPlantParam.rotationA.x << ","
			<< aPlantParam.rotationA.y << ","
			<< aPlantParam.rotationA.z << std::endl;
		outFile << "rotationB=" << aPlantParam.rotationB.x << ","
			<< aPlantParam.rotationB.y << ","
			<< aPlantParam.rotationB.z << std::endl;
		outFile << "maxDepth=" << aPlantParam.lSystem.GetDepth() << std::endl;
		//Lsystem symbol modifiers
		outFile << "additionalWidthModifier=" << aPlantParam.additionalWidthModifier << std::endl;
		outFile << "additionalHeightModifier=" << aPlantParam.additionalHeightModifier << std::endl;
		outFile << "additionalTurningAngleIncrement=" << aPlantParam.additionalTurningAngleIncrement << std::endl;
		//Lsystem
		outFile << "axiom=" << aPlantParam.lSystem.GetAxiom() << std::endl;
		outFile << "ruleF=" << aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::F)] << std::endl;
		outFile << "ruleX=" << aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::X)] << std::endl;
		outFile << "ruleY=" << aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::Y)] << std::endl;
		outFile << "ruleZ=" << aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::Z)] << std::endl;
		outFile << "ruleW=" << aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::W)] << std::endl;
		//Lead
		outFile << "useLeaves=" << static_cast<int>(aPlantParam.useLeaves) << std::endl;
		outFile << "leafRotation=" << aPlantParam.leafRotation.x << ","
			<< aPlantParam.leafRotation.y << ","
			<< aPlantParam.leafRotation.z << std::endl;
		outFile << "leafSize=" << aPlantParam.leafSize << std::endl;
		outFile << "randomLeafRotation=" << aPlantParam.randomLeafRotation << std::endl;
		outFile << "randomLeafScale=" << aPlantParam.randomLeafScale << std::endl;
		outFile << "branchSpikyness=" << aPlantParam.branchSpikyness << std::endl;

		outFile.close();
#ifdef _DEBUG
		std::cout << "Successfully appended preset '" << aPresetName << "' to " << aFilePath << std::endl;
#endif
	}
	else
	{
		//replace the content of that section
		size_t presetEndPos = fileContent.find("\n[", presetStartPos + 1); 

		if (presetEndPos == std::string::npos)
		{
			presetEndPos = fileContent.length();//set end position to the end of the file
		}

		// Extract the part before the preset, the preset to be replaced, and the part after it
		std::string beforePreset = fileContent.substr(0, presetStartPos);
		std::string afterPreset = fileContent.substr(presetEndPos);

		std::string newPreset = "[" + aPresetName + "]" + "\n";
		newPreset += "initialWidth=" + std::to_string(aPlantParam.initialWidth) + "\n";
		newPreset += "initialLength=" + std::to_string(aPlantParam.initialLength) + "\n";
		newPreset += "scaleFactorWidth=" + std::to_string(aPlantParam.generalScaleFactorWidth) + "\n";
		newPreset += "scaleFactorHeight=" + std::to_string(aPlantParam.generalScaleFactorHeight) + "\n";
		newPreset += "rotationA=" + std::to_string(aPlantParam.rotationA.x) + "," + std::to_string(aPlantParam.rotationA.y) + "," + std::to_string(aPlantParam.rotationA.z) + "\n";
		newPreset += "rotationB=" + std::to_string(aPlantParam.rotationB.x) + "," + std::to_string(aPlantParam.rotationB.y) + "," + std::to_string(aPlantParam.rotationB.z) + "\n";
		newPreset += "maxDepth=" + std::to_string(aPlantParam.lSystem.GetDepth()) + "\n";
		//Lsystem symbol modifiers
		newPreset += "additionalWidthModifier=" + std::to_string(aPlantParam.additionalWidthModifier) + "\n";
		newPreset += "additionalHeightModifier=" + std::to_string(aPlantParam.additionalHeightModifier) + "\n";
		newPreset += "additionalTurningAngleIncrement=" + std::to_string(aPlantParam.additionalTurningAngleIncrement) + "\n";
		//Lsystem
		newPreset += "axiom=" + aPlantParam.lSystem.GetAxiom() + "\n";
		newPreset += "ruleF=" + aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::F)] + "\n";
		newPreset += "ruleX=" + aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::X)] + "\n";
		newPreset += "ruleY=" + aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::Y)] + "\n";
		newPreset += "ruleZ=" + aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::Z)] + "\n";
		newPreset += "ruleW=" + aPlantParam.lSystem.AccessRules()[static_cast<int>(eRules::W)] + "\n";
		//Lead
		newPreset += "useLeaves=" + std::to_string(static_cast<int>(aPlantParam.useLeaves)) + "\n";
		newPreset += "leafRotation=" + std::to_string(aPlantParam.leafRotation.x) + "," + std::to_string(aPlantParam.leafRotation.y) + "," + std::to_string(aPlantParam.leafRotation.z) + "\n";
		newPreset += "leafSize=" + std::to_string(aPlantParam.leafSize) + "\n";
		newPreset += "randomLeafRotation=" + std::to_string(aPlantParam.randomLeafRotation) + "\n";
		newPreset += "randomLeafScale=" + std::to_string(aPlantParam.randomLeafScale) + "\n";
		newPreset += "branchSpikyness=" + std::to_string(aPlantParam.branchSpikyness) + "\n";

		std::string newFileContent = beforePreset + newPreset + afterPreset;

		std::ofstream outFile(aFilePath, std::ios::out);
		if (!outFile.is_open())
		{
#ifdef _DEBUG
			std::cout << "Failed to open file for writing at: " << aFilePath << std::endl;
#endif
			return;
		}

		outFile << newFileContent;
		outFile.close();

#ifdef _DEBUG
		std::cout << "Successfully overwritten preset '" << aPresetName << "' in " << aFilePath << std::endl;
#endif
	}
}
