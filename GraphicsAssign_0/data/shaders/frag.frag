#version 400

out vec4 FragColor;

in vec3 Position;	//Material Position
in vec2 UV;			//Material UV
in vec3 Nor; 		//Material Normal (N)

in mat3 FragmodelM;
in mat3 TBN;
in vec3 tan;
in vec3 bitan;
in mat3 cameraM;

uniform sampler2D myTextureSampler;
uniform sampler2D normalTexture;

uniform bool shaderSW;
uniform bool lineSW;
uniform bool lightSW;

struct Light
{
	int type;
	float ambient;
    vec3 diffuse;
    vec3 specular;
	vec3 positionWorld;
	
	vec3 atten;
	vec3 dir;
	
	float inner;
	float outer;
	float falloff;
};

uniform int uLightNum;
uniform Light uLight[5];

//Material Property
uniform float Mshininess; 	//Material shininess

//Camera Position
uniform vec3 CameraPosition;

void main()
{   
	//Material Property
	float Mambient; 	//(Ka)
	Mambient = 1.0;
	vec3 Mdiffuse;  						//texture color
	vec3 Mspecular = vec3(1.0, 1.0, 1.0); 	//always White
	
	//Calculate ViewDir Vector
	vec3 viewDir = normalize(CameraPosition - Position);
	
	//Result Calculated Color
	vec3 resultColor = vec3(0.0);
	
	vec3 NorTemp;
	for(int i=0; i<uLightNum; i++)
	{
		//Light Property
		float light_ambient = uLight[i].ambient; //(Ia)
		vec3 light_diffuse = uLight[i].diffuse; //(Id)
		vec3 light_specular = uLight[i].specular; //(Is)
		
		vec3 lPos = uLight[i].positionWorld;
		NorTemp = normalize(Nor);
		
		//Calculate Light Direction
		vec3 lightDir;
		if(uLight[i].type == 0 || uLight[i].type == 2) //POINT and SPOT
			lightDir = normalize(lPos - Position); // (L)
		else if(uLight[i].type == 1) //DIR
			lightDir = -uLight[i].dir; // (L)
			
		//***Ambient***
		vec3 col = vec4(UV, 0.0, 1.0).xyz;
		if(!shaderSW)
		{
			col = texture(myTextureSampler, UV).xyz;
			NorTemp = normalize(2.0 * texture(normalTexture, UV).xyz - 1.0);
			
			//Convert nomal from tangent space (NMap) to World
			NorTemp = normalize(FragmodelM * TBN * NorTemp); 
		}
		vec3 ambient = light_ambient * Mambient * col;
		Mdiffuse = col;
		
		//***Diffuse***
		float diff = max(dot(NorTemp, lightDir), 0.0);
		vec3 diffuse = light_diffuse * Mdiffuse * diff;
		
		//***Specular***
		vec3 reflectionVec = 2.0 * (dot(NorTemp, lightDir))* NorTemp - lightDir;
		float spec = pow(max(dot(reflectionVec, viewDir), 0.0), Mshininess);
		vec3 specular = light_specular * Mspecular * spec;
	
		//Calculate Attenuation
		float dis = distance(Position, lPos);
		float att = min(1.0/(uLight[i].atten.x + uLight[i].atten.y*dis + uLight[i].atten.z*dis*dis), 1);
	
		if(uLight[i].type == 2) //SPOT
		{
			vec3 dirSpot = normalize(uLight[i].dir);
			float LdotD = dot(lightDir, dirSpot);
			float effectAngle = LdotD / (length(lightDir) * length(dirSpot));
			
			float SpotLightEffect = pow(effectAngle - cos(uLight[i].outer) / cos(uLight[i].inner) - cos(uLight[i].outer), uLight[i].falloff);
		
			SpotLightEffect = clamp(SpotLightEffect, 0.0, 1.0);
			
			resultColor += ambient + SpotLightEffect*(diffuse + specular);
		}
		else
		{
			//ambient = vec3(0,0,0);
			//diffuse = vec3(0,0,0);
			//specular = vec3(0,0,0);
			resultColor += ambient + att*(diffuse + specular);
		}
	}
	
	FragColor = vec4(resultColor, 1.0);
	
	if(lineSW || lightSW)
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		
	//FragColor = vec4((NorTemp + vec3(1,1,1) / 2.0) , 1.0);
	//FragColor = vec4((tan + vec3(1,1,1) / 2.0) , 1.0);
	
	//FragColor = texture(myTextureSampler,UV);
	//FragColor = texture(normalTexture,UV);
	
}