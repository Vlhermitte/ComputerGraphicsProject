#version 140

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection

  bool  useTexture;         // defines whether the texture is used or not
};

uniform sampler2D texSampler;  // sampler for the texture access

uniform Material material;     // current material

uniform bool fogOn;            // fog uniform

smooth in vec4 positionRelativeToCamera_v;    // incoming fragment position (from vertex shader)

smooth in vec4 color_v;        // incoming fragment color (includes lighting)
smooth in vec2 texCoord_v;     // fragment texture coordinates

out vec4 color_f;        // outgoing fragment color


float computeVisbility(float distToCam) {
	float fogNear = 0.0f;
	float fogFar = 1.0f;
	float visibility = smoothstep(fogNear, fogFar, distToCam);
	return visibility;
}

void main() {    
    color_f = color_v;

    // apply material if it has a texture
    if(material.useTexture)
        color_f =  color_v * texture(texSampler, texCoord_v);

    // apply fog if it is on
    if(fogOn) {
        vec4 fogcolor = vec4(0.4, 0.4, 0.4, 1);
        float distToCam = -positionRelativeToCamera_v.z;
        float visibility = computeVisbility(distToCam);
        color_f = mix(color_f, fogcolor, visibility);
    }


}