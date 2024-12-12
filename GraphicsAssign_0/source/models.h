#pragma once

#include "CS300Parser.h"

struct Model
{
	CS300Parser::Transform transf;

	glm::mat4x4 ComputeMatrix();

	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UV;

	std::vector<float> vertices;

	std::vector<int> pointIndeces;
	std::vector<int> normalIndeces;

	std::vector<glm::vec3> comNormal;
	std::vector<glm::vec3> avgNormal;
	std::vector<int> avgCnt; //count

	//My Extra Code=========================
	unsigned int VBO;
	unsigned int VAO;

	unsigned int imageID;
	void CreateNormalMap();

	unsigned int norVBO;
	unsigned int norVAO;
	std::vector<glm::vec3> drawNormal;
	float rejustSize = 0.3f;
	static int slices;
	void CalculateNormals();
	
	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> bitangent;
	unsigned int tanVBO;
	unsigned int bitanVBO;
	void CalculateTangents();

	float t = 0.f;
	void ModelUpdate(float dt);
	//======================================
	void ClearAll();

	void LoadModel();
	void CreateModels();
	Model(const CS300Parser::Transform& _transform);

	~Model();
	
private:
	//TODO
	void CreateModelPlane();
	void CreateModelCube();
	void CreateModelCone(int slices);
	void CreateModelCylinder(int slices);
	void CreateModelSphere(int slices);
};

struct Light
{
	Model* m;
	std::string type;
	
	float ambient = 0.0f; 
	glm::vec3 diffuse; 
	glm::vec3 specular;
	glm::vec3 position;
	glm::vec3 atten;

	glm::vec3 direct;

	float inner = 0.0f;
	float outer = 0.0f;
	float falloff = 1.0f;

	float bias = 0.0f;
	unsigned int pcf = 0;

	float t = 0.f;
	void LightUpdate(float dt);
	Light(const CS300Parser::Light_info& _light);
	~Light();
	//  other variables 
	//  ...
};