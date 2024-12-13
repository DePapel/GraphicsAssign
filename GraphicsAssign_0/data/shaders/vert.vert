#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
uniform mat4 model;
uniform mat4 camera;

uniform bool only;

uniform mat4 Lv;
uniform mat4 Lp;
uniform mat4 Ls;

out vec3 Position;
out vec2 UV;
out vec3 Nor;

out mat3 FragmodelM;
out mat3 TBN;

out vec3 tan;
out vec3 bitan;
out mat3 cameraM;

out vec3 Vl;
out vec2 TC;

void main()
{
	UV = vTextCoords;
	if(only)
	{
		gl_Position = vPosition * 2;
		return;
	}
   gl_Position = camera * model * vPosition;
   
   Position = (model * vPosition).xyz;
  
   //Nor = normalize((model * vec4(vNormals, 0.0)).xyz);
   Nor = cross(tangent, bitangent);
   FragmodelM = mat3(model);
   
   tan = normalize(tangent);
   bitan = normalize(bitangent);
   
   TBN = transpose(mat3(tan, bitan, Nor));
   
   cameraM = mat3(camera);
   
   Vl = (Lv * (model * vPosition)).xyz;
   TC = (Ls * Lp * vec4(Vl, 1)).xy;
}