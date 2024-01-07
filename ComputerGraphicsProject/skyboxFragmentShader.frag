#version 140

uniform samplerCube skyboxSampler;
in vec3 texCoord_v;

vec3 ambient = vec3(0.1f, 0.1f, 0.1f);

void main() {
	gl_FragColor = texture(skyboxSampler, texCoord_v);
}