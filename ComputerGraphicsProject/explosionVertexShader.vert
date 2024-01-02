#version 140

uniform mat4 PVM;     // Projection * View * Model --> model to clip coordinates

in vec3 position;           // vertex position in world space
in vec2 texCoord;           // incoming texture coordinates

smooth out vec2 texCoord_v; // outgoing vertex texture coordinates

void main() {
	// Transform vertex into clip coordinates
	gl_Position = PVM * vec4(position, 1); 

	texCoord_v = texCoord;
}
