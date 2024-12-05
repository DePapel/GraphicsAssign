#include "models.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

//#define TINYOBJLOADER_IMPLEMENTATION
#include "../extern/tiny_obj_loader.h"

#include "Level.h"
#include <iostream>

int Model::slices = 4;

void Model::CreateTextureData()
{
	const int width = 6;
	const int height = 6;

	glm::vec3 colors[] = 
	{
		glm::vec3(0, 0, 1),   // Blue
		glm::vec3(0, 1, 1),   // Cyan
		glm::vec3(0, 1, 0),   // Green
		glm::vec3(1, 1, 0),   // Yellow
		glm::vec3(1, 0, 0),   // Red
		glm::vec3(1, 0, 1)    // Purple
	};

	unsigned char* data = new unsigned char[width * height * 3 * 36];
	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			int colorIdx = (x + (height - 1 - y)) % 6;
			for (int py = 0; py < 6; py++) 
			{  // 세로 반복
				for (int px = 0; px < 6; px++) 
				{  // 가로 반복
					int idx = ((y * 6 + py) * width * 6 + (x * 6 + px)) * 3;
					data[idx] = colors[colorIdx].r * 255;
					data[idx + 1] = colors[colorIdx].g * 255;
					data[idx + 2] = colors[colorIdx].b * 255;
				}
			}
		}
	}

	//Texture 생성
	glGenTextures(1, &textureID);
	//TextureID에 바인딩
	glBindTexture(GL_TEXTURE_2D, textureID);
	//TextureData 삽입
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width * 6, height * 6, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//Data 삽입 glTexParameteri
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	delete[] data;
}

glm::mat4x4 Model::ComputeMatrix()
{
	glm::mat4 translate = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);

	glm::mat4 rotateYXZ = glm::mat4(1.0f);

	glm::mat4 temp = glm::mat4(1.0f);
	glm::mat4 a = glm::mat4(1.0f);
	glm::mat4 b = glm::mat4(1.0f);
	glm::mat4 c = glm::mat4(1.0f);

	translate = glm::translate(translate, transf.pos);
	scale = glm::scale(scale, transf.sca);

	a = glm::rotate(a, glm::radians(transf.rot.y), { 0,1,0 });
	b = glm::rotate(b, glm::radians(transf.rot.x), { 1,0,0 });
	c = glm::rotate(c, glm::radians(transf.rot.z), { 0,0,1 });

	rotateYXZ = a * b * c;

	temp = translate * rotateYXZ * scale;

	return temp;
}

void Model::LoadModel()
{
	ClearAll();

	//If exception. use one of our functions
	if (transf.mesh == "PLANE")
		CreateModelPlane();
	else if (transf.mesh == "CUBE")
		CreateModelCube();
	else if (transf.mesh == "SPHERE")
		CreateModelSphere(slices);
	else if (transf.mesh == "CYLINDER")
		CreateModelCylinder(slices);
	else if (transf.mesh == "CONE")
		CreateModelCone(slices);
	else
	{
		//Else, use the general
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, transf.mesh.c_str());

		std::vector<glm::vec3> temp;
		std::vector<glm::vec3> tempN;
		std::vector<glm::vec2> tempUV;

		//Save mesh points
		for (int i = 0; i < attrib.vertices.size(); i += 3)
		{
			temp.push_back({ attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2] });
		}

		//Save mesh normals
		for (int i = 0; i < attrib.normals.size(); i += 3)
		{
			tempN.push_back({ attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2] });
		}

		//Save UV
		for (int i = 0; i < attrib.texcoords.size(); i += 2)
		{
			tempUV.push_back({ attrib.texcoords[i], attrib.texcoords[i + 1]});
		}

		//Load Shapes (vertex indexes)
		for (auto s : shapes)
		{
			for (auto p : s.mesh.indices)
			{
				//Load vertexes
				points.push_back(temp[p.vertex_index]);
				//Load Normals
				comNormal.push_back(tempN[p.normal_index]);
				//Load Indexes
				UV.push_back(tempUV[p.texcoord_index]);
			}
		}
	}
}

void Model::CreateModels()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	//load points
	LoadModel();

	int s = points.size();
	//vertices
	for (int i = 0; i < s; i++)
	{
		//points
		vertices.push_back(points[i].x);
		vertices.push_back(points[i].y);
		vertices.push_back(points[i].z);
		//normals
		vertices.push_back(comNormal[i].x);
		vertices.push_back(comNormal[i].y);
		vertices.push_back(comNormal[i].z);
		//UV
		vertices.push_back(UV[i].x);
		vertices.push_back(UV[i].y);
	}

	//Sanity Check
	if (vertices.size() == 0)
		return;

	//Gen VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Gen VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(float)), &vertices[0], GL_STATIC_DRAW);

	//Assign Coordinates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Assign Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Assign UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	CreateTextureData();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Model::Model(const CS300Parser::Transform& _transform) : transf(_transform), VBO(0), VAO(0)
{
	CreateModels();
}

