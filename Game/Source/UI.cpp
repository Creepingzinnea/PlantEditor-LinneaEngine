#include "pch.h"
#include "UI.h"
#include "GraphicsEngine.h"
#include "Plant.h"
#include "TGPUtilities.hpp"
#include "ImguiInterface.h"
#include "SerialisePlant.h"
#include <imgui_docking/misc/cpp/imgui_stdlib.h>
#include <filesystem>

void UI::Render(const ImTextureID aTextureID, WaterPlane& aWaterPlane, Plant* aPlant)
{
	SetupDockSpace();
	SetupInitialDockingLayout();

	RenderSceneWindow(aTextureID);
	RenderThemesWindow();
	RenderWorldWindow(aWaterPlane);
	RenderLeafWindow(aPlant);
	RenderPlantWindow(aPlant);

	ImGui::End();
}

void UI::Init(GraphicsEngine* aGraphicsEngine, Plant& aPlant)
{
	myInputLsystem = aPlant.myParams.lSystem;
	myGraphicsEnginePtr = aGraphicsEngine;
	ImGuiInterface::Style(eTheme::ochre);
	myAmbientLight = aGraphicsEngine->GetAmbientLight();
	myDirectionalLight = aGraphicsEngine->GetDirectionalLight();
}

void UI::BeginFrame(float aDeltaTime)
{
	myDeltaTime = aDeltaTime;

	myGraphicsEnginePtr->PrepareTerrainBufferData(&myTerrainBuffer);
	myGraphicsEnginePtr->SetAmbientLight(myAmbientLight);
	myGraphicsEnginePtr->SetDirectionalLight(myDirectionalLight);
}

void UI::SetupDockSpace()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace_Main", nullptr, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
}

void UI::SetupInitialDockingLayout()
{
	if (firstTime)
	{
		firstTime = false;

		ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::DockBuilderRemoveNode(dockspaceId);
		ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);

		ImGuiID dockMainID, dockRightID;
		ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Right, 0.3f, &dockRightID, &dockMainID);

		ImGui::DockBuilderGetNode(dockMainID)->LocalFlags |= ImGuiDockNodeFlags_NoResize;
		ImGui::DockBuilderGetNode(dockRightID)->LocalFlags |= ImGuiDockNodeFlags_NoResize;

		ImGui::DockBuilderDockWindow("Scene", dockMainID);
		ImGui::DockBuilderDockWindow("Themes", dockRightID);
		ImGui::DockBuilderDockWindow("World", dockRightID);
		ImGui::DockBuilderDockWindow("Leaf", dockRightID);
		ImGui::DockBuilderDockWindow("Plant", dockRightID);

		ImGui::DockBuilderFinish(dockspaceId);
	}
}

void UI::RenderSceneWindow(const ImTextureID aTextureID)
{
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoCollapse);

	//main content
	ImVec2 availSize = ImGui::GetContentRegionAvail();
	ImGui::Image(aTextureID, availSize);
	ImGui::End();
}

void UI::RenderPlantWindow(Plant* aPlant)
{
	ImGui::Begin("Plant", nullptr);

	GeneralPlant(aPlant);
	UILSystem(aPlant);
	Presets(aPlant);

	ImGui::End();

	//---------------------------------------------------------------------------------------------------------------------

	if (aPlant->myStateChanged && !postponeOneframe)
	{
		aPlant->Update();
	}
	postponeOneframe = false;
}

