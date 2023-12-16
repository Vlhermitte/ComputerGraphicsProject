#pragma once

#ifndef __OBJECT_H
#define __OBJECT_H

#include "pgr.h"

#define PLAYER_MODEL_NAME "data/ghoul/ghoul.obj"
#define TERRAIN_MODEL_NAME "data/terrain/terrain.obj"
#define FERRARI_MODEL_NAME "data/FerrariF399/FerrariF399.obj"
#define APPLE_MODEL_NAME "data/apple/apple.obj"

/**
 * \brief Shader program related stuff (id, locations, ...).
 */
typedef struct _ShaderProgram {
	/// identifier for the shader program
	GLuint program;

	bool initialized;

	/**
	  * \brief Indices of the vertex shader inputs (locations)
	  */
	struct {
		// vertex attributes locations
		GLint position;
		GLint color;
		GLint normal;
		GLint texCoord;

		// material
		GLint ambient;
		GLint diffuse;
		GLint specular;
		GLint shininess;

		// uniforms locations
		GLint PVM;
	} locations;

	// ...

	_ShaderProgram() : program(0), initialized(false) {
		locations.position = -1;
		locations.color = -1;
		locations.normal = -1;
		locations.texCoord = -1;

		locations.ambient = -1;
		locations.diffuse = -1;
		locations.specular = -1;
		locations.shininess = -1;

		locations.PVM = -1;
	}

} ShaderProgram;


/**
 * \brief Material of an object (ambient, diffuse, specular, shininess).
 */
typedef struct _Material {
	glm::vec3	  ambient;
	glm::vec3	  diffuse;
	glm::vec3	  specular;
	float		  shininess;
	GLuint		  texture;
} Material;

/**
 * \brief Geometry of an object (vertices, triangles).
 */
typedef struct _ObjectGeometry {
	GLuint        vertexBufferObject;   ///< identifier for the vertex buffer object
	GLuint        elementBufferObject;  ///< identifier for the element buffer object
	GLuint        vertexArrayObject;    ///< identifier for the vertex array object
	unsigned int  numTriangles;         ///< number of triangles in the mesh
	Material      material;             ///< material of the object
} ObjectGeometry;


typedef struct _Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
} Vertex;


/**
 * \brief Object in the scene.
 */
typedef struct Object {
	glm::vec3 position;
	glm::vec3 direction;
	float     speed;
	float     verticalSpeed;
	float     size;
	glm::vec3 enable_spot;

	bool destroyed;

	float startTime;
	float currentTime;

	bool isInitialized;
} Object;

typedef struct _Floor : public Object {
	glm::vec3 color;
} Floor;

typedef struct _Terrain : public Object {

} Terrain;

typedef struct _Player : public Object {
	float viewAngle; // in degrees

} Player;

typedef struct _Apple : public Object {

} Apple;

typedef struct _Ferrari : public Object {

} Ferrari;

typedef struct _Tree : public Object {

} Tree;


#endif // __OBJECT_H