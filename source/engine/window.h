#pragma once

// [ginkgo-engine] window.h : window refactor after first checkpoint

#include "ginkgopch.h"

struct FrameBuffer {
	int width = 0;
	int height = 0;
};

class Window {
public:
	void Init(unsigned int width, unsigned int height, unsigned int samples);
	void Update();
	void SwapBuffers() { glfwSwapBuffers(windowPtr); }

	void ShowCursor() { glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
	void HideCursor() { glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); }

	static Window& getInstance();
	GLFWwindow* getWindow() { return windowPtr; };
	GLFWwindow* getSharedContext() { return workerSharedContext; }

	bool WindowShouldClose() { return glfwWindowShouldClose(windowPtr); }

	unsigned int getSampleCount() { return MSAAsamples; }

private:
	GLFWwindow* windowPtr;
	GLFWwindow* workerSharedContext;
	FrameBuffer framebuffer;
	bool window_initialized = false;

	unsigned int MSAAsamples;

	static void GLFWErrorCallback(int error, const char* msg);
};