#include "pch.h"
#include "ImguiInterface.h"
#include "GraphicsEngine.h"
#include "FileLogger.h"
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

static ImGuiInterface* myInstance = nullptr;

ImGuiInterface& ImGuiInterface::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new ImGuiInterface();
	}

	return *myInstance;
}

ImGuiInterface::~ImGuiInterface()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (myInstance)
	{
		delete myInstance;
	}
}

static ImFont* ImGui_LoadFont(ImFontAtlas& atlas, const char* name, float size)
{
	char* windir = nullptr;
	if (_dupenv_s(&windir, nullptr, "WINDIR") || windir == nullptr)
		return nullptr;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0104, 0x017C, // Polish characters and more
		0,
	};

	ImFontConfig config;
	config.OversampleH = 4;
	config.OversampleV = 4;
	config.PixelSnapH = false;

	auto path = std::string(windir) + "\\Fonts\\" + name;
	auto font = atlas.AddFontFromFileTTF(path.c_str(), size, &config, ranges);

	free(windir);

	return font;
}

bool ImGuiInterface::Init(GraphicsEngine* aEngine, HWND& aWindowHandle)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.LogFilename = nullptr;

	// Setup ImGui binding
	assert(aWindowHandle != nullptr);
	if (!ImGui_ImplWin32_Init(aWindowHandle))
	{
		FileLogger::Get().LogError("ImGui_ImplWin32_Init() failed");
		FileLogger::Get().Flush();
		return false;
	}

	if (!ImGui_ImplDX11_Init(aEngine->GetDevice().Get(), aEngine->GetDeviceContext().Get()))
	{
		FileLogger::Get().LogError("ImGui_ImplDX11_Init() failed");
		FileLogger::Get().Flush();
		return false;
	}

	ImGui::StyleColorsDark();
	ImGui_ImplDX11_CreateDeviceObjects();

	return true;
}

void ImGuiInterface::PreFrame()
{
	if (WindowHandler::GetInstance().GetWindowResized())
	{
		float width = static_cast<float>(WindowHandler::GetInstance().GetWidth());
		float height = static_cast<float>(WindowHandler::GetInstance().GetHeight());
		ImGui::GetIO().DisplaySize = ImVec2(width, height);
	}

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
}

void ImGuiInterface::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

void ImGuiInterface::ApplyColorTransform(ImVec4* colors, float hueShift, float saturationScale)
{
	// Convert ImGui colors to HSV and transform them
	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
#undef max
#undef min

		ImVec4& color = colors[i];
		if (color.w <= 0.0f)
			continue;

		// Extract RGB components 
		float r = std::max(0.0f, std::min(1.0f, color.x));
		float g = std::max(0.0f, std::min(1.0f, color.y));
		float b = std::max(0.0f, std::min(1.0f, color.z));

		// Calculate min/max/delta values
		float max_val = std::max(r, std::max(g, b));
		float min_val = std::min(r, std::min(g, b));
		float delta = max_val - min_val;

		// Calculate HSV values
		float h = 0.0f, s = 0.0f, v = max_val;

		// Calculate saturation (avoid division by zero)
		if (max_val > 0.0f && delta > 0.0f)
		{
			s = delta / max_val;

			// Calculate hue
			float hue_factor;
			if (r >= max_val)
				hue_factor = (g - b) / delta + (g < b ? 6.0f : 0.0f);
			else if (g >= max_val)
				hue_factor = (b - r) / delta + 2.0f;
			else
				hue_factor = (r - g) / delta + 4.0f;

			h = hue_factor * 60.0f;
		}

		// Apply transformation with proper wrapping for hue
		h = std::fmod(h + hueShift + 360.0f, 360.0f);

		float targetSaturation = s * saturationScale;
		if (saturationScale > 1.0f)
			s = s + (targetSaturation - s) * 0.8f;
		else
			s = targetSaturation;

		s = std::max(0.0f, std::min(1.0f, s));

		// Convert back to RGB 
		float c = v * s;
		float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
		float m = v - c;

		float r1, g1, b1;
		if (h < 60.0f)
		{
			r1 = c; g1 = x; b1 = 0;
		}
		else if (h < 120.0f)
		{
			r1 = x; g1 = c; b1 = 0;
		}
		else if (h < 180.0f)
		{
			r1 = 0; g1 = c; b1 = x;
		}
		else if (h < 240.0f)
		{
			r1 = 0; g1 = x; b1 = c;
		}
		else if (h < 300.0f)
		{
			r1 = x; g1 = 0; b1 = c;
		}
		else
		{
			r1 = c; g1 = 0; b1 = x;
		}

		// Final RGB values
		color.x = r1 + m;
		color.y = g1 + m;
		color.z = b1 + m;
	}
}

