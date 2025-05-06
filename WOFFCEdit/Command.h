#pragma once
#include <vector>
#include "SceneObject.h"
#include "DisplayChunk.h"

//Class to store state of the world at time of command

class Command
{

public:
	Command(std::vector<SceneObject> graph,int selected, BYTE heightMap[TERRAINRESOLUTION * TERRAINRESOLUTION]);

	std::vector<SceneObject> sceneGraph;
	int selectedObject;
	BYTE m_heightMap[TERRAINRESOLUTION * TERRAINRESOLUTION];

};

