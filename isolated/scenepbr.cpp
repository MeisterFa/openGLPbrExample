#include "scenepbr.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

ScenePbr::ScenePbr() : plane(20, 20, 1, 1), tPrev(0.0f), lightPos(5.0f, 3.0f, 5.0f, 1.0f), lightPos1(0, 15.15f, -5.0f, 0), lightPos2(5.0f, 3.0f, 5.0f, 1.0f) {
	meshNumber = 1; 
	mesh1 = ObjMesh::load("../media/Chess_Rook.obj");
	mesh2 = ObjMesh::load("../media/cow.obj");
	mesh3 = ObjMesh::load("../media/bs_ears.obj");
	lightIntensity = glm::vec3(40.0f);
	lightIntensity1 = glm::vec3(6.0f);
	lightIntensity2 = glm::vec3(50.0f);

	camera = Camera(glm::vec3(0.0f, 1.5f, 10.0f));
	roughness = 0.01f; 
}

void ScenePbr::initScene() {
	compileAndLinkShader();

	glEnable(GL_DEPTH_TEST);
	objMaterial = "gold";
	view = camera.GetViewMatrix(); 
	projection = glm::perspective(glm::radians(50.0f),(float)width/height, 0.5f, 100.0f);

	lightAngle = 0.0f;
	lightRotationSpeed = 1.5f;

	prog.setUniform("Light[0].L", lightIntensity);
	prog.setUniform("Light[0].Position", view*lightPos);
	prog.setUniform("Light[1].L", lightIntensity1);
	prog.setUniform("Light[1].Position", view * lightPos1);
	prog.setUniform("Light[2].L", lightIntensity2);
	prog.setUniform("Light[2].Position", view*lightPos2);
}

void ScenePbr::update(float t, std::string keypress) {
	float deltaT = t - tPrev;
	if (tPrev == 0.0f)
		deltaT = 0.0f;
	tPrev = t;

	processKeyboardInput(keypress, deltaT);

	if (animating()) {
		lightAngle = glm::mod(lightAngle + deltaT * lightRotationSpeed, glm::two_pi<float>());
		lightPos.x = glm::cos(lightAngle) * 7.0f;
		lightPos.y = 3.0f;
		lightPos.z = glm::sin(lightAngle) * 7.0f;
	}
}

void ScenePbr::updateMouseMovement(double xpos, double ypos)
{
	camera.ProcessMouseMovement(xpos, ypos);
}

void ScenePbr::processKeyboardInput(std::string& keypress, float deltaT)
{
	if (keypress == "forward")
		camera.ProcessKeyboard(FORWARD, deltaT);
	else if (keypress == "backward")
		camera.ProcessKeyboard(BACKWARD, deltaT);
	else if (keypress == "left")
		camera.ProcessKeyboard(LEFT, deltaT);
	else if (keypress == "right")
		camera.ProcessKeyboard(RIGHT, deltaT);
	else if (keypress == "arrow_up")
		lightIntensity += 10 * deltaT;
	else if (keypress == "arrow_down" && lightIntensity.x > 5)
		lightIntensity -= 10 * deltaT;
	else if (keypress == "arrow_left" && lightIntensity2.x > 5)
		lightIntensity2 -= 10 * deltaT;
	else if (keypress == "arrow_right")
		lightIntensity2 += 10 * deltaT;
	else if (keypress == "meshOne")
		meshNumber = 1;
	else if (keypress == "meshTwo")
		meshNumber = 2;
	else if (keypress == "meshThree")
		meshNumber = 3;
	else if (keypress == "plus_roughness")
	{
		if (roughness < 0.99f)
			roughness += 0.5 * deltaT;
	}
	else if (keypress == "minus_roughness")
	{
		if (roughness > 0.02f)
			roughness -= 0.5 * deltaT;
	}
	else if (keypress == "gold" || keypress == "copper" || keypress == "aluminum" || keypress == "titanium" || keypress == "silver"
		|| keypress == "noMetal")
		objMaterial = keypress;
	else if (keypress == "mouseEnable")
		camera.firstMouse = true;
}

void ScenePbr::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	view = camera.GetViewMatrix();

	prog.setUniform("Light[0].L", lightIntensity);
	prog.setUniform("Light[0].Position", view * lightPos);
	prog.setUniform("Light[1].L", lightIntensity1);
	prog.setUniform("Light[1].Position", view * lightPos1);
	prog.setUniform("Light[2].L", lightIntensity2);
	prog.setUniform("Light[2].Position", view * lightPos2);

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
		prog.compileShader("shader/pbr_fabian.frag.glsl");
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
	glm::vec3 cowBaseColor(0.9f, 0.13f, 0.17f);

	// Draw metal cows
	float metalRough = 0.43f;

	if (objMaterial == "gold")
	{
		// Gold
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(1, 0.71f, 0.29f));
	}
	else if (objMaterial == "copper")
	{
		// Copper
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(0.95f, 0.64f, 0.54f));
	}
	else if (objMaterial == "aluminum")
	{
		// Aluminum
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(0.91f, 0.92f, 0.92f));
	}
	else if (objMaterial == "titanium")
	{
		// Titanium
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(0.542f, 0.497f, 0.449f));
	}
	else if (objMaterial == "silver")
	{
		// Silver
		drawSpot(glm::vec3(0.0f, 0.0f, 1.5f), metalRough, 1, glm::vec3(0.95f, 0.93f, 0.88f));
	}
	else if(objMaterial == "noMetal")
	{
		drawSpot(glm::vec3(0, 0, 1.5f), roughness, 0, cowBaseColor);
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

	if (meshNumber == 1)
	{
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		setMatrices();
		mesh1->render();

	}
	else if (meshNumber == 2)
	{
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		setMatrices();
		mesh2->render();
		
	}
	else if (meshNumber == 3)
	{
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		setMatrices();
		mesh3 ->render();
	}
}