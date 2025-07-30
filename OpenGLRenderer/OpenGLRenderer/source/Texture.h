#pragma once

#include <cstdint>

class Texture {
public:
	Texture(const char* source, bool flip = true);
	
	void Activate(uint32_t textureUnit = 0);

private:
	uint32_t id;
};
