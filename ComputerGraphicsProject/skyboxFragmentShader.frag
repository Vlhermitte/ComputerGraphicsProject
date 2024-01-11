#version 140

uniform samplerCube skyboxSampler;
in vec3 texCoord_v;

out vec4 fragColor;

vec3 ambient = vec3(0.1f, 0.1f, 0.1f);

void main() {
	fragColor = texture(skyboxSampler, texCoord_v);
}