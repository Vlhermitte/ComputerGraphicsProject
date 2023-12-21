#include <iostream>

#include "renderer.h"



ObjectGeometry* floorGeometry = NULL;
ObjectGeometry* TerrainGeometry = NULL;
ObjectGeometry* PlayerGeometry = NULL;

std::vector<ObjectGeometry*> AppleGeometries;
std::vector<ObjectGeometry*> FerrariGeometries;

ShaderProgram commonShaderProgram;

glm::mat4 modelMatrixFloor;
glm::mat4 modelMatrixPlayer;
glm::mat4 modelMatrixApple;
glm::mat4 modelMatrixFerrari;

bool useLighting = false;

// -----------------------  OpenGL Stuff ---------------------------------

/**
 * \brief Load all shader programs.
 */
void loadShaderPrograms()
{

	GLuint shaders[] = {
	  pgr::createShaderFromFile(GL_VERTEX_SHADER, "vertexShader.vert"),
	  pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "fragementShader.frag"),
	  0
	};

	commonShaderProgram.program = pgr::createProgram(shaders);
	commonShaderProgram.locations.position = glGetAttribLocation(commonShaderProgram.program, "position");
	commonShaderProgram.locations.normal = glGetAttribLocation(commonShaderProgram.program, "normal");
	commonShaderProgram.locations.texCoord = glGetAttribLocation(commonShaderProgram.program, "texCoord");
	commonShaderProgram.locations.color = glGetAttribLocation(commonShaderProgram.program, "color_v");

	// material
	commonShaderProgram.locations.ambient = glGetUniformLocation(commonShaderProgram.program, "material.ambient");
	commonShaderProgram.locations.diffuse = glGetUniformLocation(commonShaderProgram.program, "material.diffuse");
	commonShaderProgram.locations.specular = glGetUniformLocation(commonShaderProgram.program, "material.specular");
	commonShaderProgram.locations.shininess = glGetUniformLocation(commonShaderProgram.program, "material.shininess");
	

	// other attributes and uniforms
	commonShaderProgram.locations.PVM = glGetUniformLocation(commonShaderProgram.program, "PVM");

	// Testing if all attributes are found
	assert(commonShaderProgram.locations.position != -1);
	// assert(commonShaderProgram.locations.normal != -1);
	assert(commonShaderProgram.locations.texCoord != -1);

	// Testing if all uniforms are found
	assert(commonShaderProgram.locations.ambient != -1);
	assert(commonShaderProgram.locations.diffuse != -1);
	assert(commonShaderProgram.locations.specular != -1);
	assert(commonShaderProgram.locations.shininess != -1);
	assert(commonShaderProgram.locations.PVM != -1);
	// ...

	commonShaderProgram.initialized = true;
}

/**
 * \brief Delete all shader program objects.
 */
void cleanupShaderPrograms(void) {

	pgr::deleteProgramAndShaders(commonShaderProgram.program);
}


// -----------------------  Init scene objects ---------------------------------

/**
 * \brief Init the objects in the scene.
 */

void initFloor(ShaderProgram& shader, ObjectGeometry** geometry) {

	*geometry = new ObjectGeometry;
	int floorTrianglesCount = sizeof(floorIndices) / sizeof(float);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
	CHECK_GL_ERROR();

	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * floorTrianglesCount, floorIndices, GL_STATIC_DRAW);
	CHECK_GL_ERROR();

	// FIX THIS : SEE HOW DRAW UFOs IS DONE
	// Assuming each vertex position is 3 floats (x, y, z)
	glEnableVertexAttribArray(shader.locations.position);
	glVertexAttribPointer(shader.locations.position, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	CHECK_GL_ERROR();

	// unbind the VAO
	glBindVertexArray(0);

	(*geometry)->numTriangles = floorTrianglesCount;
	
	std::cout << "Floor initialized" << std::endl;
}

void initTerrain() {
	if (loadSingleMesh(TERRAIN_MODEL_NAME, commonShaderProgram, &TerrainGeometry) != true) {
		std::cerr << "initTerrain() : Cannot load terrain model" << std::endl;
	}
}

