#include "scenepbr.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

ScenePbr::ScenePbr() : plane(20, 20, 1, 1), tPrev(0.0f), lightPos(5.0f, 5.0f, 5.0f, 1.0f) {
	mesh = ObjMesh::load("../media/spot/spot_triangulated.obj");
	camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));
}

void ScenePbr::initScene() {
	compileAndLinkShader();

	glEnable(GL_DEPTH_TEST);
	objPos = 7; 
	cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	view = camera.GetViewMatrix(); 
	
	projection = glm::perspective(glm::radians(50.0f),(float)width/height, 0.5f, 100.0f);

	lightAngle = 0.0f;
	lightRotationSpeed = 1.5f;

	prog.setUniform("Light[0].L", glm::vec3(45.0f));
	prog.setUniform("Light[0].Position", view * lightPos);
	prog.setUniform("Light[1].L", glm::vec3(0.3f));
	prog.setUniform("Light[1].Position", glm::vec4(0, 7.15f, -1.0f, 0));
	prog.setUniform("Light[2].L", glm::vec3(45.f));
	prog.setUniform("Light[2].Position", view * glm::vec4(-7, 3, 7, 1));
	prog.setUniform("Light[3].L", glm::vec3(45.0f));
	prog.setUniform("Light[3].Position", view * glm::vec4(7, 3, 7, 1));
}

void ScenePbr::update(float t) {
	float deltaT = t - tPrev;
	if (tPrev == 0.0f) deltaT = 0.0f;
	tPrev = t;
	if (animating()) {
		lightAngle = glm::mod( lightAngle + deltaT * lightRotationSpeed, glm::two_pi<float>() );
		lightPos.x = glm::cos(lightAngle) * 7.0f;
		lightPos.y = 3.0f;
		lightPos.z = glm::sin(lightAngle) * 7.0f;
	}

}

void ScenePbr::update2(float t, int k) {
	float deltaT = t - tPrev;
	float cameraSpeed = 2.5f * deltaT;

	if (k < 10)
		objPos = k; 
	else if (k == 10)
		cameraPos += cameraSpeed * cameraFront;
	else if (k == 11)
		cameraPos -= cameraSpeed * cameraFront;
	else if (k == 12)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	else if (k == 13)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	else if (k == 14)
	{

	}
	else if (k == 15)
	{

	}
	else if (k == 16)
	{

	}
	else if (k == 17)
	{

	}


	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	if (tPrev == 0.0f) 
		deltaT = 0.0f;

	tPrev = t;
	if (animating()) {
		lightAngle = glm::mod(lightAngle + deltaT * lightRotationSpeed, glm::two_pi<float>());
		lightPos.x = glm::cos(lightAngle) * 7.0f;
		lightPos.y = 3.0f;
		lightPos.z = glm::sin(lightAngle) * 7.0f;
	}
}

void ScenePbr::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	prog.setUniform("Light[0].Position", view * lightPos);
	drawScene();
}

void ScenePbr::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void ScenePbr::setMatrices()
{
	glm::mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix", glm::mat3(mv));
	prog.setUniform("MVP", projection * mv);
}

void ScenePbr::compileAndLinkShader() {
	try {
		prog.compileShader("shader/pbr.vert.glsl");
		prog.compileShader("shader/pbr.frag.glsl");
		prog.link();
		prog.use();
	}
	catch (GLSLProgramException & e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ScenePbr::drawScene() {
	drawFloor();

	// Draw dielectric cows with varying roughness
	int numCows = 5;
	glm::vec3 cowBaseColor(0.1f, 0.33f, 0.17f);

	// Draw metal cows
	float metalRough = 0.43f;

	if (objPos < numCows)
	{
		float rough = (objPos + 1) * (1.0f / numCows);
		drawSpot(glm::vec3(0, 0, 1.5f), rough, 0, cowBaseColor);
	}
	else if (objPos == 5)
	{
		// Gold
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(1, 0.71f, 0.29f));
	}
	else if (objPos == 6)
	{
		// Copper
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(0.95f, 0.64f, 0.54f));
	}
	else if (objPos == 7)
	{
		// Aluminum
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(0.91f, 0.92f, 0.92f));
	}
	else if (objPos == 8)
	{
		// Titanium
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(0.542f, 0.497f, 0.449f));
	}
	else if (objPos == 9)
	{
		// Silver
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(0.95f, 0.93f, 0.88f));
	}
}

void ScenePbr::drawFloor() {
	model = glm::mat4(1.0f);
	prog.setUniform("Material.Rough", 0.9f);
	prog.setUniform("Material.Alpha", (0.9f*0.9f));
	prog.setUniform("Material.Metal", 0);
	prog.setUniform("Material.Color", glm::vec3(0.2f));
	model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));

	setMatrices();
	plane.render();
}

void ScenePbr::drawSpot(const glm::vec3 & pos, float rough, int metal, const glm::vec3 & color) {
	model = glm::mat4(1.0f);
	prog.setUniform("Material.Rough", rough);
	prog.setUniform("Material.Alpha", (rough*rough));
	prog.setUniform("Material.Metal", metal);
	prog.setUniform("Material.Color", color);
	model = glm::translate(model, pos);
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	setMatrices();
	mesh->render();
}