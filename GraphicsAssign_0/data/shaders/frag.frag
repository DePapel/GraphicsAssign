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

in vec4 Vl;
//in vec2 TC;
uniform mat4 Lp;
uniform mat4 Ls;
in vec4 NDC;

uniform sampler2D myTextureSampler;
uniform sampler2D normalTexture;
uniform sampler2D ShadowMap;

uniform int shaderSW;
uniform bool lineSW;
uniform bool lightSW;
uniform bool only;

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
	
	float bias;
};

uniform int uLightNum;
uniform Light uLight[5];

//Material Property
uniform float Mshininess; 	//Material shininess

//Camera Position
uniform vec3 CameraPosition;

void main()
{   
	if(only)
	{
		FragColor = texture(ShadowMap, UV);
		return;
	}
	
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
		if(uLight[i].type == 0) //POINT
			lightDir = normalize(lPos - Position); // (L)
		else if(uLight[i].type == 1  || uLight[i].type == 2) //DIR
			lightDir = uLight[i].dir; // (L)
			
		//***Ambient***
		vec3 col = vec4(UV, 0.0, 1.0).xyz;
		if(shaderSW == 1)
		{
			col = texture(myTextureSampler, UV).xyz;
			NorTemp = normalize(2.0 * texture(normalTexture, UV).xyz - 1.0);
			
			//Convert nomal from tangent space (NMap) to World
			NorTemp = normalize(FragmodelM * TBN * NorTemp); 
		}
		else if(shaderSW == 2)
		{
			FragColor = vec4((tan + vec3(1,1,1)) / 2.0, 1.0);
			return;
		}
		else if(shaderSW == 3)
		{
			FragColor = vec4((bitan + vec3(1,1,1)) / 2.0 , 1.0);
			return;
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
	
		//shadow mapping===================================
		//Vl을 NDC좌표로 바꾸고 난 후 D(=TC를 이용하여 ShadowMap에서 추출한 값)와 비교
		
		//Vl
		vec4 newV = Lp * Vl; //World-to-Light's View
		newV = vec4(newV.x/newV.w, newV.y/newV.w, newV.z/newV.w, 1); //Light's View-to-NDC
		newV = (newV + vec4(1,1,1,1)) / 2.0; //NDC Mapping
		
		//TC
		vec2 TC = newV.xy;
		
		
		//D is color in ShadowMap
		float D = texture(ShadowMap, TC).x;
		
		//Test========================
		//vec4 newNDC = NDC / NDC.w;
		//newNDC = (newNDC + vec4(1,1,1,1)) / 2.0;
		//FragColor = vec4(newNDC.xy,0, 1);
		//FragColor = texture(ShadowMap, TC);
		//return;
		//==============================
		
		
		float shadow = 1.0;
		if(newV.z - uLight[i].bias > D) 
		{
			shadow = 0.0;
		}
		
		//=================================================
	
		if(uLight[i].type == 2) //SPOT
		{
			vec3 dirSpot = normalize(lightDir);
			vec3 lightTOTarget = normalize(Position - lPos);
			float LdotD = dot(lightTOTarget, dirSpot);
			float effectAngle = LdotD / (length(lightTOTarget) * length(dirSpot));
			
			float Phi = cos(radians(uLight[i].outer));
            float Theta = cos(radians(uLight[i].inner));
			float SpotLightEffect = pow((effectAngle - Phi) / (Theta - Phi), uLight[i].falloff);
			SpotLightEffect = clamp(SpotLightEffect, 0.0, 1.0);
			
			resultColor += ambient + SpotLightEffect * shadow *(diffuse + specular);
		}
		else
		{
			resultColor += ambient + att * shadow *(diffuse + specular);
		}
	}
	
	FragColor = vec4(resultColor, 1.0);
	
	if(lineSW || lightSW)
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);	
}