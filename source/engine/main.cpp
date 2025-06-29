// [ginkgo-engine] main.cpp : engine demo/test entry point

#include "ginkgopch.h"

#include "window.h"
#include "scene.h"
#include "scene2d.h"
#include "Framebuffer.h"
#include "editor.h"
#include "ScriptingSystem.h"
#include "audio.h"
#include "BehaviorTreeSystem.h"
#include "AssetManager.h"
#include "GameObjectFactory.h"
#include "camera.h"
#include "imeManager.h"

static void error_callback(int error, const char* msg) {
    fputs(msg, stderr);
}

int main(int argc, char** argv) {
    Window window = Window::getInstance();
    window.Init(1200, 800, 2);

    //Renderer render;
    //render.Init();
    // Disable any input method editor
    HWND hwnd = glfwGetWin32Window(window.getWindow());
    DisableIME(hwnd);

    CAudioEngine& audioEngine = CAudioEngine::getInstance();
    audioEngine.Initialize();

    Scene& scene = Scene::GetInstance();

    Scene2D& scene2d = Scene2D::GetInstance();
    scene2d.Init(400, 300, window.getSharedContext());
    
    ScriptingSystem& scripting = ScriptingSystem::GetInstance();
    BehaviorTreeSystem& bts = BehaviorTreeSystem::GetInstance();

    InputSystem& inputSystem = InputSystem::GetInstance();

    //scripting.sendBehaviorAgentToLua();

    Framebuffer framebuffer(800, 600);

    Camera* camera = scene.GetCamera();

#ifdef _DEBUG
    scene.Init(framebuffer, window.getSharedContext(), window.getSampleCount());
    Editor editor;
    editor.Init(scene, framebuffer, window.getWindow());
#else
    scene.Init(window.getWindow(), window.getSharedContext(), window.getSampleCount());
	MenuManager::GetInstance().SetWindow(window.getWindow());
	SettingsManager::GetInstance().SetWindow(window.getWindow());
#endif
    MenuManager::GetInstance().LoadMenus("menu.json");
    scripting.Init(); //  calling the setup function in script
    audioEngine.RegisterCallbacks();
    bts.Init();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!window.WindowShouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        window.Update();
		
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bts.Update(0.1f);

        scene2d.Draw(deltaTime);

#ifdef _DEBUG
        editor.Render(deltaTime);
#else
        scene.Draw(window.getWindow(), deltaTime);
        if (MenuManager::GetInstance().GetCurrentMenuState() != MenuState::NONE) {
            MenuManager::GetInstance().RenderActiveMenu();
        }
#endif

        scripting.Update(deltaTime);

        audioEngine.Update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        scene.Iterate();
        //scene.Draw(window.getWindow());

        window.SwapBuffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    audioEngine.Shutdown();
    scripting.Shutdown();

    AssetManager& assetManager = AssetManager::GetInstance();
    assetManager.closeDown();

    GameObjectFactory& GOF = GameObjectFactory::GetInstance();
    GOF.closeDown();

    scene.Shutdown();
    scene2d.Shutdown();

    glfwTerminate();
    exit(EXIT_SUCCESS);
}