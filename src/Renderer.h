#pragma once
#include<Shaders.h>

class Buffer
{
public:
	Buffer(Texture* tex);
	Buffer(int w, int h);

	void Generate();
	void SetInput(int i, Texture* t);
	void Render();

	Shader* GetShaderPtr();
	void SetShader(std::string frag, std::string vert = default_vert);
	void SetShader(Shader* s);
	GLuint GetOutputNative();
	Texture* GetOutput();

private:
	Shader* fv;
	Texture *out0, *out1;
	std::map<int, Texture*> in;

	GLuint framebuffer, rbo;
};

