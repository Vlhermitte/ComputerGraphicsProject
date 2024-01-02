#version 140

// Struct Material
struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection

  bool  useTexture;    // defines whether the texture is used or not
};

// Struct Light
struct Light {         // structure describing light parameters
  vec3  ambient;       // intensity & color of the ambient component
  vec3  diffuse;       // intensity & color of the diffuse component
  vec3  specular;      // intensity & color of the specular component
  vec3  position;      // light position
  vec3  spotDirection; // spotlight direction
  float spotCosCutOff; // cosine of the spotlight's half angle
  float spotExponent;  // distribution of the light energy within the reflector's cone (center->cone's edge)
};

// Attributes
in vec3 position;           // vertex position in world space
in vec3 normal;             // vertex normal
in vec2 texCoord;           // incoming texture coordinates

// Uniforms
uniform mat4 PVM;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;

uniform Material material;  // current material

uniform bool turnSunOn;
uniform bool useSpotLight;
uniform vec3 spotLightPosition;
uniform vec3 spotLightDirection;

uniform float time;         // time used for simulation of moving lights (such as sun)

// Outgoing attributes to Fragment shader
smooth out vec2 texCoord_v;  // outgoing texture coordinates
smooth out vec4 color_v;     // outgoing fragment color
out vec4 positionRelativeToCamera_v;

Light sun;
float sunSpeed = 0.25f;		// sun speed to simulate night and day cycle
Light playerLight;

void SetupLight() {
	// Light parameters
	// sun.position = vec3(1.0f, 1.0f, 1.0f);

	sun.ambient  = vec3(0.2f, 0.2f, 0.2f);
	sun.diffuse  = vec3(0.8f, 0.8f, 0.8f);
	sun.specular = vec3(1.0f, 1.0f, 1.0f);
	sun.position = (ViewMatrix * vec4(cos(time * sunSpeed), 0.0f, sin(time * sunSpeed), 0.0f)).xyz;
	sun.spotDirection = normalize((ViewMatrix * vec4(spotLightDirection, 0.0)).xyz);

	playerLight.ambient = vec3(0.2f);
	playerLight.diffuse = vec3(1.0);
	playerLight.specular = vec3(1.0);
	playerLight.spotCosCutOff = 0.95f;
	playerLight.spotExponent = 0.0;
	playerLight.position = (ViewMatrix * vec4(spotLightPosition, 1.0)).xyz;
	playerLight.spotDirection = normalize((ViewMatrix * vec4(spotLightDirection, 0.0)).xyz);
}

vec4 directionalLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {

  vec3 ret = vec3(0.0);
  vec3 ambient = light.ambient * material.ambient;

  float cosAlpha = dot(vertexNormal, light.position);

  vec3 diffuse = max(cosAlpha, 0) * light.diffuse * material.diffuse;
  vec3 reflectVector = reflect(-light.position, vertexNormal);

  float cosBeta = dot(reflectVector, vec3(0.0, 0.0, 1.0));

  vec3 specular = pow(max(cosBeta, 0), material.shininess) * light.specular * material.specular;

  ret = ambient + diffuse + specular;
  return vec4(ret, 1.0);
}

vec4 spotLight(Light spotLight, Material material, vec3 vertexPosition, vec3 vertexNormal) {

	vec3 ret = vec3(0.0);

	vec3 L = normalize(spotLight.position - vertexPosition);
	vec3 R = reflect(-L, vertexNormal);
	vec3 V = normalize(-vertexPosition);
	float NdotL = max(0.0, dot(vertexNormal, L));
	float RdotV = max(0.0, dot(R, V));
	float spotCoef = max(0.0, dot(-L, spotLight.spotDirection));

	ret += material.ambient * spotLight.ambient;
	ret += material.diffuse * spotLight.diffuse * NdotL;
	ret += material.specular * spotLight.specular * pow(RdotV, material.shininess);

	if (spotCoef >= spotLight.spotCosCutOff) {
		ret *= pow(spotCoef, spotLight.spotExponent);
	} else {
		ret = vec3(0.0);
	}

	return vec4(ret, 1.0);
}


void main() {

	// First we need to setup the light
	SetupLight();

	// eye-coordinates position and normal of vertex
	vec3 vertexPosition = (ViewMatrix * ModelMatrix * vec4(position, 1.0)).xyz;         // vertex in eye coordinates
	vec3 vertexNormal = normalize((ViewMatrix * NormalMatrix * vec4(normal, 0.0)).xyz);   // normal in eye coordinates by NormalMatrix

	// initialize the output color with the global ambient term
	vec3 globalAmbientLight = vec3(0.5f);
	vec4 outputColor = vec4(material.ambient * globalAmbientLight, 0.0);

	// accumulate contributions from all lights
	if (turnSunOn)
		outputColor += directionalLight(sun, material, vertexPosition, vertexNormal);
	if (useSpotLight)
		outputColor += spotLight(playerLight, material, vertexPosition, vertexNormal);

	// For fog
	vec4 positionRelativeToCamera = ViewMatrix * ModelMatrix * vec4(position, 1.0);

	gl_Position = PVM * vec4(position, 1.0);
	color_v = outputColor;
	texCoord_v = texCoord;
	positionRelativeToCamera_v = positionRelativeToCamera;
}