void initPlayer() {
	if(loadSingleMesh(PLAYER_MODEL_NAME, commonShaderProgram, &PlayerGeometry) != true) {
		std::cerr << "initPlayer() : Cannot load player model" << std::endl;
	}

	CHECK_GL_ERROR();
}

void initApple() {
	if (loadMeshes(APPLE_MODEL_NAME, commonShaderProgram, AppleGeometries) != true) {
		std::cerr << "initApple() : Cannot load apple model" << std::endl;
	}
	CHECK_GL_ERROR();
}

void initFerrari() {
	if (loadMeshes(FERRARI_MODEL_NAME, commonShaderProgram, FerrariGeometries) != true) {
		std::cerr << "initFerrari() : Cannot load ferrari model" << std::endl;
	}
	CHECK_GL_ERROR();
}


void initSceneObjects() {
	useLighting = true;

	initTerrain();
	initPlayer();
	// initApple();
	// initFerrari();
}

// -----------------------  Drawing ---------------------------------

/**
 * \brief Draw the objects in the scene.
 */

void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(commonShaderProgram.locations.PVM, 1, GL_FALSE, glm::value_ptr(PVM));
	CHECK_GL_ERROR();
}

void setMaterialUniforms(const Material& material, glm::vec4 color) {

	if (useLighting) {
		glUniform3fv(commonShaderProgram.locations.ambient, 1, glm::value_ptr(material.ambient));
		glUniform3fv(commonShaderProgram.locations.diffuse, 1, glm::value_ptr(material.diffuse));
		glUniform3fv(commonShaderProgram.locations.specular, 1, glm::value_ptr(material.specular));
		glUniform1f(commonShaderProgram.locations.shininess, material.shininess);
		CHECK_GL_ERROR();
	}
	else {
		// FOR NOW WE SET THE COLOR MANUALLY
		glUniform4fv(commonShaderProgram.locations.color, 1, glm::value_ptr(color));
		CHECK_GL_ERROR();
	}
}

void drawFloor(Floor* Floor, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (commonShaderProgram.initialized && floorGeometry != NULL) {
		glUseProgram(commonShaderProgram.program);

		/*glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), Floor->position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(Floor->size, Floor->size, Floor->size));*/

		glm::mat4 modelMatrix = alignObject(Floor->position, Floor->direction, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(Floor->size));

		//// Move, rotate and scale the triangle
		//modelMatrixFloor = glm::translate(modelMatrixFloor, Floor->position);
		//modelMatrixFloor = glm::rotate(modelMatrixFloor, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate around x-axis
		//modelMatrixFloor = glm::rotate(modelMatrixFloor, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate around y-axis
		//modelMatrixFloor = glm::rotate(modelMatrixFloor, glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate around z-axis
		//modelMatrixFloor = glm::scale(modelMatrixFloor, glm::vec3(Floor->size));

		setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

		glBindVertexArray(floorGeometry->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, floorGeometry->numTriangles, GL_UNSIGNED_INT, (void*)0);

		glBindVertexArray(0);
		glUseProgram(0);
		CHECK_GL_ERROR();

		// std::cout << "Floor drawn" << std::endl;
	}
	else {
		std::cout << "drawFloor() : Shader program not initialized or floorGeometry NULL" << std::endl;
	}
}

void drawPlayer(Player* Player, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(commonShaderProgram.program);

	// prepare modeling transform matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), Player->position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(Player->viewAngle), glm::vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(Player->size, Player->size, Player->size));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	// set material uniforms
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	setMaterialUniforms(PlayerGeometry->material, color);

	glBindVertexArray(PlayerGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, PlayerGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0);
	glUseProgram(0);
}

