#pragma once

#include "CS300Parser.h"

struct Model
{
	CS300Parser::Transform transf;

	//TODO
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
	unsigned int textureID;

	unsigned int norVBO;
	unsigned int norVAO;
	std::vector<glm::vec3> drawNormal;
	float rejustSize = 0.5f;
	
	void CreateTextureData();
	static int slices;
	void CreateModels();
	void CalculateNormals();
	//======================================
	void LoadModel();

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