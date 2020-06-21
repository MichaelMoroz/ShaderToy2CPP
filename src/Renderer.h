#pragma once
#include<Shaders.h>

class Buffer
{
public:
	Buffer(Texture* tex);
	Buffer(int w, int h);

	void Generate();
	void AddInput(Texture& t);
	void Render();

	Shader* GetShaderPtr();
	void SetShader(std::string frag, std::string vert = default_vert);
	void SetShader(Shader* s);
	GLuint GetOutput();

private:
	Shader* fv;
	Texture* out;
	std::vector<Texture*> in;

	GLuint framebuffer, rbo;
};

