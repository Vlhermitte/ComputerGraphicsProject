/*
* \file data.h
* \author Valentin Lhermitte
* \date 2023-2024
* \brief Various parameters
*/

#pragma once

#ifndef __DATA_H
#define __DATA_H

#include "pgr.h"

#define SCENE_WIDTH  1.0f
#define SCENE_HEIGHT 1.0f
#define SCENE_DEPTH  1.0f

#define PLAYER_SIZE 0.05f
#define TERRAIN_SIZE 1.0f
#define CUBE_SIZE 0.1f
#define FLOOR_SIZE 1.0f
#define AIRCRAFT_SIZE 0.1f
#define CAR_SIZE 0.1f
#define TREE_SIZE 0.25f
#define TORCH_SIZE 1.0f
#define BANNER_SIZE 1.0f

#define CAMERA_ELEVATION_MAX 50.0f

#define PLAYER_SPEED_INCREMENT 0.2f
#define PLAYER_UP_SPEED_INCREMENT 0.1f
#define PLAYER_SPEED_MAX 0.4f
#define PLAYER_SPEED_MIN -0.2f
#define PLAYER_VIEW_ANGLE_DELTA 2.0f

#define MAX_HEIGHT 0.15f
#define MIN_HEIGHT -0.2f

enum { 
	KEY_LEFT_ARROW, 
	KEY_RIGHT_ARROW, 
	KEY_UP_ARROW, 
	KEY_DOWN_ARROW, 
	KEY_SPACE,
	KEY_B,
	KEY_SHIFT_L,
	KEYS_COUNT
};



const float cubeVertices[] = {
	// position (x, y, z)   u, v   normal (x, y, z)
	// Front face (z positive)
	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,

	// Back face (z negative)
	-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f, 0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,

	// Left face (x negative)
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, 1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,

	// Right face (x positive)
	 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,    1.0f, 0.0f, 0.0f,

	 // Top face (y positive)
	 -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
	  0.5f,  0.5f, -0.5f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
	  0.5f,  0.5f,  0.5f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
	 -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,

	 // Bottom face (y negative)
	 -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
	  0.5f, -0.5f, -0.5f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
	  0.5f, -0.5f,  0.5f, 0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
	 -0.5f, -0.5f,  0.5f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f
};


const unsigned int cubeIndices[] = {
	// Front face
	0, 1, 2, 
	2, 3, 0,

	// Back face
	4, 5, 6,
	6, 7, 4,

	// Left face
	8, 9, 10,
	10, 11, 8,

	// Right face
	12, 13, 14,
	14, 15, 12,

	// Top face
	16, 17, 18,
	18, 19, 16,

	// Bottom face
	20, 21, 22,
	22, 23, 20
};


const int explosionNumQuadVertices = 4;
const float explosionVertexData[explosionNumQuadVertices * 5] = {
  // x      y     z     u     v
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
   1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};


const int bannerNumQuadVertices = 4;
const float bannerVertexData[bannerNumQuadVertices * 5] = {
	  //  x      y     z     u     v
	  -1.0f,  0.15f, 0.0f, 0.0f, 1.0f,
	  -1.0f, -0.15f, 0.0f, 0.0f, 0.0f,
	   1.0f,  0.15f, 0.0f, 3.0f, 1.0f,
	   1.0f, -0.15f, 0.0f, 3.0f, 0.0f
};

#endif // __DATA_H