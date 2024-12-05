#version 400

out vec4 FragColor;

in vec3 Position;	//Material Position
in vec2 UV;			//Material UV
in vec3 Nor; 		//Material Normal (N)

uniform sampler2D myTextureSampler;
uniform bool shaderSW;
uniform bool lineSW;
uniform bool lightSW;

uniform mat4 camera;
uniform int uLightNum;

struct Light
{
	float ambient;
    vec3 diffuse;
    vec3 specular;
	vec3 positionWorld;
};
uniform Light uLight[1];

//Material Property
uniform float Mshininess; 	//Material shininess

//Camera Position
uniform vec3 CameraPosition;

//Attenuation
uniform vec3 atten;

void main()
{   
	//Light Property
	float light_ambient = uLight[0].ambient; //(Ia)
	vec3 light_diffuse = uLight[0].diffuse; //(Id)
	vec3 light_specular = uLight[0].specular; //(Is)
	
	vec3 lPos = uLight[0].positionWorld;
	
	//Material Property
	float Mambient; 	//(Ka)
	Mambient = 1.0;
	vec3 Mdiffuse;  						//texture color
	vec3 Mspecular = vec3(1.0, 1.0, 1.0); 	//always White
	
	//Calculate ViewDir Vector
	vec3 viewDir = normalize(CameraPosition - Position);
	
	//Result Calculated Color
	vec3 resultColor = vec3(0.0);
	
	
	//Start loop===================================
	//Calculate Light Direction
	vec3 lightDir = normalize(lPos - Position); // (L)
	
	//***Ambient***
	
	vec3 col = vec4(UV, 0.0, 1.0).xyz;
	
	if(!shaderSW)
	{
		col = texture(myTextureSampler, UV).xyz;
	}
	
	
	vec3 ambient = light_ambient * Mambient * col;
	
	Mdiffuse = col;
	
	//***Diffuse***
	float diff = 1;//max(dot(Nor, lightDir), 0.0);
	vec3 diffuse = light_diffuse * Mdiffuse * diff;
	
	//***Specular***
	vec3 reflectionVec = 2.0 * (dot(Nor, lightDir))* Nor - lightDir;
	float spec = pow(max(dot(reflectionVec, viewDir), 0.0), Mshininess);
	vec3 specular = light_specular * Mspecular * spec;

	
	//Calculate Attenuation
	float dis = distance(Position, lPos);
	float att = min(1.0/(atten.x + atten.y*dis + atten.z*dis*dis), 1);


	resultColor += ambient + att*(diffuse + specular);
	//End loop======================================================

	FragColor = vec4(resultColor, 1.0);
	
	if(lineSW || lightSW)
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}