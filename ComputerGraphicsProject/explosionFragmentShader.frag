#version 140

uniform float time;           // used to select proper animation frame
uniform sampler2D texSampler; // sampler for texture access

smooth in vec3 position_v;    // camera space fragment position
smooth in vec2 texCoord_v;    // fragment texture coordinates

// there are 8 frames in the row, two rows total
uniform ivec2 pattern = ivec2(4, 4);
// one frame lasts 0.05s
uniform float frameDuration = 0.05f;

out vec4 fragColor;           // fragment color


vec4 sampleTexture(int frame) {
	// Assuming texCoord_v is the normalized texture coordinates [0,1] passed from the vertex shader
	vec2 grid = vec2(4.0, -4.0); // Assuming a 4x4 grid of sub-images/frames
	vec2 subImage = vec2(1.0 / grid.x, 1.0 / grid.y); // Calculate the size of a sub-image/frame
	vec2 subImageCoord = vec2(mod(float(frame), grid.x), floor(float(frame) / grid.x)); // Calculate the sub-image/frame coordinate

	subImageCoord *= subImage; // Calculate the UV coordinate of the sub-image/frame
	subImageCoord += texCoord_v * subImage; // Calculate the UV coordinate of the pixel within the sub-image/frame

	return texture(texSampler, subImageCoord); // Sample the texture
}


void main() {
  // frame of the texture to be used for explosion drawing 
  int frame = int(time / frameDuration);

  // sample proper frame of the texture to get a fragment color  
  fragColor = sampleTexture(frame);
}