void UI::RenderLeafWindow(Plant* aPlant)
{
	ImGui::Begin("Leaf", nullptr, ImGuiWindowFlags_NoCollapse);

	//Use leaves------------------------------------------------------------------------------------------------------------
	if (ImGui::Checkbox("Use leaves", &aPlant->myParams.useLeaves))
	{
		postponeOneframe = true;
		myInputLsystem = aPlant->myParams.lSystem;
		aPlant->FullUpdate();
	}
	if (postponeOneframe)
	{
		DisplayWaitingInfo(false);
	}
	ImGui::NewLine();

	//Material----------------------------------------------------------------------------------------------------------------
	ImGui::Text("Leaf material");
	ImGui::InputText("##Leaf material", &myInputLeafMesh);
	ImGui::SameLine();
	if (ImGui::Button("Apply material"))
	{
		std::string fullName = "Leaf/" + myInputLeafMesh;
		aPlant->SetLeafMaterial(fullName, myGraphicsEnginePtr);
	}
	ImGui::NewLine();

	//Scale--------------------------------------------------------------------------------------------------------------------
	if (ImGui::CollapsingHeader("Scale"))
	{
		ImGui::Indent();

		ImGui::Text("Random scale factor");
		if (ImGui::SliderFloat("##LeafRandomScale", &aPlant->myParams.randomLeafScale, 0.0f, 1.0f))
		{
			aPlant->SetLeafRandomScale();
			aPlant->myStateChanged = true;
		};

		ImGui::SliderFloat("##LeafScale", &aPlant->myParams.leafSize, 0.01f, 1.0f);
		aPlant->myStateChanged = true;

		ImGui::Unindent();
	}
	ImGui::NewLine();

	//Rotation------------------------------------------------------------------------------------------------------------------
	if (ImGui::CollapsingHeader("Leaf rotation"))
	{
		ImGui::Indent();

		ImGui::Text("Random rotation factor");
		if (ImGui::SliderFloat("##LeafRandomRot", &aPlant->myParams.randomLeafRotation, 0.0f, 180.0f))
		{
			aPlant->SetLeafRandomRotation();
			aPlant->myStateChanged = true;
		};

		ImGui::NewLine();

		if (ImGui::SliderFloat("##Leaf rotationX", &aPlant->myParams.leafRotation.x, 360.0f, -360.0f)) { aPlant->myStateChanged = true; };
		if (ImGui::SliderFloat("##Leaf rotationY", &aPlant->myParams.leafRotation.y, 360.0f, -360.0f)) { aPlant->myStateChanged = true; };
		if (ImGui::SliderFloat("##Leaf rotationZ", &aPlant->myParams.leafRotation.z, 360.0f, -360.0f)) { aPlant->myStateChanged = true; };

		ImGui::Unindent();
	}
	ImGui::NewLine();

	ImGui::End();
}

