#pragma once
#include <vector>
#include <string>

#include "scene.h"

class InspectorPanel {
public:
	InspectorPanel();
	void Render(int& selectedObjectID, int windowWidth, int windowHeight);
	void DisplayChildrenInfo(RenderComponent* parentNode);
	void SetScene(Scene* scene);

private:
	Scene* scene;
};
//class Scene;

//void RenderInspectorPanel(int &selectedObjectID, Scene& scene);
