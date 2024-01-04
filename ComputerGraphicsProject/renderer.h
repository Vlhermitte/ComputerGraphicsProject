#pragma once

#ifndef __RENDERER_H
#define __RENDERER_H

#include <vector>
#include <list>
#include "data.h"
#include "object.h"
#include "spline.h"

extern ShaderProgram commonShaderProgram;
extern SkyboxShaderProgram skyboxShaderProgram;


typedef struct _GameObjects {
	Player* player;
	Terrain* terrain;
	Object* cube;
	Foxbat* foxbat;
	std::list<void*> explosions;
	Object* car;
	Object* tree1;
	Object* tree2;
} GameObjectsList;

extern GameObjectsList GameObjects;

void loadShaderPrograms();
void cleanupShaderPrograms(void);


// -----------------------  Init scene objects ---------------------------------

void initTerrain();
void initPlayer();
void initSkybox();
void initModel(const std::string ModelName, std::vector<ObjectGeometry*> ModelGeometries);

void initSceneObjects();

// -----------------------  Colision Detection -------------------------------
glm::vec3 checkBounds(const glm::vec3& position, float objectSize);

// -----------------------  Draw scene objects ---------------------------------

void drawTerrain(Terrain* Terrain, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawPlayer(Player* Player, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawModel(Object* Model, std::vector<ObjectGeometry*> ModelGeometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

void drawObjects(GameObjectsList GameObjects, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);


// -----------------------  Clean up scene objects ----------------------------

void cleanupGeometry(ObjectGeometry* geometry);
void cleanupModels();

// -----------------------  Loading .obj file ---------------------------------
bool loadMeshes(const std::string& fileName, ShaderProgram& shader, std::vector<ObjectGeometry*>& geometries);
bool loadSingleMesh(const std::string& fileName, ShaderProgram& shader, ObjectGeometry** geometry);


#endif // __RENDERER_H