#version 400

layout(location = 0) in vec4 vPosition;

uniform mat4 model;
uniform mat4 camera;

void main()
{
   gl_Position = camera * model * vPosition;
   
}