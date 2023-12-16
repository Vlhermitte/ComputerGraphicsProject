#pragma once

#ifndef __RENDERER_H
#define __RENDERER_H

#include <vector>
#include "data.h"
#include "object.h"
#include "spline.h"


void loadShaderPrograms();
void cleanupShaderPrograms(void);


// -----------------------  Init scene objects ---------------------------------

void initFloor(ShaderProgram& shader, ObjectGeometry** geometry);
void initPlayer();
void initApple();
void initFerrari();

void initSceneObjects();

// -----------------------  Draw scene objects ---------------------------------

void drawFloor(Floor* Floor, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawPlayer(Player* Player, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawTerrain(Terrain* Terrain, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawApple(Apple* Apple, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void drawFerrari(Ferrari* Ferrari, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

// -----------------------  Clean up scene objects ----------------------------

void cleanupGeometry(ObjectGeometry* geometry);
void cleanupModels();

// -----------------------  Loading .obj file ---------------------------------

bool loadMeshes(const std::string& fileName, ShaderProgram& shader, std::vector<ObjectGeometry*>& geometries);
bool loadSingleMesh(const std::string& fileName, ShaderProgram& shader, ObjectGeometry** geometry);


#endif // __RENDERER_H