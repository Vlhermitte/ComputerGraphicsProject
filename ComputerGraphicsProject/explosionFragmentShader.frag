#version 140

uniform float time;           // used to select proper animation frame
uniform sampler2D texSampler; // sampler for texture access

smooth in vec3 position_v;    // camera space fragment position
smooth in vec2 texCoord_v;    // fragment texture coordinates

// there are 8 frames in the row, two rows total
uniform ivec2 pattern = ivec2(8, 2);
// one frame lasts 0.1s
uniform float frameDuration = 0.1f;


vec4 sampleTexture(int frame) {
  vec2 subImage = texCoord_v / vec2(8.0, 2.0) + vec2(frame%8, frame/8) * vec2(1.0/8.0, 1.0/2.0);
  return texture(texSampler, subImage);
}

void main() {
  // frame of the texture to be used for explosion drawing 
  int frame = int(time / frameDuration);

  // sample proper frame of the texture to get a fragment color  
  gl_FragColor = sampleTexture(frame);
}
