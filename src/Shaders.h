#pragma once
#include <string>
#include <fstream>
#include <regex>
#include <filesystem>
#include <map>
#include <algorithm>
#include <sstream>
#include <Res.h>
#include <Resources.h>

namespace fs = std::filesystem;

#ifdef _WIN32
#include <Windows.h>
#define ERROR_MSG(x) MessageBox(nullptr, TEXT(x), TEXT("ERROR"), MB_OK);
#else
#include<iostream>
#define ERROR_MSG(x) std::cerr << x << std::endl;
#endif

bool INIT();

using namespace glm;

class Shader
{
public:
	GLuint ProgramID;

	Shader();

	void LoadShader(const std::string file_path, GLuint type);
	void Finish();

	void Use();

	void RunCompute(vec2 global);

	void setUniform(std::string name, float X, float Y);
	void setUniform(std::string name, float X, float Y, float Z);
	void setUniform(std::string name, float X);
	void setUniform(std::string name, int X);
	void setUniform(std::string name, glm::mat3 X, bool transpose);
	void setUniform(std::string name, glm::vec4 X);
	void setUniform(std::string name, glm::vec3 X);
	void setUniform(std::string name, glm::vec2 X);
	void setUniform(int i, GLuint tid);
	void setUniform(std::string name, GLuint tid);

	GLuint getNativeHandle();

	std::string PreprocessIncludes(const fs::path& filename, int level = 0);

	void SaveErrors(const fs::path& filename, std::string code, std::string errors);

	std::string LoadFileText(fs::path path);

	void Delete();

private:
	int texture_unit;
};
