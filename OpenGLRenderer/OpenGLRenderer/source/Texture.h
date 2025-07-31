#pragma once

#include <cstdint>
#include <string>

class Texture {
public:
	Texture(const char* source, bool flip = true);
	
	void Activate(uint32_t textureUnit = 0);

	const std::string& GetFileName() const { return fileName; }

	void SetFileName(const std::string& newPath) { fileName = newPath; }

private:
	uint32_t id;

	std::string fileName;
};
