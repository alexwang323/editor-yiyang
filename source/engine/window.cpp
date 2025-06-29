#include "ginkgopch.h"
#include "window.h"


void Window::GLFWErrorCallback(int error, const char* msg) {
    std::cout << "GLFW error " << error << ": " << msg << std::endl;
}

void Window::Init(unsigned int width, unsigned int height, unsigned int samples) {
    glfwSetErrorCallback(this->GLFWErrorCallback);
    
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_RESIZABLE, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    MSAAsamples = samples;
    glfwWindowHint(GLFW_SAMPLES, samples);

    this->windowPtr = glfwCreateWindow(width, height, "Ginkgo Engine", NULL, NULL);

    if (!windowPtr) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // shared context for worker thread
    // make the width and height as small as possible since it's not for rendering
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    workerSharedContext = glfwCreateWindow(1, 1, "worker thread context", NULL, this->windowPtr);
    if (!workerSharedContext) {
        std::cout << "Failed to create worker thread shared context " << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(this->windowPtr);

    GLenum glew_init_result = glewInit();
    if (glew_init_result != GLEW_OK) {
        exit(glew_init_result);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(this->windowPtr, true);
    ImGui_ImplOpenGL3_Init("#version 430");
    ImGui::StyleColorsDark();

}

void Window::Update() {
    glfwPollEvents();
}

Window& Window::getInstance() {
    static Window instance;
    return instance;
}
