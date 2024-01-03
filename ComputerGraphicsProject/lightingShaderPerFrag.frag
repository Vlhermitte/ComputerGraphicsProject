#version 140

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

// Struct Material
struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection
  bool  useTexture;    // defines whether the texture is used or not
};

uniform Light light;
uniform Material material;
uniform sampler2D fragTexSampler;  // sampler for the texture access

// Uniforms
uniform mat4 PVM;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;

uniform float time; // Time since the beginning of the program
uniform bool fogOn;

uniform bool turnSunOn;
uniform bool useSpotLight;
uniform vec3 spotLightPosition;
uniform vec3 spotLightDirection;
uniform vec3 viewPosition; // Position of the camera/view

// Inputs from the vertex shader
smooth in vec3 fragPosition;
smooth in vec3 fragNormal;
smooth in vec2 fragTexCoord;


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

	vec3 L = normalize(light.position - vertexPosition); // Light direction
	vec3 V = normalize(-vertexPosition); // View direction
	vec3 H = normalize(L + V); // Halfway vector between light and view directions
	float NdotL = max(0.0, dot(vertexNormal, L));
	float NdotH = max(0.0, dot(vertexNormal, H)); // Dot product of normal and halfway vector

	ret += material.ambient * light.ambient;
	ret += material.diffuse * light.diffuse * NdotL;
	ret += material.specular * light.specular * pow(NdotH, material.shininess); // Blinn-Phong specular term

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

float computeVisbility(float distToCam) {
	float fogNear = 0.0f;
	float fogFar = 1.0f;
	float visibility = smoothstep(fogNear, fogFar, distToCam);
	return visibility;
}

void main() {
	// First we need to setup the light
	SetupLight();

	// initialize the output color with the global ambient term
	vec3 globalAmbientLight = vec3(0.5f);
	vec4 outputColor = vec4(material.ambient * globalAmbientLight, 0.0);

	// accumulate contributions from all lights 
	if (turnSunOn)
		outputColor += directionalLight(sun, material, fragPosition, fragNormal);
	if (useSpotLight)
		outputColor += spotLight(playerLight, material, fragPosition, fragNormal);

	// apply texture if it is on
	if(material.useTexture)
        outputColor = outputColor * texture(fragTexSampler, fragTexCoord);

	// apply fog if it is on
    if(fogOn) {
        vec4 fogcolor = vec4(0.4, 0.4, 0.4, 1);
        float distToCam = -fragPosition.z;
        float visibility = computeVisbility(distToCam);
        outputColor = mix(outputColor, fogcolor, visibility);
    }

	gl_FragColor = outputColor;
}