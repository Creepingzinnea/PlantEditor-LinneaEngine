#pragma once
#include "PlantParams.h"
#include <sstream>
#include <fstream>
#include <unordered_map>

class PlantSerializer
{
public:
	PlantSerializer() = default;
	~PlantSerializer() = default;

	void LoadPresets(const std::string aFilePath, std::unordered_map<std::string, sPlantParams>& aPresets);
	bool TrySavePreset(const std::string aFilePath, const std::string aPresetName);
	void SavePreset(const std::string aFilePath, const std::string& aPresetName, sPlantParams& aPlantParam);

private:
	bool myOverwrite = false;
};

	