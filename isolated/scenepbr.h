#pragma once

#include "scene.h"
#include "glslprogram.h"
#include "objmesh.h"
#include "cookbookogl.h"
#include <glm/glm.hpp>
#include <plane.h>

class ScenePbr : public Scene {
private:
    GLSLProgram prog;

    Plane plane;
    std::unique_ptr<ObjMesh> mesh;

	float tPrev, lightAngle, lightRotationSpeed;
	glm::vec4 lightPos;

    void setMatrices();
    void compileAndLinkShader();

	void drawScene();
	void drawFloor();
	void drawSpot(const glm::vec3 & pos, float rough, int metal, const glm::vec3 & color);

public:
    ScenePbr();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};
