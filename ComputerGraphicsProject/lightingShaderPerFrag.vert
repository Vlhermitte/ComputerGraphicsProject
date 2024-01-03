#version 140

// Inputs
in vec3 position;
in vec3 normal;
in vec2 texCoord;

// Uniforms
uniform mat4 PVM;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;

// Outputs to fragment shader
smooth out vec3 fragPosition;
smooth out vec3 fragNormal;
smooth out vec2 fragTexCoord;


void main() {
	// Calculate the position of the vertex in eye coordinates for the fragment shader
    fragPosition = vec3(ViewMatrix * ModelMatrix * vec4(position, 1.0));
    
    // Calculate the normal for the vertex in eye coordinates
    fragNormal = normalize(vec3(NormalMatrix * vec4(normal, 0.0)));

    // Pass through the texture coordinates
    fragTexCoord = texCoord;

    // Calculate the position of the vertex for rasterization
    gl_Position = PVM * vec4(position, 1.0);
}