#include "Shaders.h"
bool initialized = false;

std::string int2str(const int n)
{
	std::ostringstream stm;
	stm << n;
	return stm.str();
}

Shader::Shader(): texture_unit(0)
{
	ProgramID = glCreateProgram();
}

std::string Shader::LoadFileText(fs::path path)
{
	std::string text;
	std::ifstream TextStream(path, std::ios::in);
	if (TextStream.is_open())
	{
		std::string Line = "";
		while (getline(TextStream, Line))
			text += Line + "\n";
		TextStream.close();
	}
	else
	{
		ERROR_MSG("Impossible to open text file");
	}
	return text;
}

void Shader::Delete()
{
	glDeleteProgram(ProgramID);
}


void Shader::LoadShader(const std::string file_path, GLuint type = GL_COMPUTE_SHADER)
{
		// Create the shaders
		GLuint ShaderID = glCreateShader(type);

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Read the Shader code from the file
		std::string ShaderCode = PreprocessIncludes(fs::path(file_path));

		// Compile Shader
		char const * SourcePointer = ShaderCode.c_str();
		glShaderSource(ShaderID, 1, &SourcePointer, NULL);
		glCompileShader(ShaderID);

		// Check Shader
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (Result == GL_FALSE)
		{
			std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			ERROR_MSG(("Shader compilation error. \n" + std::string(&ShaderErrorMessage[0])).c_str());
			SaveErrors(file_path, ShaderCode, std::string(&ShaderErrorMessage[0]));
		}

		// Link the program
		glAttachShader(ProgramID, ShaderID);
}

void Shader::Finish()
{
	glLinkProgram(ProgramID);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (Result == GL_FALSE)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		ERROR_MSG(("Program error. \n" + std::string(&ProgramErrorMessage[0])).c_str());
	}
}

void Shader::Use()
{
	glUseProgram(ProgramID);
}

void Shader::RunCompute(vec2 global)
{
	Shader::Use();
	glDispatchCompute(ceil(global.x), ceil(global.y), 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Shader::setUniform(std::string name, float X, float Y)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform2f(A, X, Y);
}

void Shader::setUniform(std::string name, float X, float Y, float Z)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform3f(A, X, Y, Z);
}

void Shader::setUniform(std::string name, float X)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform1f(A, X);
}

void Shader::setUniform(std::string name, int X)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform1i(A, X);
}

void Shader::setUniform(std::string name, glm::mat3 X, bool transpose)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniformMatrix3fv(A, 1, transpose, glm::value_ptr(X));
}

void Shader::setUniform(std::string name, glm::vec4 X)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform4fv(A, 1, glm::value_ptr(X));
}

void Shader::setUniform(std::string name, glm::vec3 X)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform3fv(A, 1, glm::value_ptr(X));
}

void Shader::setUniform(std::string name, glm::vec2 X)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform2fv(A, 1, glm::value_ptr(X));
}

void Shader::setUniform(int i, GLuint tid)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, ("iChannel" + std::to_string(i)).c_str());
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, tid);
}

/*
//dont use!
void Shader::setUniform(std::string name, GLuint tid)
{
	glUseProgram(ProgramID);
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, tid);
	texture_unit++;
}
*/

GLuint Shader::getNativeHandle()
{
	return ProgramID;
}

bool INIT()
{
	if (initialized)
	{
		return true;
	}
	if ( glewInit() != GLEW_OK) 
	{
		ERROR_MSG("Failed to initialize GLEW\n");
		return false;
	}
	initialized = true;
	return true;
}


std::string Shader::PreprocessIncludes(const fs::path& filename, int level /*= 0 */)
{
	if (level > 32)
		ERROR_MSG("Header inclusion depth limit reached, might be caused by cyclic header inclusion");
	using namespace std;

	//match regular expression
	static const regex re("^[ ]*#include\s*[\"<](.*)[\">].*");
	stringstream input;
	stringstream output;
	input << LoadFileText(filename);

	smatch matches;
	string line;
	while (std::getline(input, line))
	{
		if (regex_search(line, matches, re))
		{
			//add the code from the included file
			std::string include_file = matches[1].str();
			output << PreprocessIncludes(include_file, level + 1) << endl;
		}
		else
		{
			output << line << "\n";
		}
	}
	return output.str();
}

void Shader::SaveErrors(const fs::path& filename, std::string code, std::string errors)
{
	fs::path outf = (filename.parent_path() / filename.filename()).concat("_error.txt");
	std::ofstream error_out(outf);
	
	error_out << code << std::endl << errors;
	error_out.close();
}