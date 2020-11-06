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
    std::unique_ptr<ObjMesh> mesh;
    Camera camera;
    float tPrev, lightAngle, lightRotationSpeed, roughness;
    std::string objMaterial; 
	glm::vec4 lightPos;
    glm::vec3 cameraPos, cameraFront, cameraUp;

    void setMatrices();
    void compileAndLinkShader();

	void drawScene();
	void drawFloor();
	void drawSpot(const glm::vec3 & pos, float rough, int metal, const glm::vec3 & color);

public:
    ScenePbr();

    void initScene();
    void update( float t );
    void update2(float t, std::string k);
    void processKeyboardInput(std::string& keypress, float deltaT);
    void render();
    void resize(int, int);
};
