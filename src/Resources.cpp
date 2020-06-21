#include "Resources.h"

using namespace std;

string ConvertSpaces2_(string text)
{
	while (true)
	{
		std::size_t pos = text.find(" ");

		if (pos == std::string::npos)
			break;

		text = text.replace(pos, 1, "_");
	}
	return text;
}

//get path vector of all files of given type
vector<fs::path> GetFilesInFolder(string folder, string filetype)
{
	vector<fs::path> paths;

	for (const auto & entry : fs::directory_iterator(folder))
	{
		//check if the file has the correct filetype
		if (entry.path().extension().string() == filetype)
		{
			paths.push_back(entry.path());
		}
	}

	//sort the files in alphabetical order
	sort(paths.begin(), paths.end());

	return paths;
}

Texture::Texture(std::string img)
{
	txt_copy.loadFromFile(img);
	sf::Vector2u s = txt_copy.getSize();
	W = s.x; H = s.y;
	texture = txt_copy.getNativeHandle();
	type = GL_RGBA;
}

Texture::Texture(sf::Texture t)
{
	txt_copy = t;
	sf::Vector2u s = txt_copy.getSize();
	W = s.x; H = s.y;
	texture = txt_copy.getNativeHandle();
	type = GL_RGBA;
}

Texture::Texture(sf::Texture *t)
{
	//txt_copy = t;
	sf::Vector2u s = t->getSize();
	W = s.x; H = s.y;
	texture = t->getNativeHandle();
	type = GL_RGBA;
}

Texture::Texture(int w, int h, GLuint type = GL_RGBA32F): W(w), H(h)
{
	Generate(type);
}

void Texture::Generate(GLuint type = GL_RGBA32F)
{
	Texture::type = type;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, type, W, H, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GLuint Texture::GetNative()
{
	return texture;
}

void Texture::GenerateMipMaps()
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glGenerateMipmap(GL_TEXTURE_2D);
}

vec2 Texture::GetSize()
{
	return vec2(W, H);
}