void drawTerrain(Terrain* Terrain, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(commonShaderProgram.program);

	// prepare modeling transform matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), Terrain->position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(Terrain->size));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	// set material uniforms
	glm::vec4 color = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f);
	setMaterialUniforms(TerrainGeometry->material, color);

	// draw geometry
	glBindVertexArray(TerrainGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, TerrainGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void drawApple(Apple* Apple, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(commonShaderProgram.program);

	// prepare modeling transform matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), Apple->position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(2.0f), glm::vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(Apple->size));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	// draw geometry
	/*for (int i = 0; i < AppleGeometries.size(); i++) {
		setMaterialUniforms(AppleGeometries[i]->material);
		glBindVertexArray(AppleGeometries[i]->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, AppleGeometries[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
	}*/

	glBindVertexArray(0);
	glUseProgram(0);
}

void drawFerrari(Ferrari* Ferrari, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(commonShaderProgram.program);

	// prepare modeling transform matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), Ferrari->position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(2.0f), glm::vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(Ferrari->size));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	// draw geometry
	/*for (int i = 0; i < FerrariGeometries.size(); i++) {
		setMaterialUniforms(FerrariGeometries[i]->material);
		glBindVertexArray(FerrariGeometries[i]->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, FerrariGeometries[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
	}*/

	glBindVertexArray(0);
	glUseProgram(0);
}

// -----------------------  Cleanup scene objects ----------------------------

void cleanupGeometry(ObjectGeometry* geometry) {

	glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
	glDeleteBuffers(1, &(geometry->elementBufferObject));
	glDeleteBuffers(1, &(geometry->vertexBufferObject));

}

void cleanupModels() {
	cleanupGeometry(PlayerGeometry);
	cleanupGeometry(floorGeometry);
}

// -----------------------  Loading .obj file ---------------------------------


/** Load mesh using assimp library
 *  Vertex, normals and texture coordinates data are stored without interleaving |VVVVV...|NNNNN...|tttt
 * \param fileName [in] file to open/load
 * \param shader [in] vao will connect loaded data to shader
 * \param geometry
 */
bool loadMeshes(const std::string& fileName, ShaderProgram& shader, std::vector<ObjectGeometry*>& geometries) {
	Assimp::Importer importer;

	std::cout << "Loading model " << fileName << std::endl;

	// Unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	for (int i = 0; i < scn->mNumMeshes; i++) {
		std::cout << "Mesh " << i << " has " << scn->mMeshes[i]->mNumVertices << " vertices" << std::endl;
	}

	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we can access the data by scn->*
	for (int i = 0; i < scn->mNumMeshes; i++) {
		// get the current mesh
		const aiMesh* mesh = scn->mMeshes[i];

		ObjectGeometry* geometry = new ObjectGeometry();

		glGenBuffers(1, &geometry->vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->mNumVertices * 8, NULL, GL_STATIC_DRAW);
		CHECK_GL_ERROR();

		// copy vertices
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * mesh->mNumVertices * 3, mesh->mVertices);
		CHECK_GL_ERROR();

		// copy normals
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * mesh->mNumVertices * 3, sizeof(float) * mesh->mNumVertices * 3, mesh->mNormals);
		CHECK_GL_ERROR();

		// copy texture coordinates
		if (mesh->HasTextureCoords(0)) {
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * mesh->mNumVertices * 6, sizeof(float) * mesh->mNumVertices * 2, mesh->mTextureCoords[0]);
			CHECK_GL_ERROR();
		}

		// copy indices
		geometry->numTriangles = mesh->mNumFaces;
		glGenBuffers(1, &geometry->elementBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, NULL, GL_STATIC_DRAW);
		CHECK_GL_ERROR();

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * i * 3, sizeof(unsigned int) * 3, mesh->mFaces[i].mIndices);
			CHECK_GL_ERROR();
		}

		// copy material
		aiMaterial* material = scn->mMaterials[mesh->mMaterialIndex];

		aiColor3D color;
		aiString name;

		// ambient
		if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
			geometry->material.ambient = glm::vec3(color.r, color.g, color.b);
		}
		else {
			geometry->material.ambient = glm::vec3(0.0f);
		}

		// diffuse
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
			geometry->material.diffuse = glm::vec3(color.r, color.g, color.b);
		}
		else {
			geometry->material.diffuse = glm::vec3(0.0f);
		}

		// specular
		if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
			geometry->material.specular = glm::vec3(color.r, color.g, color.b);
		}
		else {
			geometry->material.specular = glm::vec3(0.0f);
		}

		// shininess
		unsigned int max;
		material->Get(AI_MATKEY_SHININESS, max);
		geometry->material.shininess = (float)max;

		// texture
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &name) == AI_SUCCESS) {
			std::string textureFileName = name.C_Str();
			std::cout << "Loading texture " << textureFileName << std::endl;

			// load texture
			geometry->material.texture = pgr::createTexture(textureFileName, true);
		}
		else {
			geometry->material.texture = 0;
		}

		// create vertex array object
		glGenVertexArrays(1, &geometry->vertexArrayObject);
		glBindVertexArray(geometry->vertexArrayObject);
		CHECK_GL_ERROR();

		// bind vertex attributes according to shader specifications
		glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
		glVertexAttribPointer(shader.locations.position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(shader.locations.position);
		CHECK_GL_ERROR();

		glVertexAttribPointer(shader.locations.normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * mesh->mNumVertices * 3));
		glEnableVertexAttribArray(shader.locations.normal);
		CHECK_GL_ERROR();

		if (mesh->HasTextureCoords(0)) {
			glVertexAttribPointer(shader.locations.texCoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * mesh->mNumVertices * 6));
			glEnableVertexAttribArray(shader.locations.texCoord);
			CHECK_GL_ERROR();
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject);
		CHECK_GL_ERROR();

		glBindVertexArray(0);
		CHECK_GL_ERROR();

		geometries.push_back(geometry);

	}

	return true;
}

