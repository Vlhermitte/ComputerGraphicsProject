#include <iostream>

#include "renderer.h"

ObjectGeometry* TerrainGeometry = NULL;
ObjectGeometry* PlayerGeometry = NULL;
ObjectGeometry* SkyboxGeometry = NULL;
std::vector<ObjectGeometry*> FoxBatGeometries;
std::vector<ObjectGeometry*> CarGeometries;

ShaderProgram commonShaderProgram;
SkyboxShaderProgram skyboxShaderProgram;

GameObjectsList GameObjects;

bool useLighting = false;

// -----------------------  OpenGL Stuff ---------------------------------

/**
 * \brief Load all shader programs.
 */
void loadShaderPrograms() {

	std::vector<GLuint> shaderList;

	/*GLuint shaders[] = {
	  pgr::createShaderFromFile(GL_VERTEX_SHADER, "vertexShader.vert"),
	  pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "fragementShader.frag"),
	};*/

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "vertexShader.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "fragementShader.frag"));

	commonShaderProgram.program = pgr::createProgram(shaderList);
	commonShaderProgram.locations.position = glGetAttribLocation(commonShaderProgram.program, "position");
	commonShaderProgram.locations.normal = glGetAttribLocation(commonShaderProgram.program, "normal");
	commonShaderProgram.locations.texCoord = glGetAttribLocation(commonShaderProgram.program, "texCoord");

	// material
	commonShaderProgram.locations.ambient = glGetUniformLocation(commonShaderProgram.program, "material.ambient");
	commonShaderProgram.locations.diffuse = glGetUniformLocation(commonShaderProgram.program, "material.diffuse");
	commonShaderProgram.locations.specular = glGetUniformLocation(commonShaderProgram.program, "material.specular");
	commonShaderProgram.locations.shininess = glGetUniformLocation(commonShaderProgram.program, "material.shininess");
	commonShaderProgram.locations.useTexture = glGetUniformLocation(commonShaderProgram.program, "material.useTexture");
	commonShaderProgram.locations.texSampler = glGetUniformLocation(commonShaderProgram.program, "texSampler");

	// other attributes and uniforms
	commonShaderProgram.locations.PVM = glGetUniformLocation(commonShaderProgram.program, "PVM");
	commonShaderProgram.locations.ViewMatrix = glGetUniformLocation(commonShaderProgram.program, "ViewMatrix");
	commonShaderProgram.locations.ModelMatrix = glGetUniformLocation(commonShaderProgram.program, "ModelMatrix");
	commonShaderProgram.locations.NormalMatrix = glGetUniformLocation(commonShaderProgram.program, "NormalMatrix");
	commonShaderProgram.locations.time = glGetUniformLocation(commonShaderProgram.program, "time");
	commonShaderProgram.locations.fogOn = glGetUniformLocation(commonShaderProgram.program, "fogOn");


	// Testing if all attributes are found
	assert(commonShaderProgram.locations.position != -1);
	assert(commonShaderProgram.locations.normal != -1);
	assert(commonShaderProgram.locations.texCoord != -1);

	// Testing if all uniforms are found
	assert(commonShaderProgram.locations.ambient != -1);
	assert(commonShaderProgram.locations.diffuse != -1);
	assert(commonShaderProgram.locations.specular != -1);
	assert(commonShaderProgram.locations.shininess != -1);
	assert(commonShaderProgram.locations.useTexture != -1);
	assert(commonShaderProgram.locations.texSampler != -1);

	assert(commonShaderProgram.locations.PVM != -1);
	assert(commonShaderProgram.locations.ViewMatrix != -1);
	assert(commonShaderProgram.locations.ModelMatrix != -1);
	assert(commonShaderProgram.locations.NormalMatrix != -1);
	
	WARN_IF(commonShaderProgram.locations.time == -1, "commonShaderProgram.locations.time == -1");
	WARN_IF(commonShaderProgram.locations.fogOn == -1, "commonShaderProgram.locations.fogOn == -1");

	commonShaderProgram.initialized = true;
	shaderList.clear();

	// Skybox Shaders

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "skyboxVertexShader.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "skyboxFragmentShader.frag"));

	skyboxShaderProgram.program = pgr::createProgram(shaderList);

	skyboxShaderProgram.locations.screenCoord = glGetAttribLocation(skyboxShaderProgram.program, "screenCoord");
	// get uniforms locations
	skyboxShaderProgram.locations.skyboxSampler = glGetUniformLocation(skyboxShaderProgram.program, "skyboxSampler");
	skyboxShaderProgram.locations.inversePVmatrix = glGetUniformLocation(skyboxShaderProgram.program, "inversePVmatrix");


	// assertions (Here using MACRO WARN_IF because the skybox is not essential for the game to work properly)
	WARN_IF(skyboxShaderProgram.locations.screenCoord == -1, "skyboxShaderProgram.locations.screenCoord == -1");
	WARN_IF(skyboxShaderProgram.locations.skyboxSampler == -1, "skyboxShaderProgram.locations.skyboxSampler == -1");
	WARN_IF(skyboxShaderProgram.locations.inversePVmatrix == -1, "skyboxShaderProgram.locations.inversePVmatrix == -1");

	skyboxShaderProgram.initialized = true;
	shaderList.clear();
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

void initSkybox() {
	GLint screenCoordLoc = glGetAttribLocation(skyboxShaderProgram.program, "screenCoord");
	CHECK_GL_ERROR();
	static const float screenCoords[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		1.0f,  1.0f
	};

	SkyboxGeometry = new ObjectGeometry;

	SkyboxGeometry->material.ambient = glm::vec3(0.4f);
	SkyboxGeometry->material.diffuse = glm::vec3(0.4f);
	SkyboxGeometry->material.specular = glm::vec3(0.4f);
	SkyboxGeometry->material.shininess = 32.0f;

	glGenVertexArrays(1, &SkyboxGeometry->vertexArrayObject);
	glBindVertexArray(SkyboxGeometry->vertexArrayObject);

	glGenBuffers(1, &(SkyboxGeometry->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, SkyboxGeometry->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(screenCoordLoc);
	glVertexAttribPointer(screenCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	CHECK_GL_ERROR();

	SkyboxGeometry->numTriangles = 2;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &SkyboxGeometry->material.texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxGeometry->material.texture);

	const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
	GLuint targets[] = {
	  GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for (int i = 0; i < 6; i++) {
		std::string texName = std::string(SKYBOX_PATH_NAME) + "/sh_" + suffixes[i] + ".jpg";
		std::cout << "Loading cube map texture: " << texName << std::endl;
		if (!pgr::loadTexImage2D(texName, targets[i])) {
			pgr::dieWithError("Skybox cube map loading failed!");
		}
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// unbind the texture (just in case someone will mess up with texture calls later)
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERROR();
}

void initModel(const std::string ModelName, std::vector<ObjectGeometry*> *ModelGeometries) {
	if (loadMeshes(ModelName, commonShaderProgram, *ModelGeometries) != true) {
		std::cerr << "\033[31minitModel : Cannot load : " << ModelName << "\033[0m" << std::endl;
	}
}

void initSceneObjects() {
	useLighting = true;
	initTerrain();
	initPlayer();
	initSkybox();
	initModel(FOXBAT_MODEL_NAME, &FoxBatGeometries);
	initModel(CAR_MODEL_NAME, &CarGeometries);
}

// -----------------------  Drawing ---------------------------------

/**
 * \brief Draw the objects in the scene.
 */

void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(commonShaderProgram.locations.PVM, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniformMatrix4fv(commonShaderProgram.locations.ViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(commonShaderProgram.locations.ModelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	const glm::mat3 modelRotationMatrix = glm::mat3(
		modelMatrix[0],
		modelMatrix[1],
		modelMatrix[2]
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat4(glm::mat3(modelRotationMatrix))));
	glUniformMatrix4fv(commonShaderProgram.locations.NormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));


	CHECK_GL_ERROR();
}

void setMaterialUniforms(const Material& material) {
	glUniform3fv(commonShaderProgram.locations.ambient, 1, glm::value_ptr(material.ambient));
	glUniform3fv(commonShaderProgram.locations.diffuse, 1, glm::value_ptr(material.diffuse));
	glUniform3fv(commonShaderProgram.locations.specular, 1, glm::value_ptr(material.specular));
	glUniform1f(commonShaderProgram.locations.shininess, material.shininess);
	CHECK_GL_ERROR();
	if (material.texture != 0) {
		glUniform1i(commonShaderProgram.locations.useTexture, 1);  // do texture sampling
		glUniform1i(commonShaderProgram.locations.texSampler, 0);  // texturing unit 0 -> samplerID   [for the GPU linker]
		glActiveTexture(GL_TEXTURE0 + 0);						   // texturing unit 0 -> to be bound [for OpenGL BindTexture]
		glBindTexture(GL_TEXTURE_2D, material.texture);
	}
	else {
		glUniform1i(commonShaderProgram.locations.useTexture, 0);  // do not sample the texture
	}
}

void drawPlayer(Player* Player, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (Player->isInitialized) {
		glUseProgram(commonShaderProgram.program);

		// prepare modeling transform matrix
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), Player->position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(Player->viewAngle), glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(Player->size, Player->size, Player->size));

		// send matrices to the vertex & fragment shader
		setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
		// set material uniforms
		setMaterialUniforms(PlayerGeometry->material);

		glBindVertexArray(PlayerGeometry->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, PlayerGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);


		glBindVertexArray(0);
		glUseProgram(0);
	}
	else {
		std::cerr << "Player not initialised" << std::endl;
	}
}

void drawTerrain(Terrain* Terrain, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (Terrain->isInitialized) {
		glUseProgram(commonShaderProgram.program);

		// prepare modeling transform matrix
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), Terrain->position);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(Terrain->size));

		// send matrices to the vertex & fragment shader
		setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
		TerrainGeometry->material.shininess = 30.0f;
		// set material uniforms
		setMaterialUniforms(TerrainGeometry->material);

		// draw geometry
		glBindVertexArray(TerrainGeometry->vertexArrayObject);
		glDrawElements(GL_TRIANGLES, TerrainGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);
	}
	else {
		std::cerr << "Terrain not initialised" << std::endl;
	}
}

