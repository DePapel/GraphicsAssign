#pragma once

#define W_WIDTH 1280.0
#define W_HEIGHT 720.0

class GLFWwindow;
class Model;

#include <vector>
#include "program.h"

//Singleton level
struct Level
{
	int Initialize();
	void Run();
	void Cleanup();

	static Level* GetPtr();
	static void DeletePtr();

	void ReloadShaderProgram();

	void KeyInput();
	void Joom(float val);
	void RotateCamX(float angle);
	void RotateCamY(float angle);

	bool keyStateW = false, keyStateS = false, keyStateA = false, keyStateD = false;
	bool normalVecSw, shaderSW;
private:
	void Render(Model*);
	Level();
	~Level();
	Level(const Level&) = delete;
	const Level& operator=(const Level&) = delete;

	static Level* ptr;

	GLFWwindow* window;

	std::vector<Model*> allObjects;



	//camera 
	struct Camera
	{
		float     fovy = 60.0f;
		float     width = 16.0f;
		float     height = 9.0f;
		float     nearPlane = 1.0f;
		float     farPlane = 500.0f;
		glm::vec3 camPos;
		glm::vec3 camTarget;
		glm::vec3 camUp;
		glm::vec3 camRight;
		glm::vec3 camForward;

		glm::mat4 ViewMat;
		glm::mat4 ProjMat;
	};

	Camera cam;

	//shaders
	cg::Program* shader;

public:
	const std::vector<Model*>& getAllObject() { return allObjects; }
};

//struct Light
//{
//	int  type;
//
//	glm::vec3 ambient;
//	glm::vec3 diffuse;
//	glm::vec3 specular;
//
//	glm::vec3 position;
//
//	//ambientColor;
//	//diffuseColor;
//	//specularColor;
//	//positionWorld;
//	//  other variables 
//	//  ...
//};
//std::vector<Light*> lights;