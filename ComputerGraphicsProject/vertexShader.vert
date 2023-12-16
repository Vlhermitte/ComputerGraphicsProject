#version 140

in vec3 position;
// in vec3 color;
uniform mat4 PVM;

// smooth out vec4 theColor;

void main()
{
	gl_Position = PVM * vec4(position, 1.0);
	// theColor = vec4(color, 1.0);
}
