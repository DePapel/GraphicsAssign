#include <GL/glew.h>

#include "Level.h"
#include "CS300Parser.h"
#include "Controls.h"
#include "models.h"

#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Level* Level::ptr = nullptr;

int Level::Initialize()
{
	// Initialize GLFW
	if (!glfwInit()) {
		return -1;
	}

	// Create a window
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Assignment 1", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	//Set callbacks
	glfwSetKeyCallback(window, Controls::keyCallback);

	//Load Scene
	CS300Parser parser;
	parser.LoadDataFromFile("data/scenes/test.txt");

	//Convert from parser->obj to Model
	for (auto o : parser.objects)
	{
		allObjects.push_back(new Model(o));
	}

	for (auto o : parser.lights)
	{
		allObjects.push_back(new Model(o));
	}
	//First Make a Light Model
	//parser light -> real light obj
	//Second light shader

	//Save camera
	cam.fovy = parser.fovy;
	cam.width = parser.width;
	cam.height = parser.height;
	cam.nearPlane = parser.nearPlane;
	cam.farPlane = parser.farPlane;
	cam.camPos = parser.camPos;
	cam.camTarget = parser.camTarget;
	cam.camUp = parser.camUp;
	 
	
	//Shader program
	ReloadShaderProgram();
	//glEnable(GL_CULL_FACE);

	//glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);

	return 0;
}


void Level::Run()
{
	glClearColor(0, 0, 0, 0);
	// Main loop
	while (!glfwWindowShouldClose(window)) 
	{
		// Render graphics here
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use shader program
		glUseProgram(shader->handle);

		KeyInput();

		//Calculate Camera Matrix
		glm::vec3 dir = glm::normalize(cam.camTarget - cam.camPos);
		cam.camForward = dir;
		dir = -dir;
		glm::vec3 r = glm::normalize(glm::cross(cam.camUp, dir));
		cam.camRight = r;
		glm::mat4 V = glm::mat4(1);
		glm::vec3 up = glm::normalize(glm::cross(dir, r));


		V[0][0] = r.x;
		V[1][0] = r.y;
		V[2][0] = r.z;
		V[0][1] = up.x;
		V[1][1] = up.y;
		V[2][1] = up.z;
		V[0][2] = dir.x;
		V[1][2] = dir.y;
		V[2][2] = dir.z;
		V[3][0] = -dot(r,cam.camPos );
		V[3][1] = -dot(up, cam.camPos);
		V[3][2] = -dot(dir, cam.camPos);


		//cam.ViewMat = glm::lookAt(cam.camPos, cam.camTarget, up);
		cam.ViewMat = V;

		//The image is mirrored on X
		cam.ProjMat = glm::perspective(glm::radians(cam.fovy), cam.width / cam.height, cam.nearPlane, cam.farPlane);

		//For each object in the level
		for (auto o : allObjects)
		{
			//Render the object
			Render(o);
		}

		glUseProgram(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return;
}

void Level::Cleanup()
{
	// Clean up
	glfwDestroyWindow(window);
	glfwTerminate();

	DeletePtr();
}

Level* Level::GetPtr()
{
	if (!ptr)
		ptr = new Level;
	return ptr;
}

void Level::DeletePtr()
{
	if (ptr)
	{
		delete ptr;
	}
	
	ptr = nullptr;
}

#include <fstream>
#include <sstream>

void Level::ReloadShaderProgram()
{
	glUseProgram(0);

	if (shader)
		delete shader;

	std::stringstream v;
	std::stringstream f;

	std::ifstream file("data/shaders/vert.vert");

	if (file.is_open())
	{
		v << file.rdbuf();
	}

	file.close();
	file.open("data/shaders/frag.frag");
	f << file.rdbuf();
	file.close();

	shader = new cg::Program(v.str().c_str(), f.str().c_str());
}

void Level::KeyInput()
{
	if(keyStateW)
		RotateCamX(1.f);
	if(keyStateS)
		RotateCamX(-1.f);
	if(keyStateA)
		RotateCamY(1.f);
	if(keyStateD)
		RotateCamY(-1.f);
}
void Level::Joom(float val)
{
	val *= 0.01f;
	cam.camPos += val * (cam.camTarget - cam.camPos);
}

void Level::RotateCamX(float angle)
{
	glm::vec3 right = glm::cross(cam.camUp, cam.camPos - cam.camTarget);
	glm::vec3 rotVec = glm::vec3(glm::rotate(glm::identity<glm::mat4>(), glm::radians(-angle), right) * glm::vec4(cam.camTarget - cam.camPos, 1));

	if (abs(rotVec.z) > 0.1f)
		cam.camPos = cam.camTarget - rotVec;
}

void Level::RotateCamY(float angle)
{
	cam.camPos = cam.camTarget - glm::vec3(glm::rotate(glm::identity<glm::mat4>(), glm::radians(-angle), cam.camUp) * glm::vec4(cam.camTarget - cam.camPos,1));
}

void Level::Render(Model* obj)
{
	//use obj VBO
	glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
	//use obj VAO
	glBindVertexArray(obj->VAO);

	//Send model matrix to the shader

	glm::mat4x4 m2w = obj->ComputeMatrix();

	//Send view matrix to the shader
	shader->setUniform("model", cam.ProjMat * cam.ViewMat * m2w);

	//uniform 던져주기============================================
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obj->textureID);
	shader->setUniform("myTextureSampler", 0);
	shader->setUniform("shaderSW", shaderSW);
	
	//=====================================================

	//shader->setUniform("uLightNum", int(lights.size()));
	//for (size_t i = 0; i < lights.size(); i++)
	//{
	//	shader->setUniform("uLight[" + std::to_string(i) + "].type", int(lights[i]->type));
	//
	//	shader->setUniform("uLight[" + std::to_string(i) + "].ambient", lights[i]->ambientColor);
	//	shader->setUniform("uLight[" + std::to_string(i) + "].diffuse", lights[i]->diffuseColor);
	//	shader->setUniform("uLight[" + std::to_string(i) + "].specular", lights[i]->specularColor);
	//
	//	shader->setUniform("uLight[" + std::to_string(i) + "].positionWorld", lights[i]->positionWorld);
	//
	//	//  other variables 
	//	//  ...
	//}

	//draw
	if (wireSW)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
	shader->setUniform("lineSW", false);
	glDrawArrays(GL_TRIANGLES, 0, obj->points.size());

	shader->setUniform("lineSW", true);
	shader->setUniform("lineColor", drawColor);
	glBindBuffer(GL_ARRAY_BUFFER, obj->norVBO);
	glBindVertexArray(obj->norVAO);

	if (norOnOffSW)
		glDrawArrays(GL_LINES, 0, obj->drawNormal.size());


	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}


Level::Level(): window (nullptr), shader(nullptr)
{

}

Level::~Level()
{
	for (auto m : allObjects)
		delete m;

	allObjects.clear();
}
