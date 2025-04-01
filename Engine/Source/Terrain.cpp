#include "pch.h"
#include "Terrain.h"
#include "GraphicsEngine.h"
#include "TGPUtilities.hpp"
#include "uppgift05_helper.h"
#include "Texture.h"
#include <fstream>
#include <InputManager.h>

Terrain::Terrain()
{
    myGrassMaterial = new Texture;
    myRockMaterial = new Texture;
    mySandMaterial = new Texture;

    myGrassColor = new Texture;
    myRockColor = new Texture;
    mySandColor = new Texture;

    myGrassNormalMap = new Texture;
    myRockNormalMap = new Texture;
    mySandNormalMap = new Texture;

    myCubeMap = new Texture;
}

Terrain::~Terrain()
{
    delete myGrassMaterial;
    delete myRockMaterial;
    delete mySandMaterial;
    delete myGrassColor;
    delete myRockColor;
    delete mySandColor;
    delete myGrassNormalMap;
    delete myRockNormalMap;
    delete mySandNormalMap;
    delete myCubeMap;
}

bool Terrain::GenerateTerrain(int initialResolution, int numOctaves,
                                float initialAmplitude, float terrainSize)
{
    //GENERATE HEIGHT MAP--------------------------------------------------------------
    float halfSize = terrainSize * 0.5f;
    int resolution = initialResolution;
    float amplitude = initialAmplitude;

    std::vector<float> heightMap(resolution * resolution, 0.0f);

    for (int octave = 0; octave < numOctaves; ++octave)
    {
        // Add noise to the height map for the current octave
        AddNoise(heightMap, amplitude);

        heightMap = Upsample2X(heightMap, resolution);
        resolution *= 2;

        amplitude *= 0.5f;
    }

    myVertices.clear();
    myIndices.clear();
    //-----------------------------------------------------------------------------------

    // Generate vertices
    for (int z = 0; z < resolution; ++z)
    {
        for (int x = 0; x < resolution; ++x)
        {
            sVertex vertex;

            float posX = ((float)x / (resolution - 1)) * terrainSize - halfSize;
            float posZ = ((float)z / (resolution - 1)) * terrainSize - halfSize;
            float height = heightMap[z * resolution + x];

            vertex.pos = vector3f(posX, height, posZ);

            float uvScale = 7.0f; //Adjust for repeat scaling

            vertex.uv = vector2f((float)x / (resolution - 1) * uvScale, (float)z / (resolution - 1) * uvScale);

            myVertices.push_back(vertex);
        }
    }

    // Generate indices
    for (int z = 0; z < resolution - 1; ++z)
    {
        for (int x = 0; x < resolution - 1; ++x)
        {
            unsigned int index0 = z * resolution + x;
            unsigned int index1 = index0 + 1;
            unsigned int index2 = (z + 1) * resolution + x;
            unsigned int index3 = index2 + 1;

            myIndices.push_back(index0);
            myIndices.push_back(index2);
            myIndices.push_back(index1);

            myIndices.push_back(index1);
            myIndices.push_back(index2);
            myIndices.push_back(index3);
        }
    }

    myIndexCount = static_cast<unsigned int>(myIndices.size());

    if (!CalculateTBN(true))
    {
        TGPUtilities::CustomdebugOutput("Failed to calculate TBN");
        return false;
    }

    return true;
}

bool Terrain::LoadMaterial(GraphicsEngine* aGraphicsEngine)
{
    myGrassColor->LoadTexture("../Assets/Textures/Grass_c.png", aGraphicsEngine, true);
    myRockColor->LoadTexture("../Assets/Textures/Rock_c.png", aGraphicsEngine, true);
    mySandColor->LoadTexture("../Assets/Textures/Sand_c.png", aGraphicsEngine, true);

    myGrassMaterial->LoadTexture("../Assets/Textures/Grass_m.png", aGraphicsEngine, true);
    myRockMaterial->LoadTexture("../Assets/Textures/Rock_m.png", aGraphicsEngine, true);
    mySandMaterial->LoadTexture("../Assets/Textures/Sand_m.png", aGraphicsEngine, true);

    myGrassNormalMap->LoadTexture("../Assets/Textures/Grass_n.png", aGraphicsEngine, false);
    myRockNormalMap->LoadTexture("../Assets/Textures/Rock_n.png", aGraphicsEngine, false);
    mySandNormalMap->LoadTexture("../Assets/Textures/Sand_n.png",aGraphicsEngine, false);


    myGrassColor->Bind(aGraphicsEngine->GetDeviceContext().Get(), 0);
    myRockColor->Bind(aGraphicsEngine->GetDeviceContext().Get(), 1);
    mySandColor->Bind(aGraphicsEngine->GetDeviceContext().Get(), 2);

    myGrassNormalMap->Bind(aGraphicsEngine->GetDeviceContext().Get(), 3);
    myRockNormalMap->Bind(aGraphicsEngine->GetDeviceContext().Get(), 4);
    mySandNormalMap->Bind(aGraphicsEngine->GetDeviceContext().Get(), 5);

    myGrassMaterial->Bind(aGraphicsEngine->GetDeviceContext().Get(), 6);
    myRockMaterial->Bind(aGraphicsEngine->GetDeviceContext().Get(), 7);
    mySandMaterial->Bind(aGraphicsEngine->GetDeviceContext().Get(), 8);

    myCubeMap->LoadCubeMap(aGraphicsEngine);
    myCubeMap->Bind(aGraphicsEngine->GetDeviceContext().Get(), 9);

    return true;
}
