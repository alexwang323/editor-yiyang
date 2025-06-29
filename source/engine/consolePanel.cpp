#include "ginkgopch.h"
#include "imgui.h"
#include "consolePanel.h"

ConsolePanel::ConsolePanel() {}

void ConsolePanel::SetScene(Scene* scene) {
    this->scene = scene;
}

void ConsolePanel::Render(int windowWidth, int windowHeight) {

    ImGui::SetNextWindowPos(ImVec2((float)windowWidth * 0.333f, (float)windowHeight - 250));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth * 0.666f, 250));
    ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    for (const auto& message : consoleMessages) {
        ImGui::TextWrapped("%s", message.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::End();
}