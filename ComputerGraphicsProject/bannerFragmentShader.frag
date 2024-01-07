#version 140

uniform sampler2D texSampler;  // sampler for texture access

smooth in vec2 texCoord_v;     // incoming fragment texture coordinates

void main() {

  // fragment color is given only by the texture
  gl_FragColor = texture(texSampler, texCoord_v);
}
