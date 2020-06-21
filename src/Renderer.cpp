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
	out = tex;
	Generate();
}

//Create a new FLOAT32 texture for the buffer
Buffer::Buffer(int w, int h)
{
	out = new Texture(w, h, GL_RGBA32F);
	Generate();
}

void Buffer::Generate()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture

	glBindTexture(GL_TEXTURE_2D, out->GetNative());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out->GetNative(), 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, out->GetSize().x, out->GetSize().y); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ERROR_MSG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Buffer::AddInput(Texture& t)
{
	in.push_back(&t);
}

void Buffer::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glDisable(GL_DEPTH_TEST); 

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	fv->Use();

	for (int i = 0; i < in.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); 
		glBindTexture(GL_TEXTURE_2D, in[i]->GetNative());
	}

	glBindVertexArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

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

GLuint Buffer::GetOutput()
{
	return out->GetNative();
}
