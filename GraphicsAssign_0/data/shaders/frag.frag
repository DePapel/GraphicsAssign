#version 400

out vec4 FragColor;
in vec2 UV;
uniform sampler2D myTextureSampler;
uniform bool shaderSW;

//uniform int   uLightNum;
//uniform Light uLight[LIGHT_NUM_MAX];

void main()
{   
	if(!shaderSW)
		FragColor = texture(myTextureSampler, UV);
	else
		FragColor = vec4(UV, 0.0, 1.0);
}