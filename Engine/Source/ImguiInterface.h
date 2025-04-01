#pragma once

enum class eTheme
{
	pink = 0,
	warmGreen,
	cyan,
	ochre
};

class GraphicsEngine;
class ImGuiInterface
{
public:
	ImGuiInterface(const ImGuiInterface&) = delete;
	ImGuiInterface& operator=(const ImGuiInterface&) = delete;
	static ImGuiInterface& GetInstance();

	~ImGuiInterface();
	bool Init(GraphicsEngine* aEngine, HWND& aWindowHandle);
	void PreFrame();
	void EndFrame();

	static void Style(eTheme aTheme);

private:
	static void ApplyColorTransform(ImVec4* colors, float hueShift, float saturationScale);
	ImGuiInterface() = default;

};
