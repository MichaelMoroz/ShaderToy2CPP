#pragma once
#include <map>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#if !defined(__gl_h_) && !defined(__GL_H__) && !defined(_GL_H) && !defined(__X_GL_H)
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <Res.h>


namespace fs = std::filesystem;
using namespace glm;

std::string ConvertSpaces2_(std::string text);

std::vector<fs::path> GetFilesInFolder(std::string folder, std::string filetype);

class Texture
{
public:
	Texture(std::string img);
	Texture(sf::Texture t);
	Texture(sf::Texture* t);
	Texture(int w, int h, GLuint type);

	void Generate(GLuint type);
	GLuint GetNative();
	void GenerateMipMaps();
	vec2 GetSize();


private:
	int W, H;
	GLuint type;
	GLuint texture;
	sf::Texture txt_copy;
};