#include "scene.h"
#include "scenerunner.h"
#include "scenepbr.h"
#include <windows.h>
#include <string>
#include <iostream>

std::wstring ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

int main(int argc, char *argv[]){
	std::wcout << ExePath();
	SceneRunner runner("PBR Beispiel");
	std::unique_ptr<Scene> scene;
	scene = std::unique_ptr<Scene>(new ScenePbr());
	return runner.run(std::move(scene));
}