Model::~Model()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &norVBO);
	glDeleteVertexArrays(1, &norVAO);
}

void Model::CreateModelPlane()
{
	//Points coorinates
	points =
	{
	   {-0.5f, -0.5f, 0.0f},  // Bottom-left
	   {0.5f, -0.5f, 0.0f},   // Bottom-right
	   {0.5f, 0.5f, 0.0f},    // Top-right
	   {-0.5f, -0.5f, 0.0f},  // Bottom-left
	   {0.5f, 0.5f, 0.0f},    // Top-right
	   {-0.5f, 0.5f, 0.0f},    // Top-left
	};

	// UV coordinates
	UV =
	{
		{0.0f, 0.0f},  // Bottom-left
		{1.0f, 0.0f},  // Bottom-right
		{1.0f, 1.0f},  // Top-right
		{0.0f, 0.0f},  // Bottom-left
		{1.0f, 1.0f},  // Top-right
		{0.0f, 1.0f},   // Top-left
	};

	// Normals (pointing up for all vertices)
	comNormal =
	{
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}
	};

	for (int i = 0; i < comNormal.size(); i++)
		drawNormal.push_back(comNormal[i]);
}

void Model::CreateModelCube()
{
	//Points coordinates
	std::vector<glm::vec3> shapePoint =
	{
	   {-0.5f, -0.5f, 0.5f},  // Front-Bottom-left
	   {0.5f, -0.5f, 0.5f},   // Front-Bottom-right
	   {0.5f, 0.5f, 0.5f},    // Front-Top-right
	   {-0.5f, 0.5f, 0.5f},    // Front-Top-left
	   {-0.5f, -0.5f, -0.5f},  // Back-Bottom-left
	   {0.5f, -0.5f, -0.5f},   // Back-Bottom-right
	   {0.5f, 0.5f, -0.5f},    // Back-Top-right
	   {-0.5f, 0.5f, -0.5f},    // Back-Top-left
	};

	pointIndeces =
	{
		//Front
		0, 1, 2,
		0, 2, 3,
		//Back
		5, 4, 7,
		5, 7, 6,
		//Right
		1, 5, 2,
		6, 2, 5,
		//Left
		4, 0, 7,
		3, 7, 0,
		//Top
		2, 6, 7,
		2, 7, 3,
		//Btm
		0, 4, 5,
		0, 5, 1
	};

	//UV coordinates
	const std::vector<glm::vec2> basisUV_FB =
	{
		{0.0f, 0.0f},  // Bottom-left
		{1.0f, 0.0f},  // Bottom-right
		{1.0f, 1.0f},  // Top-right
		{0.0f, 0.0f},  // Bottom-left
		{1.0f, 1.0f},  // Top-right
		{0.0f, 1.0f},   // Top-left
	};
	const std::vector<glm::vec2> basisUV_RL =
	{
		{0.0f, 0.0f},  // Bottom-left
		{1.0f, 0.0f},  // Bottom-right
		{0.0f, 1.0f},   // Top-left
		{1.0f, 1.0f},  // Top-right
		{0.0f, 1.0f},   // Top-left
		{1.0f, 0.0f},  // Bottom-right
	};

	for (int i = 0; i < basisUV_FB.size(); i++)
	{
		if (i == 2 || i == 3) //Right, Left 일 경우에만 반대로
			UV.insert(UV.end(), basisUV_RL.begin(), basisUV_RL.end());
		else
			UV.insert(UV.end(), basisUV_FB.begin(), basisUV_FB.end());
	}

	//Use to calculate Normal vector's average=====================
	avgNormal.resize(shapePoint.size());
	avgCnt.resize(shapePoint.size());
	//=============================================================
	//Common Normal vector
	glm::vec3 a, b, nor;
	for (int i = 0; i < pointIndeces.size(); i+=3)
	{
		points.push_back(shapePoint[pointIndeces[i]]);
		points.push_back(shapePoint[pointIndeces[i + 1]]);
		points.push_back(shapePoint[pointIndeces[i + 2]]);
		
		//CrossProduct Error!!!
		a = shapePoint[pointIndeces[i]] - shapePoint[pointIndeces[i + 1]];
		b = shapePoint[pointIndeces[i]] - shapePoint[pointIndeces[i + 2]];
		nor = glm::cross(a, b);
		nor = glm::normalize(nor);
		//regular normals
		comNormal.push_back(nor);
		comNormal.push_back(nor);
		comNormal.push_back(nor);

		//TODO: Make Average normals system
		avgNormal[pointIndeces[i]] += nor;
		avgNormal[pointIndeces[i + 1]] += nor;
		avgNormal[pointIndeces[i + 2]] += nor;
		//increase avgCnt
		avgCnt[pointIndeces[i]] += 1;
		avgCnt[pointIndeces[i + 1]] += 1;
		avgCnt[pointIndeces[i + 2]] += 1;
	}

	CalculateNormals();

}

