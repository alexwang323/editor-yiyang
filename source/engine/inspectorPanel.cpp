#include "ginkgopch.h"
#include "inspectorPanel.h"
#include <iostream>

InspectorPanel::InspectorPanel() {}

void InspectorPanel::SetScene(Scene* scene) {
	this->scene = scene;
}

void InspectorPanel::DisplayChildrenInfo(RenderComponent* parentNode) {
    if (parentNode == nullptr) {
        std::cerr << "Error: parentNode is nullptr." << std::endl;
        return;
    }

    // Add your child display logic here
}

// Helper function to find the index of a RenderComponent
int FindRenderComponentIndexByID(Scene* scene, int rcID) {
    for (int i = 0; i < scene->GetRenderComponentsSize(); ++i) {
        RenderComponent& rc = scene->GetRenderComponents(i).get();
        if (rc.GetID() == rcID) {
            return i;
        }
    }
    return -1;
}

void InspectorPanel::Render(int& selectedObjectID, int windowWidth, int windowHeight) {

    ImGui::SetNextWindowPos(ImVec2((float)windowWidth * 0.666f, 50.0f));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth * 0.333f, (float)windowHeight - 300.0f));
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (selectedObjectID != -1) {
        int foundIndex = FindRenderComponentIndexByID(scene, selectedObjectID);

        if (foundIndex >= 0) {
            RenderComponent& selectedNode = scene->GetRenderComponents(foundIndex).get();
            TransformComponent* transform = &(selectedNode.GetTransformComponent());
            Material* material = &(selectedNode.GetMaterial());

            ImGui::Text("Selected Object ID: %d", selectedObjectID);

            glm::vec3 rotationRadians = transform->GetRotation();
            glm::vec3 rotationDegrees = glm::degrees(rotationRadians);

            // keep the values in silder between 0 and 360
            rotationDegrees.x = fmod(rotationDegrees.x, 360.0f);
            if (rotationDegrees.x < 0.0f) rotationDegrees.x += 360.0f;

            rotationDegrees.y = fmod(rotationDegrees.y, 360.0f);
            if (rotationDegrees.y < 0.0f) rotationDegrees.y += 360.0f;

            rotationDegrees.z = fmod(rotationDegrees.z, 360.0f);
            if (rotationDegrees.z < 0.0f) rotationDegrees.z += 360.0f;

            bool positionChanged = false;
            bool rotationChanged = false;
            bool scaleChanged = false;
            bool materialChanged = false;

            if (transform != nullptr) {
                glm::vec3 position = transform->GetPosition();
                ImGui::InputFloat3("Position", &position[0], "%.3f");
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    positionChanged = true;
                }

                if (ImGui::SliderFloat3("Rotation Slider", &rotationDegrees[0], 0.0f, 360.0f)) {
                    rotationRadians = glm::radians(rotationDegrees);
                    rotationChanged = true;
                }

                ImGui::InputFloat3("Rotation Input", &rotationDegrees[0], "%.3f");
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    rotationRadians = glm::radians(rotationDegrees);
                    rotationChanged = true;
                }

                glm::vec3 scale = transform->GetScale();
                ImGui::InputFloat3("Scale", &scale[0], "%.3f");
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    scaleChanged = true;
                }

                if (positionChanged) {
                    transform->SetPosition(position);
                }
                if (rotationChanged) {
                    transform->SetRotation(rotationRadians);
                }
                if (scaleChanged) {
                    transform->SetScale(scale);
                }
            }
            else {
                ImGui::Text("This node doesn't have a TransformComponent.");
            }

            if (material != nullptr) {
                glm::vec3 diffuse = material->GetDiffuse();
                if (ImGui::ColorEdit3("Base Diffuse Color", &diffuse[0])) {
                    material->SetDiffuse(diffuse);
                    materialChanged = true;
                }

                glm::vec3 specular = material->GetSpecular();
                if (ImGui::ColorEdit3("Base Specular Color", &specular[0])) {
                    material->SetSpecular(specular);
                    materialChanged = true;
                }

                float roughness = material->GetRoughness();
                if (ImGui::SliderFloat("Base Roughness", &roughness, 0.05f, 1.0f)) {
                    material->SetRoughness(roughness);
                    materialChanged = true;
                }

                int selectedTexture = -1;
                if (material->HasTexture().test(0)) selectedTexture = 0;
                else if (material->HasTexture().test(1)) selectedTexture = 1;
                else if (material->HasTexture().test(2)) selectedTexture = 2;
                else if (material->HasTexture().test(3)) selectedTexture = 3;

                const char* textureOptions[] = { "Albedo", "Normal", "Specular", "Roughness", "None" };
                if (ImGui::Combo("Texture", &selectedTexture, textureOptions, IM_ARRAYSIZE(textureOptions))) {
                    std::bitset<4> updatedTextureFlags;
                    updatedTextureFlags.reset();

                    if (selectedTexture == 0) updatedTextureFlags.set(0);
                    else if (selectedTexture == 1) updatedTextureFlags.set(1);
                    else if (selectedTexture == 2) updatedTextureFlags.set(2);
                    else if (selectedTexture == 3) updatedTextureFlags.set(3);

                    material->SetHasTexture(updatedTextureFlags);
                    materialChanged = true;
                }

                if (materialChanged) {
                    Shader* shader = material->GetShader();
                    shader->UseShader();
                    material->BindMaterial(shader);
                }
            }
            else {
                ImGui::Text("This node doesn't have a Material component.");
            }
        }
        else {
            ImGui::Text("Invalid object ID: %d", selectedObjectID);
        } 
    }
    else {
        ImGui::Text("No object selected.");
    }

    ImGui::End();
}