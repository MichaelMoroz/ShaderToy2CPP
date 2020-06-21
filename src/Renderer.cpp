#include "Renderer.h"

float GetAvgIllumination(sf::Texture* txt)
{
	glBindTexture(GL_TEXTURE_2D, txt->getNativeHandle());
	
	//get the average of the texture using mipmaps
	float avg[4];
	int mipmap_level = floor(log2(float(std::max(txt->getSize().x, txt->getSize().y))));
	glGenerateMipmap(GL_TEXTURE_2D);
	glGetTexImage(GL_TEXTURE_2D, mipmap_level, GL_RGBA, GL_FLOAT, avg);
	GLenum err = glGetError();
	glBindTexture(GL_TEXTURE_2D, 0);

	return sqrt(avg[0] * avg[0] + avg[1] * avg[1] + avg[2] * avg[2]);
}

//Generate a buffer from an existing texture
Buffer::Buffer(Texture* tex)
{
	out0 = tex;
	out1 = tex;
	Generate();
}

//Create a new FLOAT32 texture for the buffer
Buffer::Buffer(int w, int h)
{
	out0 = new Texture(w, h, GL_RGBA32F);
	out1 = new Texture(w, h, GL_RGBA32F);
	Generate();
}

void Buffer::Generate()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	//color attachment texture
	glBindTexture(GL_TEXTURE_2D, out0->GetNative());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out0->GetNative(), 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ERROR_MSG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Buffer::SetInput(int i, Texture* t)
{
	fv->setUniform(i, t->GetNative());
	in[i] = t;
}

void Buffer::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glDisable(GL_DEPTH_TEST); 
	glDisable(GL_BLEND);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	fv->Use();

	for (auto T:in)
	{
		int i = T.first;
		glActiveTexture(GL_TEXTURE0 + i); 
		glBindTexture(GL_TEXTURE_2D, T.second->GetNative());
	}

	glBindVertexArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
	//	ERROR_MSG(("ERROR::FRAMEBUFFER::Render error " + std::to_string(err) + "!").c_str());
	}

	if (out0 != out1)
	{
		//swap buffers
		std::swap(out0, out1);

		//reattach the other buffer
		glBindTexture(GL_TEXTURE_2D, out0->GetNative());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out0->GetNative(), 0);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Shader* Buffer::GetShaderPtr()
{
	return fv;
}

void Buffer::SetShader(std::string frag, std::string vert)
{
	fv = new Shader();
	fv->LoadShader(frag, GL_FRAGMENT_SHADER);
	fv->LoadShader(vert, GL_VERTEX_SHADER);
	fv->Finish();
}

void Buffer::SetShader(Shader* s)
{
	fv = s;
}

GLuint Buffer::GetOutputNative()
{
	return out1->GetNative();
}

Texture* Buffer::GetOutput()
{
	return out1;
}