void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(skyboxShaderProgram.program);
	glm::mat4 matrix = projectionMatrix * viewMatrix;

	// crate view rotation matrix by using view matrix with cleared translation (we must rotate by 90° on X-axis because we start on Top view)
	glm::mat4 viewRotation = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 inversePVmatrix = glm::inverse(projectionMatrix * viewRotation);
	glUniformMatrix4fv(skyboxShaderProgram.locations.inversePVmatrix, 1, GL_FALSE, glm::value_ptr(inversePVmatrix));
	glUniform1i(skyboxShaderProgram.locations.skyboxSampler, 0);

	glBindVertexArray(SkyboxGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxGeometry->material.texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);
}

void drawModel(Object* Model, std::vector<ObjectGeometry*> ModelGeometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	if (Model->isInitialized && !Model->destroyed) {
		glUseProgram(commonShaderProgram.program);

		// prepare modelling transform matrix
		glm::mat4 modelMatrix = alignObject(Model->position, (Model->direction), glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));

		modelMatrix = glm::scale(modelMatrix, glm::vec3(Model->size, Model->size, Model->size));

		// send matrices to the vertex & fragment shader
		setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
		for (size_t i = 0; i < ModelGeometry.size(); i++) {
			setMaterialUniforms(ModelGeometry[i]->material);

			// draw geometry
			glBindVertexArray(ModelGeometry[i]->vertexArrayObject);
			glDrawElements(GL_TRIANGLES, ModelGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
		glUseProgram(0);
	}
}

void drawObjects(GameObjectsList GameObjects, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
	drawTerrain(GameObjects.terrain, viewMatrix, projectionMatrix);
	drawPlayer(GameObjects.player, viewMatrix, projectionMatrix);
	drawModel(GameObjects.foxbat, FoxBatGeometries, viewMatrix, projectionMatrix);
	drawModel(GameObjects.car, CarGeometries, viewMatrix, projectionMatrix);
}


// -----------------------  Cleanup scene objects ----------------------------

void cleanupGeometry(ObjectGeometry* geometry) {
	glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
	glDeleteBuffers(1, &(geometry->elementBufferObject));
	glDeleteBuffers(1, &(geometry->vertexBufferObject));

}

void cleanupModels() {
	cleanupGeometry(PlayerGeometry);
	cleanupGeometry(TerrainGeometry);
	cleanupGeometry(SkyboxGeometry);
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
		// copy the material info to structure
		const aiMaterial* material = scn->mMaterials[mesh->mMaterialIndex];
		aiColor4D color;
		aiString name;
		aiReturn retValue = AI_SUCCESS;

		material->Get(AI_MATKEY_NAME, name);

		// diffiuse
		if ((retValue = aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

		geometry->material.diffuse = glm::vec3(color.r, color.g, color.b);

		// ambient
		if ((retValue = aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		geometry->material.ambient = glm::vec3(color.r, color.g, color.b);

		// specular
		if ((retValue = aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		geometry->material.specular = glm::vec3(color.r, color.g, color.b);

		// shininess
		ai_real shininess, strength;
		unsigned int max;	// changed: to unsigned

		max = 1;
		if ((retValue = aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
			shininess = 1.0f;
		max = 1;
		if ((retValue = aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
			strength = 1.0f;
		geometry->material.shininess = shininess * strength;


		geometry->material.texture = 0;
		// load texture image
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			// get texture name 
			aiString path; // filename

			aiReturn texFound = material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			std::string textureName = path.data;

			size_t found = fileName.find_last_of("/\\");
			// insert correct texture file path 
			if (found != std::string::npos) { // not found
				//subMesh_p->textureName.insert(0, "/");
				textureName.insert(0, fileName.substr(0, found + 1));
			}

			std::cout << "Loading texture file: " << textureName << std::endl;
			geometry->material.texture = pgr::createTexture(textureName);
		}
		CHECK_GL_ERROR();

		glGenVertexArrays(1, &(geometry->vertexArrayObject));
		glBindVertexArray(geometry->vertexArrayObject);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject); // bind our element array buffer (indices) to vao
		glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);

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

		geometry->numTriangles = mesh->mNumFaces;

		geometries.push_back(geometry);
	}

	return true;
}

bool loadSingleMesh(const std::string & fileName, ShaderProgram & shader, ObjectGeometry **geometry) {
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

	// copy the material info to structure
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
