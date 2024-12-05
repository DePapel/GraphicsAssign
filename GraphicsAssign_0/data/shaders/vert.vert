#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;

uniform mat4 model;
uniform mat4 camera;

out vec3 Position;
out vec2 UV;
out vec3 Nor;

void main()
{
   gl_Position = camera * model * vPosition;
   
   Position = (model * vPosition).xyz;
   UV = vTextCoords;
   Nor = vNormals;
}