#version 330 core
out vec4 FragColor;

in vec3 Color;

void main()
{
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    FragColor = vec4(Color.x, Color.y, Color.z, 1.0);
}