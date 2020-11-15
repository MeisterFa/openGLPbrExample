#pragma once

#include "scene.h"
#include "glslprogram.h"
#include "objmesh.h"
#include "cookbookogl.h"
#include <glm/glm.hpp>
#include <plane.h>
#include <camera.h>

class ScenePbr : public Scene {
private:
    GLSLProgram prog;

    Plane plane;
    std::unique_ptr<ObjMesh> mesh1; 
    std::unique_ptr<ObjMesh> mesh2; 
    std::unique_ptr<ObjMesh> mesh3;
    int meshNumber;
    Camera camera;
    float tPrev, lightAngle, lightRotationSpeed, roughness;
    std::string objMaterial; 
	glm::vec4 lightPos, lightPos1, lightPos2;
    glm::vec3 lightIntensity, lightIntensity1, lightIntensity2;
    glm::vec3 cameraPos, cameraFront, cameraUp;

    void setMatrices();
    void compileAndLinkShader();

	void drawScene();
	void drawFloor();
	void drawSpot(const glm::vec3 & pos, float rough, int metal, const glm::vec3 & color);

public:
    ScenePbr();

    void initScene();
    void update(float t, std::string k);
    void updateMouseMovement(double xpos, double ypos);
    void processKeyboardInput(std::string& keypress, float deltaT);
    void render();
    void resize(int, int);
};
