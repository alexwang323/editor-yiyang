#pragma once
#include <string>
#include <vector>

#include "scene.h"

class ConsolePanel {
public:
	ConsolePanel();
	void SetScene(Scene* scene);
	void Render(int windowWidth, int windowHeight);
	static std::vector<std::string> consoleMessages;

private:
	Scene* scene;
};
