#pragma once
#include <string>
#include <vector>

#include "scene.h"

class HierarchyPanel {
public:
	HierarchyPanel();

	void Render(int& selectedObjectID, int windowWidth, int windowHeight);
	void SetScene(Scene* scene);

private:
	Scene* scene;
};
