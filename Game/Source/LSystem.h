#pragma once
#include <string>
#include <array>

enum class eRules
{
	F = 0,
	X,
	Y,
	Z,
	W,
	count
};

class LSystem
{
public:
	LSystem();
	LSystem(const std::string& aFRule, const std::string& aXRule, const std::string& aYRule,
		const std::string& aZRule, const std::string& aWRule);
	~LSystem() = default;

	void LSystemGenerate();
	bool AnalyseLsystemPerformance();

	inline void SetRule(const eRules& aRule, const std::string& aReplacement) 
						{ myRules[static_cast<int>(aRule)] = aReplacement; };

	inline void SetDepth(const unsigned int& aDepth) { myDepth = aDepth; };
	inline void SetAxiom(const std::string& aAxiom) { myAxiom = aAxiom; };

	inline const std::string& GetResult() { return myResult; };
	inline const std::string& GetAxiom() { return myAxiom; };
	inline std::string& AccessAxiom() { return myAxiom; };
	inline std::string* AccessRules() { return myRules; }
	inline const int GetDepth() { return myDepth; };
	inline int& AccessDepth() { return myDepth; };

private:
	std::string myAxiom = "FX";
	std::string myResult = "";
	std::string myRules[static_cast<int>(eRules::count)];
	int myDepth = 3;
};