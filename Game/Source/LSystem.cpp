#include "LSystem.h"
#include "pch.h"

LSystem::LSystem()
{
	myRules[static_cast<int>(eRules::F)] = "F";
	myRules[static_cast<int>(eRules::X)] = "[-FX]+FX";
	myRules[static_cast<int>(eRules::Y)] = "";
	myRules[static_cast<int>(eRules::Z)] = "";
	myRules[static_cast<int>(eRules::W)] = "";
	LSystemGenerate();
}

LSystem::LSystem(const std::string& aFRule, const std::string& aXRule, const std::string& aYRule,
	const std::string& aZRule, const std::string& aWRule)
{
	myRules[static_cast<int>(eRules::F)] = aFRule;
	myRules[static_cast<int>(eRules::X)] = aXRule;
	myRules[static_cast<int>(eRules::Y)] = aYRule;
	myRules[static_cast<int>(eRules::Z)] = aZRule;
	myRules[static_cast<int>(eRules::W)] = aWRule;
	LSystemGenerate();
}

void LSystem::LSystemGenerate()
{
	myResult = "";

	std::string result = myAxiom;
	for (int i = 0; i < myDepth; ++i)
	{
		std::string newResult = "";
		for (char c : result)
		{
			bool ruleApplied = false;

			// Check and apply rule F if not empty
			if (c == 'F' && !myRules[static_cast<int>(eRules::F)].empty())
			{
				newResult += myRules[static_cast<int>(eRules::F)];
				ruleApplied = true;
			}

			// Check and apply rule X if not empty
			else if (c == 'X' && !myRules[static_cast<int>(eRules::X)].empty())
			{
				newResult += myRules[static_cast<int>(eRules::X)];
				ruleApplied = true;
			}

			// Check and apply rule Y if not empty
			else if (c == 'Y' && !myRules[static_cast<int>(eRules::Y)].empty())
			{
				newResult += myRules[static_cast<int>(eRules::Y)];
				ruleApplied = true;
			}

			// Check and apply rule Z if not empty
			else if (c == 'Z' && !myRules[static_cast<int>(eRules::Z)].empty())
			{
				newResult += myRules[static_cast<int>(eRules::Z)];
				ruleApplied = true;
			}

			// Check and apply rule W if not empty
			else if (c == 'W' && !myRules[static_cast<int>(eRules::W)].empty())
			{
				newResult += myRules[static_cast<int>(eRules::W)];
				ruleApplied = true;
			}

			// If no rule was applied, keep the original character
			if (!ruleApplied)
			{
				newResult += c;
			}
		}
		result = newResult;
	}
	myResult = result;
}

bool LSystem::AnalyseLsystemPerformance()
{
	if (myResult.size() < 10000)
	{
		return true;
	}
	return false;
}


