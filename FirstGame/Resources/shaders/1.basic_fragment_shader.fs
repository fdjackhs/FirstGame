#version 330 core

in vec3 colorPos;

out vec4 FragColor;

void main()
{
	FragColor = vec4(colorPos.x, colorPos.y, colorPos.z, 1.0f);
}