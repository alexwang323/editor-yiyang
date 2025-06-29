#pragma once
#include <GL/glew.h>

class Framebuffer {
    friend class Scene;
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void Bind();
    void Unbind();
    void Resize(int newWidth, int newHeight);
    unsigned int GetTextureID() const;
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

private:
    unsigned int fbo;
    unsigned int textureColorBuffer;
    unsigned int rbo;
    int width;
    int height;

    void Init();
};