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
#include <chrono>

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
	parser.LoadDataFromFile("data/scenes/scene_A3.txt");

	//Convert from parser->obj to Model
	for (auto o : parser.objects)
	{
		allObjects.push_back(new Model(o));
	}

	for (auto o : parser.lights)
	{
		allLights.push_back(new Light(o));
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

	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);

	CreateColorTexture();
	CreateDepthTexture();

	return 0;
}


void Level::Run()
{
	glClearColor(0, 0, 0, 0);
	float TLastFrame = 0;

	// Main loop
	while (!glfwWindowShouldClose(window)) 
	{
		float TCurrentFrame = 0;
		std::chrono::time_point<std::chrono::steady_clock> time = std::chrono::steady_clock::now();
		// Render graphics here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Update objects pos
		for (auto obj : allObjects)
			obj->ModelUpdate(TLastFrame);
		

		for (auto light : allLights)
			light->LightUpdate(TLastFrame);

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

		//light.ViewMat = glm::lookAt(cam.camPos, cam.camTarget, up);

		//The image is mirrored on X
		cam.ProjMat = glm::perspective(glm::radians(cam.fovy), cam.width / cam.height, cam.nearPlane, cam.farPlane);

		//glbindFramebuffer(ajsfj);

		//glViewport(200, 200, 200, 200);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//For each object in the level
		for (auto o : allObjects)
		{
			Render(o);
		}
		for (auto l : allLights)
		{
			Render(l->m);
		}
	
		//뷰포트 변경
		//SubRender();

		glUseProgram(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		std::chrono::time_point<std::chrono::steady_clock> endtime = std::chrono::steady_clock::now();

		TCurrentFrame = std::chrono::duration<float>(endtime - time).count();
		TLastFrame = TCurrentFrame;
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

void Level::CreateColorTexture()
{
	const int width = 6;
	const int height = 6;
	const int patternSize = 6;

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
			int colorIdx = (x + (height - 1 - y)) % patternSize;
			for (int py = 0; py < patternSize; py++)
			{  // 세로 반복
				for (int px = 0; px < patternSize; px++)
				{  // 가로 반복
					int idx = ((y * patternSize + py) * width * patternSize + (x * patternSize + px)) * 3;
					data[idx] = colors[colorIdx].r * 255;
					data[idx + 1] = colors[colorIdx].g * 255;
					data[idx + 2] = colors[colorIdx].b * 255;
				}
			}
		}
	}

	//Texture 생성
	glGenTextures(1, &colorID);
	//어느 Texture에 할건지 active
	glActiveTexture(GL_TEXTURE0);
	//TextureID에 바인딩
	glBindTexture(GL_TEXTURE_2D, colorID);
	//TextureData 삽입
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width * 6, height * 6, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//Data 삽입 glTexParameteri
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	delete[] data;

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Level::CreateDepthTexture()
{
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, W_WIDTH, W_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);


	//hdjka
}

void Level::Render(Model* obj)
{
	if (obj->transf.name != "Light")
	{
		shader->setUniform("lineSW", false);
		shader->setUniform("lightSW", false);
	}
	else
	{
		shader->setUniform("lineSW", false);
		shader->setUniform("lightSW", true);
	}
	glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
	glBindVertexArray(obj->VAO);

	//Send model matrix to the shader

	glm::mat4x4 m2w = obj->ComputeMatrix();
	//Send view matrix to the shader

	glm::mat4x4 CameraMatrix = cam.ProjMat * cam.ViewMat;

	shader->setUniform("model", m2w);
	shader->setUniform("camera", CameraMatrix);
	
	//TBN===========================================
	//shader->setUniform("tangent", obj->tangent);
	//shader->setUniform("bitangent", obj->bitangent);
	//==============================================

	//Color Texture Binding
	glBindTextureUnit(0, colorID);
	shader->setUniform("myTextureSampler", 0);

	//Image Texture Binding
	glBindTextureUnit(1, obj->imageID);
	shader->setUniform("normalTexture", 1); //maybe 1.

	if (shaderSW > 3)
		shaderSW = 0;
	shader->setUniform("shaderSW", shaderSW);

	if (wireSW)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, obj->points.size());


	//Line
	shader->setUniform("lineSW", true);
	shader->setUniform("lightSW", false);
	glBindBuffer(GL_ARRAY_BUFFER, obj->norVBO);
	glBindVertexArray(obj->norVAO);

	if (norOnOffSW)
		glDrawArrays(GL_LINES, 0, obj->drawNormal.size());


	shader->setUniform("uLightNum", int(allLights.size()));
	for (size_t i = 0; i < allLights.size(); i++)
	{
		//Type of light
		shader->setUniform("uLight[" + std::to_string(i) + "].type", GetType(allLights[i]->type));
		//Properity of light
		shader->setUniform("uLight[" + std::to_string(i) + "].ambient", allLights[i]->ambient);
		shader->setUniform("uLight[" + std::to_string(i) + "].diffuse", allLights[i]->diffuse);
		shader->setUniform("uLight[" + std::to_string(i) + "].specular", allLights[i]->specular);
		//Position
		shader->setUniform("uLight[" + std::to_string(i) + "].positionWorld", allLights[i]->position);
		//Direction
		shader->setUniform("uLight[" + std::to_string(i) + "].dir", allLights[i]->direct);
		//Light Attenuation
		shader->setUniform("uLight[" + std::to_string(i) + "].atten", allLights[i]->atten);
		//Angle
		shader->setUniform("uLight[" + std::to_string(i) + "].inner", allLights[i]->inner);
		shader->setUniform("uLight[" + std::to_string(i) + "].outer", allLights[i]->outer);
		shader->setUniform("uLight[" + std::to_string(i) + "].falloff", allLights[i]->falloff);


		//  other variables 
		//  ...
	}

	//Material shininess --> (ns)
	shader->setUniform("Mshininess", obj->transf.ns);

	//Cameara Position
	shader->setUniform("CameraPosition", cam.camPos);

}


Level::Level(): window (nullptr), shader(nullptr)
{

}

Level::~Level()
{
	for (auto m : allObjects)
		delete m;
	for (auto l : allLights)
		delete l;

	allObjects.clear();
	allLights.clear();
}

int Level::GetType(std::string _type)
{
	if (_type == "POINT")
		return 0;
	else if (_type == "DIR")
		return 1;
	else if (_type == "SPOT")
		return 2;
}
