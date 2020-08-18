#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Color;

uniform mat4 model;
uniform vec3 color;

void main()
{
    gl_Position = model * vec4(aPos, 1.0);
	Color = color;
}