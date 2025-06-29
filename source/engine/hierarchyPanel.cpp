#include "ginkgopch.h"
#include "imgui.h"
#include "hierarchyPanel.h"
#include "scene.h"
#include "rendercomponent.h"
#include "GameObject.h"

HierarchyPanel::HierarchyPanel() {}

void HierarchyPanel::SetScene(Scene* scene) {
	this->scene = scene;
}

void HierarchyPanel::Render(int& selectedObjectID, int windowWidth, int windowHeight) {

    ImGui::SetNextWindowPos(ImVec2(0, (float)windowHeight - 250.0f));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth * 0.333f, 250.0f));
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    for (int i = 0; i < scene->GetRenderComponentsSize(); i++) {

        RenderComponent& rcRef = scene->GetRenderComponents(i);
        int rcID = rcRef.GetID();

        GameObject* go = scene->FindGameObjectOfRenderComponent(rcID);

        std::string nodeName;
        if (go) {
            nodeName = go->GetName();
        }
        else {
            nodeName = "Node " + std::to_string(i);
        }

        if (ImGui::Selectable(nodeName.c_str(), selectedObjectID == rcID)) {
            selectedObjectID = rcID;
        }
    }

    ImGui::End();
}