void UI::RenderWorldWindow(WaterPlane& aWaterPlane)
{
	ImGui::Begin("World", nullptr, ImGuiWindowFlags_NoCollapse);

	if (ImGui::CollapsingHeader("Light"))
	{
		ImGui::Indent();
		if (ImGui::CollapsingHeader("Directional"))
		{
			ImGui::Indent();
			ImGui::PushID("DirLight");

			// Label above the slider for Direction x
			ImGui::Text("Direction x");
			ImGui::SliderFloat("##DirLightX", &myDirectionalLight.direction.x, -1, 1);

			// Label above the slider for Direction y
			ImGui::Text("Direction y");
			ImGui::SliderFloat("##DirLightY", &myDirectionalLight.direction.y, -1, 1);

			// Label above the slider for Direction z
			ImGui::Text("Direction z");
			ImGui::SliderFloat("##DirLightZ", &myDirectionalLight.direction.z, -1, 1);

			// Label above the slider for Color
			ImGui::Text("Color");
			ImGui::ColorEdit3("##DirLightColor", &myDirectionalLight.colorAndIntensity.x);

			// Label above the slider for Intensity
			ImGui::Text("Intensity");
			ImGui::SliderFloat("##DirLightIntensity", &myDirectionalLight.colorAndIntensity.w, 0, 30);

			ImGui::PopID();
			ImGui::Unindent();
		}
		if (ImGui::CollapsingHeader("Ambient"))
		{
			ImGui::Indent();
			ImGui::PushID("AmbLight");

			// Label above the slider for Ambient Color
			ImGui::Text("Color");
			ImGui::ColorEdit3("##AmbLightColor", &myAmbientLight.colorAndIntensity.x);

			// Label above the slider for Ambient Intensity
			ImGui::Text("Intensity");
			ImGui::SliderFloat("##AmbLightIntensity", &myAmbientLight.colorAndIntensity.w, 0, 30);

			ImGui::Unindent();
			ImGui::PopID();
		}
		ImGui::Unindent();
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Terrain"))
	{
		ImGui::Indent();
		if (ImGui::CollapsingHeader("Slope Blend", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float sliderWidth = ImGui::GetContentRegionAvail().x * 0.45f; // Adjust slider width
			ImGui::Indent();
			ImGui::PushItemWidth(sliderWidth);

			// Label above the slider for Slope Min
			ImGui::Text("Min");
			ImGui::SliderFloat("##SlopeMin", &myTerrainBuffer.slopeBlendMin, -1, 1);

			// Label above the slider for Slope Max
			ImGui::Text("Max");
			ImGui::SliderFloat("##SlopeMax", &myTerrainBuffer.slopeBlendMax, -1, 1);

			ImGui::PopItemWidth();
			ImGui::Unindent();
		}

		// Height Blend Section
		if (ImGui::CollapsingHeader("Height Blend", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float sliderWidth = ImGui::GetContentRegionAvail().x * 0.45f;
			ImGui::Indent();
			ImGui::PushItemWidth(sliderWidth);

			// Label above the slider for Height Min
			ImGui::Text("Min");
			ImGui::SliderFloat("##HeightMin", &myTerrainBuffer.heightBlendMin, -10, 10);

			// Label above the slider for Height Max
			ImGui::Text("Max");
			ImGui::SliderFloat("##HeightMax", &myTerrainBuffer.heightBlendMax, -10, 10);

			ImGui::PopItemWidth();
			ImGui::Unindent();
		}
		ImGui::Unindent();
		ImGui::Separator();
	}

	ImGui::NewLine();

	// Label above the Water height slider
	ImGui::Text("Water height");
	ImGui::SliderFloat("##WaterHeight", &aWaterPlane.myTransform.myMatrix(4, 2), -10.f, 10.f);

	ImGui::Separator();

	ImGui::End();
}

void UI::RenderThemesWindow()
{
	ImGui::Begin("Themes");

	if (ImGui::Selectable("Cyan"))
	{
		ImGuiInterface::Style(eTheme::cyan);
	}
	if (ImGui::Selectable("Ochre"))
	{
		ImGuiInterface::Style(eTheme::ochre);
	}
	if (ImGui::Selectable("Pink"))
	{
		ImGuiInterface::Style(eTheme::pink);
	}
	if (ImGui::Selectable("Green"))
	{
		ImGuiInterface::Style(eTheme::warmGreen);
	}

	ImGui::End();
}

void UI::DisplayWaitingInfo(bool displayBelow)
{
	if (displayBelow)
	{
		ImGui::Text("This might take a minue...");
	}
	else
	{
		ImGui::SameLine();
		ImGui::Text("This might take a minue...");
		ImGui::NewLine();
	}
	postponeOneframe = true;
}

void UI::GrammarWindow()
{
	if (showGrammar)
	{
		ImVec2 buttonPos = ImGui::GetCursorScreenPos();
		ImVec2 windowPos(buttonPos.x - 450, buttonPos.y);
		ImGui::SetNextWindowPos(windowPos);
		ImGui::Begin("Grammar", &showGrammar, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		ImGui::Text("F\tMove forward");
		ImGui::Text("+\tRotate branch with 'RotationA'");
		ImGui::Text("-\tRotate branch with 'RotationB'");
		ImGui::Text("[\tOpen branch (Push current drawing state to stack)");
		ImGui::Text("]\tClose branch (Pop current drawing state from stack)");
		ImGui::Text("#\tIncrement line width");
		ImGui::Text("!\tDecrement line width");
		ImGui::Text(">\tMultiply branch length by length scale factor");
		ImGui::Text("<\tDivide branch length by length scale factor");
		ImGui::Text("&\tSwap meaning of < and >");
		ImGui::Text("(\tDecrement turning angle by turning angle increment");
		ImGui::Text(")\tIncrement turning angle by turning angle increment");
		ImGui::End();
	}
}

void UI::GeneralPlant(Plant* aPlant)
{
	if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();
		if (ImGui::CollapsingHeader("Branch shape"))
		{
			ImGui::Indent();

			ImGui::Text("Branch spikyness");
			if (ImGui::SliderFloat("##Branch spikyness", &aPlant->myParams.branchSpikyness, 0.0f, 1.0f))
			{
				aPlant->myStateChanged = true;
			}

			ImGui::Text("Initial thickness");
			if (ImGui::SliderFloat("##InitialThickness", &aPlant->myParams.initialWidth, 0.01f, 1.0f))
			{
				aPlant->myStateChanged = true;
			}

			ImGui::Text("Initial length");
			if (ImGui::SliderFloat("##InitialLength", &aPlant->myParams.initialLength, 0.01f, 1.0f))
			{
				aPlant->myStateChanged = true;
			}

			ImGui::Text("Scale-factor width");
			if (ImGui::SliderFloat("##ScaleFactorWidth", &aPlant->myParams.generalScaleFactorWidth, 0.1f, 2.0f))
			{
				aPlant->myStateChanged = true;
			}

			ImGui::Text("Scale-factor length");
			if (ImGui::SliderFloat("##ScaleFactorLength", &aPlant->myParams.generalScaleFactorHeight, 0.1f, 2.0f))
			{
				aPlant->myStateChanged = true;
			}
		}

		if (ImGui::CollapsingHeader("Branch rotation"))
		{
			ImGui::Indent();
			if (ImGui::CollapsingHeader("Rotation A", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushID(&aPlant->myParams.rotationA);

				ImGui::Text("Rotation x:");
				if (ImGui::SliderFloat("##RotationAX", &aPlant->myParams.rotationA.x, -180.0f, 180.0f))
				{
					aPlant->myStateChanged = true;
				}

				ImGui::Text("Rotation y:");
				if (ImGui::SliderFloat("##RotationAY", &aPlant->myParams.rotationA.y, -180.0f, 180.0f))
				{
					aPlant->myStateChanged = true;
				}

				ImGui::Text("Rotation z:");
				if (ImGui::SliderFloat("##RotationAZ", &aPlant->myParams.rotationA.z, -180.0f, 180.0f))
				{
					aPlant->myStateChanged = true;
				}

				ImGui::PopID();
			}
			ImGui::NewLine();

			if (ImGui::CollapsingHeader("Rotation B", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushID(&aPlant->myParams.rotationB);

				ImGui::Text("Rotation x:");
				if (ImGui::SliderFloat("##RotationBX", &aPlant->myParams.rotationB.x, -180.0f, 180.0f))
				{
					aPlant->myStateChanged = true;
				}

				ImGui::Text("Rotation y:");
				if (ImGui::SliderFloat("##RotationBY", &aPlant->myParams.rotationB.y, -180.0f, 180.0f))
				{
					aPlant->myStateChanged = true;
				}

				ImGui::Text("Rotation z:");
				if (ImGui::SliderFloat("##RotationBZ", &aPlant->myParams.rotationB.z, -180.0f, 180.0f))
				{
					aPlant->myStateChanged = true;
				}

				ImGui::PopID();
			}
			ImGui::Unindent();
			ImGui::NewLine();
		}


		ImGui::Unindent();
		if (ImGui::Button("Reset"))
		{
			DisplayWaitingInfo();
			aPlant->myParams = aPlant->myPreset;
			myInputLsystem = aPlant->myParams.lSystem;
			aPlant->FullUpdate();
		}
	}
	ImGui::NewLine();
}

void UI::UILSystem(Plant* aPlant)
{
	if (ImGui::CollapsingHeader("L-system", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//Lsystem-------------------------------------------------------------------------------------------------
		int& depth = myInputLsystem.AccessDepth();
		ImGui::Text("Iterations:");

		float alignment = 100.0f;
		ImGui::SameLine(alignment);
		if (ImGui::InputInt("##Iterations", &depth, 1, 5))
		{
			depth = TGPUtilities::Clampi(depth, 1, 10);
		}
		ImGui::NewLine();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Axiom:");
		ImGui::SameLine(alignment);
		ImGui::InputText("##Axiom", &myInputLsystem.AccessAxiom());
		ImGui::NewLine();

		const char* ruleLabels[] = { "F ->", "X ->", "Y ->", "Z ->", "W ->" };
		for (int i = 0; i < 5; ++i)
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("%s", ruleLabels[i]);
			ImGui::SameLine(alignment);
			std::string inputLabel = "##" + std::string(ruleLabels[i]);
			ImGui::InputText(inputLabel.c_str(), &myInputLsystem.AccessRules()[static_cast<int>(eRules(i))]);
		}

		//Wiev grammar---------------------------------------------------------------------------------------------------------
		if (ImGui::Button("View grammar"))
		{
			showGrammar = !showGrammar;
		}
		GrammarWindow();

		//Additional symbol attributes-------------------------------------------------------------------------------------------------
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("L-system symbol attributes");
		ImGui::NewLine();

		ImGui::Text("# !");
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("This value will increment and respectivly decrement the branch width \n whenever '#' or '!' symbols are found in the L-system.");
		}
		ImGui::InputFloat("##Branch Thickness Modifier (for # symbol)", &aPlant->myParams.additionalWidthModifier, 0.1f, 10.0f);

		ImGui::Text("> <");
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("This value will multiply and respectivly divide the branch length \n whenever '>' or '<' symbols are found in the L-system.");
		}
		ImGui::InputFloat("##Branch length modifier", &aPlant->myParams.additionalHeightModifier, 0.1f, 10.0f);

		ImGui::Text("( )");
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("This value will increment and respectivly decrement the rotation values \n whenever '(' or ')' symbols are found in the L-system.");
		}
		ImGui::InputFloat("##Rotation modifier", &aPlant->myParams.additionalTurningAngleIncrement, 0.1f, 90.0f);

		ImGui::NewLine();

		//Apply------------------------------------------------------------------------------------------------------------------
		if (ImGui::Button("Apply L-system"))
		{
			myInputLsystem.LSystemGenerate();
			if (myInputLsystem.AnalyseLsystemPerformance())
			{
				DisplayWaitingInfo();
				aPlant->myParams.lSystem = myInputLsystem;
				aPlant->FullUpdate();
				myInputLsystem = aPlant->myParams.lSystem;
			}
			else
			{
				myInputLsystem = aPlant->myParams.lSystem;
				myTextTimer = mySetTextTime;
			}
		}
		myTextTimer -= myDeltaTime;
		if (myTextTimer > 0)
		{
			ImGui::SameLine();
			ImGui::Text("The generated L-system is too long.\n Try decresing iterations or reduce rules.");
		}
		ImGui::Unindent();
	}
	ImGui::NewLine();
}

void UI::Presets(Plant* aPlant)
{
	PlantSerializer serialize;
	std::string filePath = "../Assets/Config/PlantPresets.txt";

	if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputText("Preset name", &presetName);
		if (ImGui::Button("Save Preset"))
		{
			if (strlen(presetName.c_str()) > 0 && strlen(presetName.c_str()) <= 20)
			{
				if (serialize.TrySavePreset(filePath, presetName))
				{
					ImGui::OpenPopup("##OverwriteConfirmation");
				}
				else
				{
					serialize.SavePreset(filePath, presetName, aPlant->myParams);
				}
				serialize.LoadPresets(filePath, aPlant->myPresets);
				myInputLsystem = aPlant->myParams.lSystem;
			}
			else
			{
				ImGui::SetItemDefaultFocus();
				ImGui::Text("Name too long or invalid characters");
			}
		}

		//Overwrite confirmatio pop up definition
		if (ImGui::BeginPopupModal("##OverwriteConfirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			std::string popUpText = "The preset name  ''" + presetName + "'' already exists.\nAre you sure you want to overwrite it ? ";
			ImGui::Text(popUpText.c_str());
			ImGui::NewLine();

			if (ImGui::Button("Save Preset")) // Fixed typo "Peset" to "Preset"
			{
				serialize.SavePreset(filePath, presetName, aPlant->myParams);
				serialize.LoadPresets(filePath, aPlant->myPresets);
				myInputLsystem = aPlant->myParams.lSystem;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::NewLine();

		ImGui::Text("Load preset");
		if (ImGui::BeginCombo("##Load preset", aPlant->mySelectedPreset.c_str()))
		{
			for (auto& pair : aPlant->myPresets)
			{
				const std::string& label = pair.first;

				if (ImGui::Selectable(label.c_str()))
				{
					postponeOneframe = true;
					aPlant->UpdatePreset(label);
				}
			}
			ImGui::EndCombo();
			if (postponeOneframe)
			{
				myInputLsystem = aPlant->myParams.lSystem;
				DisplayWaitingInfo(true);
			}
		};
	}
	ImGui::NewLine();
}
