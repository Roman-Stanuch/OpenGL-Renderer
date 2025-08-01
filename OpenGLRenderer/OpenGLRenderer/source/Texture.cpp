#include "Texture.h"
#include "glad/glad.h"
#include "stb/stb_image.h"

#include <iostream>

Texture::Texture(const char* source, bool flip) {
	id = 0;
	int width;
	int height;
	int numberOfChannels;
	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(source, &width, &height, &numberOfChannels, 0);
	if (data) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		GLuint textureType = (numberOfChannels == 3) ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, textureType, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR: Failed to load texture at " << source << "\n";
	}
	stbi_image_free(data);
}

void Texture::Activate(uint32_t textureUnit) {
	glBindTextureUnit(textureUnit, id);
}
