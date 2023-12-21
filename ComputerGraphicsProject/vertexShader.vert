#version 140

// Struct
struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection

  bool  useTexture;    // defines whether the texture is used or not
};

// Attributes
in vec3 position;           // vertex position in world space
in vec3 normal;             // vertex normal
in vec2 texCoord;           // incoming texture coordinates

// Uniforms
uniform mat4 PVM;
uniform Material material;  // current material

// Outgoing attributes to Fragment shader
smooth out vec2 texCoord_v;  // outgoing texture coordinates
smooth out vec4 color_v;     // outgoing fragment color

void main() {
	gl_Position = PVM * vec4(position, 1.0);

	// vec3 vertexPosition = (Vmatrix * Mmatrix * vec4(position, 1.0)).xyz;
    // vec3 vertexNormal   = normalize( (Vmatrix * normalMatrix * vec4(normal, 0.0) ).xyz);

	vec3 globalAmbientLight = vec3(0.4f);
	vec4 outputColor = vec4(material.ambient * globalAmbientLight, 0.0);


	color_v = outputColor;
	texCoord_v = texCoord;
}
