#version 330 core
layout (location = 0) in vec3 Pos;

out vec3 colorPos;

void main()
{
	colorPos = vec3(Pos.x, Pos.y, Pos.z);
	gl_Position = vec4(Pos.x, Pos.y, Pos.z, 1.0);
}