#version 140

uniform mat4 PVM;     // Projection * View * Model --> model to clip coordinates
uniform float time;         // used for simulation of moving lights (such as sun)

in vec3 position;           // vertex position in world space
in vec2 texCoord;           // incoming texture coordinates

smooth out vec2 texCoord_v; // outgoing texture coordinates

float decay = 0.01;

void main() {
  gl_Position = PVM * vec4(position, 1.0);

  float localTime = time * decay;
  vec2 offset = vec2(1.0, 0.0) - time * vec2(1.0, 0.0);

  // outputs entering the fragment shader
  texCoord_v = texCoord + offset;
}
