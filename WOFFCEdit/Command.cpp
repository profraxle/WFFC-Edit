#include "Command.h"

//create new command
Command::Command(std::vector<SceneObject> graph, int selected, BYTE heightMap[TERRAINRESOLUTION * TERRAINRESOLUTION]) {

	sceneGraph = graph;
	selectedObject = selected;
	for (int i = 0; i < TERRAINRESOLUTION * TERRAINRESOLUTION; ++i) {
		m_heightMap[i] = heightMap[i];
	}
}