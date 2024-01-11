#version 140

uniform sampler2D texSampler;  // sampler for texture access

smooth in vec2 texCoord_v;     // incoming fragment texture coordinates

out vec4 fragColor;            // outgoing fragment color to framebuffer

void main() {

  // fragment color is given only by the texture
  fragColor = texture(texSampler, texCoord_v);
}