void Model::CreateModelCone(int slices)
{
	float angle = 360.f / slices;
	angle = glm::radians(angle);
	glm::vec3 topPoint = { 0.0f, 0.5f, 0.0f };
	glm::vec3 btmPoint = { 0.0f, -0.5f, 0.0f };
	glm::vec3 pointA, pointB, vecA, vecB, nor;

	//avgNormal.resize(slices + 1); //slices와 TopPoint의 1개를 더한 크기로 사이즈 재 조절

	//Wall
	for (int i = 0; i < slices; i++)
	{
		points.push_back(topPoint);
		UV.push_back({ (i + 0.5f) / (float)slices, 1.0f });

		pointA = { 0.5 * cos((i + 1) * angle), -0.5, 0.5 * sin((i + 1) * angle) };
		points.push_back(pointA);
		UV.push_back({ (i + 1) / (float)slices, 0.0f });
		
		pointB = { 0.5 * cos(i * angle), -0.5, 0.5 * sin(i * angle) };
		points.push_back(pointB);
		UV.push_back({ i / (float)slices, 0.0f });

		vecA = pointB - topPoint;
		vecB = topPoint - pointA;
		nor = glm::cross(vecA, vecB);
		comNormal.push_back(nor);
		comNormal.push_back(nor);
		comNormal.push_back(nor);
	}
	//Btm
	for (int i = 0; i < slices; i++)
	{
		points.push_back(btmPoint);
		UV.push_back({ (i + 0.5f) / (float)slices,0.0f });

		pointA = { 0.5 * cos(i * angle), -0.5, 0.5 * sin(i * angle) };
		points.push_back(pointA);
		UV.push_back({ i / (float)slices, 0.0f });

		pointB = { 0.5 * cos((i + 1) * angle), -0.5, 0.5 * sin((i + 1) * angle) };
		points.push_back(pointB);
		UV.push_back({ (i + 1) / (float)slices, 0.0f });

		vecA = btmPoint - pointA;
		vecB = pointA - pointB;
		nor = glm::cross(vecA, vecB);
		comNormal.push_back(nor);
		comNormal.push_back(nor);
		comNormal.push_back(nor);
	}

	//In Future
	//normal 벡터 slices 증가할때마다 줄어드는 현상 고치기
	//AVGnormal vec 구하기
	CalculateNormals();
}

void Model::CreateModelCylinder(int slices)
{
	//TODO: Points

	//TODO: UVs

	//TODO: Normals
}

void Model::CreateModelSphere(int slices)
{
	//TODO: Points

	//TODO: UVs

	//TODO: Normals
}

void Model::CalculateNormals()
{
	//Normals coordinates
	for (int i = 0; i < avgNormal.size(); i++)
	{
		avgNormal[i] /= avgCnt[i];
		avgNormal[i] = glm::normalize(avgNormal[i]);
	}

	normals.clear();
	drawNormal.clear();
	if (!Level::GetPtr()->comavgSw)
	{
		for (int i = 0; i < comNormal.size(); i++)
		{
			normals.push_back(comNormal[i]);
			drawNormal.push_back(points[i]);
			drawNormal.push_back(points[i] + (normals[i] * rejustSize));
		}
	}
	else
	{
		for (int i = 0; i < pointIndeces.size(); i++)
		{
			normals.push_back(avgNormal[pointIndeces[i]]);
			drawNormal.push_back(points[i]);
			drawNormal.push_back(points[i] + (normals[i] * rejustSize));
		}
	}

	if (normals.empty())
		return;

	// VAO와 VBO 삭제
	glDeleteBuffers(1, &norVBO);
	glDeleteVertexArrays(1, &norVAO);
	
	glGenVertexArrays(1, &norVAO);
	glBindVertexArray(norVAO);
	
	glGenBuffers(1, &norVBO);
	glBindBuffer(GL_ARRAY_BUFFER, norVBO);
	glBufferData(GL_ARRAY_BUFFER, drawNormal.size() * (sizeof(float) * 3), &drawNormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void Model::ClearAll()
{
	points.clear();
	normals.clear();
	comNormal.clear();
	avgNormal.clear();
	drawNormal.clear();
	avgCnt.clear();
	pointIndeces.clear();
	normalIndeces.clear();
	UV.clear();
	vertices.clear();
}

Light::Light(const CS300Parser::Light_info &_light)
{
	CS300Parser::Transform T;

	//Create Light's Model
	T.name = "Light";
	T.mesh = "CUBE";
	//T.normalMap = default
	T.pos = _light.pos;
	T.rot = glm::vec3(1.0f, 1.0f, 1.0f);
	T.sca = glm::vec3(5.0f, 5.0f, 5.0f);
	//ns        = default
	//ior       = default
	//reflector = default
	m = new Model(T);

	//Input variable from _light
	ambient  = _light.amb;
	diffuse  = _light.col;
	specular = _light.col;
	position = _light.pos;

	atten = _light.att;
}

Light::~Light()
{
	delete m;
}
