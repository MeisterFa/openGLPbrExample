#include "scene.h"
#include "scenerunner.h"
#include "scenepbr.h"


int main(int argc, char *argv[])
{
	SceneRunner runner("Chapter 4 - ");
	std::unique_ptr<Scene> scene;
	scene = std::unique_ptr<Scene>(new ScenePbr());
	return runner.run(std::move(scene));
}
