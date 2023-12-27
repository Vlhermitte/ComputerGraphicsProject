#version 140

uniform samplerCube skyboxSampler;
in vec3 texCoord_v;
out vec4 color_f;

vec3 ambient = vec3(0.1f, 0.1f, 0.1f);

void main() {
	color_f = texture(skyboxSampler, texCoord_v);
}