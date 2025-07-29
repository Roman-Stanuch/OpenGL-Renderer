#pragma once

#include "GLFW/glfw3.h"

class Texture {
public:
	Texture(const char* source, bool flip = true);
	
	void Activate(GLuint textureUnit = 0);

private:
	GLuint id;
};
