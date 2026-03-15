#pragma once
#include "CommonUtilities/include/Vector3.hpp"
#include "LSystem.h"

struct sPlantParams
{
    CommonUtilities::Vector3<float> rotationA = { 20.0f, 20.0f, 20.0f };
    CommonUtilities::Vector3<float> rotationB = { -20.0f, -20.0f, -20.0f };
    CommonUtilities::Vector3<float> intermediateRotationA = { 20.0f, 20.0f, 20.0f };
    CommonUtilities::Vector3<float> intermediateRotationB = { -20.0f, -20.0f, -20.0f };
    LSystem lSystem;

    //initial
    float initialWidth = 0.25f;
    float initialLength = 1.0f;

    //additional symbols
    float additionalWidthModifier = 0.3f;             //# and !
    float additionalHeightModifier = 0.3f;            //< and >
    float additionalTurningAngleIncrement = 5.0f;     //( and )

    //general scale factor
    float generalScaleFactorWidth = 0.1f;
    float generalScaleFactorHeight = 0.1f;

    //leaf
    CommonUtilities::Vector3<float> leafRotation = { 0.0f, 0.0f, 0.0f };
    float randomLeafRotation = 0.0f;
    float randomLeafScale = 1.0f;
    float leafSize = 0.1f;
    float branchSpikyness = 1.1f;
    bool useLeaves = false;
};