void ImGuiInterface::Style(eTheme aTheme)
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	const ImVec4 TextColor = ImVec4(1.00f, 0.3f, 0.5f, 1.00f);
	const ImVec4 TextDisabledColor = ImVec4(0.45125f, 0.45125f, 0.45125f, 1.00f);
	const ImVec4 WindowBgColor = ImVec4(0.01805f, 0.009075f, 0.01805f, 0.94f);
	const ImVec4 BorderColor = ImVec4(0.063175f, 0.027125f, 0.063175f, 1.00f);
	const ImVec4 BorderShadowColor = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	const ImVec4 FrameBgColor = ImVec4(0.09025f, 0.063175f, 0.1083f, 0.4f);
	const ImVec4 FrameBgActiveColor = ImVec4(0.09025f, 0.063175f, 0.1083f, 0.632f);
	const ImVec4 TitleBgCollapsedColor = ImVec4(0.00f, 0.00f, 0.00f, 0.4605f);
	const ImVec4 MenuBarBgColor = ImVec4(0.12635f, 0.12635f, 0.12635f, 1.00f);
	const ImVec4 ButtonHoveredColor = ImVec4(0.686f, 0.1895f, 0.6685f, 1.00f);
	const ImVec4 ButtonActiveColor = ImVec4(0.279f, 0.161f, 0.722f, 1.00f);
	const ImVec4 HeaderColor = ImVec4(0.81225f, 0.1895f, 0.44175f, 0.1805f);
	const ImVec4 HeaderHoveredColor = ImVec4(0.81225f, 0.1895f, 0.44175f, 0.45125f);
	const ImVec4 HeaderActiveColor = ImVec4(0.81225f, 0.1895f, 0.44175f, 0.45125f);
	const ImVec4 SeparatorColor = ImVec4(0.226625f, 0.1615f, 0.77615f, 0.45125f);
	const ImVec4 SeparatorHoveredColor = ImVec4(0.37905f, 0.11775f, 0.62275f, 0.7035f);
	const ImVec4 SeparatorActiveColor = ImVec4(0.491375f, 0.0361f, 0.722f, 0.9025f);
	const ImVec4 TabColor = ImVec4(0.207575f, 0.11825f, 0.361f, 0.77615f);
	const ImVec4 TabHoveredColor = ImVec4(0.406125f, 0.207575f, 0.77615f, 0.722f);
	const ImVec4 PlotLinesColor = ImVec4(0.5505f, 0.5505f, 0.5505f, 1.00f);
	const ImVec4 PlotLinesHoveredColor = ImVec4(0.9025f, 0.3885f, 0.316875f, 1.00f);
	const ImVec4 PlotHistogramColor = ImVec4(0.81225f, 0.63225f, 0.00f, 1.00f);
	const ImVec4 PlotHistogramHoveredColor = ImVec4(0.9025f, 0.5415f, 0.00f, 1.00f);
	const ImVec4 TableBorderStrongColor = ImVec4(0.279375f, 0.279375f, 0.316875f, 1.00f);
	const ImVec4 TableBorderLightColor = ImVec4(0.207575f, 0.207575f, 0.226625f, 1.00f);
	const ImVec4 TableRowBgAltColor = ImVec4(1.00f, 1.00f, 1.00f, 0.05375f);
	const ImVec4 TextSelectedBgColor = ImVec4(0.23465f, 0.53275f, 0.883f, 0.316875f);
	const ImVec4 DragDropTargetColor = ImVec4(0.9025f, 0.9025f, 0.00f, 0.81275f);
	const ImVec4 NavHighlightColor = ImVec4(0.23465f, 0.53275f, 0.883f, 0.95f);
	const ImVec4 NavWindowingHighlightColor = ImVec4(0.9025f, 0.9025f, 0.9025f, 0.63375f);
	const ImVec4 WindowDimBgColor = ImVec4(0.722f, 0.722f, 0.722f, 0.316875f);

	// Apply Color Settings to ImGui
	colors[ImGuiCol_Text] = TextColor;
	colors[ImGuiCol_TextDisabled] = TextDisabledColor;
	colors[ImGuiCol_WindowBg] = WindowBgColor;
	colors[ImGuiCol_ChildBg] = WindowBgColor;
	colors[ImGuiCol_PopupBg] = WindowBgColor;
	colors[ImGuiCol_Border] = BorderColor;
	colors[ImGuiCol_BorderShadow] = BorderShadowColor;
	colors[ImGuiCol_FrameBg] = FrameBgColor;
	colors[ImGuiCol_FrameBgHovered] = FrameBgActiveColor;
	colors[ImGuiCol_FrameBgActive] = FrameBgActiveColor;
	colors[ImGuiCol_TitleBg] = BorderColor;
	colors[ImGuiCol_TitleBgActive] = BorderColor;
	colors[ImGuiCol_TitleBgCollapsed] = TitleBgCollapsedColor;
	colors[ImGuiCol_MenuBarBg] = MenuBarBgColor;
	colors[ImGuiCol_Button] = TabColor;
	colors[ImGuiCol_ButtonHovered] = ButtonHoveredColor;
	colors[ImGuiCol_ButtonActive] = ButtonActiveColor;
	colors[ImGuiCol_Header] = HeaderColor;
	colors[ImGuiCol_HeaderHovered] = HeaderHoveredColor;
	colors[ImGuiCol_HeaderActive] = HeaderActiveColor;
	colors[ImGuiCol_Separator] = SeparatorColor;
	colors[ImGuiCol_SeparatorHovered] = SeparatorHoveredColor;
	colors[ImGuiCol_SeparatorActive] = SeparatorActiveColor;
	colors[ImGuiCol_SliderGrab] = TabColor;
	colors[ImGuiCol_SliderGrabActive] = ButtonActiveColor;
	colors[ImGuiCol_ScrollbarBg] = BorderShadowColor;
	colors[ImGuiCol_ScrollbarGrab] = TabColor;
	colors[ImGuiCol_ScrollbarGrabHovered] = ButtonHoveredColor;
	colors[ImGuiCol_ScrollbarGrabActive] = ButtonActiveColor;
	colors[ImGuiCol_ResizeGrip] = TabColor;
	colors[ImGuiCol_ResizeGripHovered] = ButtonHoveredColor;
	colors[ImGuiCol_ResizeGripActive] = ButtonActiveColor;
	colors[ImGuiCol_Tab] = TabColor;
	colors[ImGuiCol_TabHovered] = TabHoveredColor;
	colors[ImGuiCol_TabActive] = TabHoveredColor;
	colors[ImGuiCol_TabUnfocused] = TabColor;
	colors[ImGuiCol_TabUnfocusedActive] = TabColor;
	colors[ImGuiCol_PlotLines] = PlotLinesColor;
	colors[ImGuiCol_PlotLinesHovered] = PlotLinesHoveredColor;
	colors[ImGuiCol_PlotHistogram] = PlotHistogramColor;
	colors[ImGuiCol_PlotHistogramHovered] = PlotHistogramHoveredColor;
	colors[ImGuiCol_TableHeaderBg] = TabColor;
	colors[ImGuiCol_TableBorderStrong] = TableBorderStrongColor;
	colors[ImGuiCol_TableBorderLight] = TableBorderLightColor;
	colors[ImGuiCol_TableRowBg] = WindowBgColor;
	colors[ImGuiCol_TableRowBgAlt] = TableRowBgAltColor;
	colors[ImGuiCol_TextSelectedBg] = TextSelectedBgColor;
	colors[ImGuiCol_DragDropTarget] = DragDropTargetColor;
	colors[ImGuiCol_NavHighlight] = NavHighlightColor;
	colors[ImGuiCol_NavWindowingHighlight] = NavWindowingHighlightColor;
	colors[ImGuiCol_NavWindowingDimBg] = WindowDimBgColor;
	colors[ImGuiCol_ModalWindowDimBg] = WindowDimBgColor;


	static bool firstTimeStyling = true;
	if (firstTimeStyling)
	{
		// Style Rounding
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 4.0f;
		style.FrameRounding = 4.0f;
		style.PopupRounding = 4.0f;
		style.GrabRounding = 2.0f;
		style.ScrollbarRounding = 4.0f;
		style.TabRounding = 4.0f;

		//Size
		style.ScaleAllSizes(1.4f);
		style.FramePadding = ImVec2(6.0f, 3.0f);
		style.ItemSpacing = ImVec2(6.0f, 3.0f);
		style.IndentSpacing = 4.0f;

		firstTimeStyling = false;
	}

	switch (aTheme)
	{
	case eTheme::pink:
	{
		break;
	}
	case eTheme::warmGreen:
	{
		const float hueShift = 145.0f;
		const float saturationScale = 1.1f;
		ApplyColorTransform(colors, hueShift, saturationScale);
		break;
	}
	case eTheme::cyan:
	{
		const float hueShift = 240.0f;
		const float saturationScale = 1.0f;
		ApplyColorTransform(colors, hueShift, saturationScale);
		break;

	}
	case eTheme::ochre:
	{
		const float hueShift = 110.0f;
		const float saturationScale = 0.8f;
		ApplyColorTransform(colors, hueShift, saturationScale);
		break;
	}
	}

}