bool loadSingleMesh(const std::string & fileName, ShaderProgram & shader, ObjectGeometry * *geometry) {
	Assimp::Importer importer;

	// Unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		*geometry = NULL;
		return false;
	}

	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
	if (scn->mNumMeshes != 1) {
		std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
		*geometry = NULL;
		return false;
	}

	// in this phase we know we have one mesh in our loaded scene, we can directly copy its data to OpenGL ...
	const aiMesh* mesh = scn->mMeshes[0];

	*geometry = new ObjectGeometry;

	// vertex buffer object, store all vertex positions and normals
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
	// first store all vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
	// then store all normals
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

	// just texture 0 for now
	float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
	float* currentTextureCoord = textureCoords;

	// copy texture coordinates
	aiVector3D vect;

	if (mesh->HasTextureCoords(0)) {
		// we use 2D textures with 2 coordinates and ignore the third coordinate
		for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
			vect = (mesh->mTextureCoords[0])[idx];
			*currentTextureCoord++ = vect.x;
			*currentTextureCoord++ = vect.y;
		}
	}

	// finally store all texture coordinates
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

	// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
	unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}

	// copy our temporary index array to OpenGL and free the array
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

	delete[] indices;

	// copy the material info to MeshGeometry structure
	const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
	aiColor4D color;
	aiString name;
	aiReturn retValue = AI_SUCCESS;

	// Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
	mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	(*geometry)->material.diffuse = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->material.ambient = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->material.specular = glm::vec3(color.r, color.g, color.b);

	ai_real shininess, strength;
	unsigned int max;	// changed: to unsigned

	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
		shininess = 1.0f;
	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
		strength = 1.0f;
	(*geometry)->material.shininess = shininess * strength;

	(*geometry)->material.texture = 0;

	// load texture image
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		// get texture name 
		aiString path; // filename

		aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		std::string textureName = path.data;

		size_t found = fileName.find_last_of("/\\");
		// insert correct texture file path 
		if (found != std::string::npos) { // not found
			//subMesh_p->textureName.insert(0, "/");
			textureName.insert(0, fileName.substr(0, found + 1));
		}

		std::cout << "Loading texture file: " << textureName << std::endl;
		(*geometry)->material.texture = pgr::createTexture(textureName);
	}
	CHECK_GL_ERROR();

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

	glEnableVertexAttribArray(shader.locations.position);
	glVertexAttribPointer(shader.locations.position, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (useLighting) {
		glEnableVertexAttribArray(shader.locations.normal);
		glVertexAttribPointer(shader.locations.normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));
	}
	else {
		glDisableVertexAttribArray(shader.locations.color);
		// following line is problematic on AMD/ATI graphic cards
		// -> if you see black screen (no objects at all) than try to set color manually in vertex shader to see at least something
		glVertexAttrib3f(shader.locations.color, color.r, color.g, color.b);
	}

	glEnableVertexAttribArray(shader.locations.texCoord);
	glVertexAttribPointer(shader.locations.texCoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
	CHECK_GL_ERROR();

	glBindVertexArray(0);

	(*geometry)->numTriangles = mesh->mNumFaces;

	return true;
}
