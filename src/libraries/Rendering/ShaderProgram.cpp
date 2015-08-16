#include "ShaderProgram.h"

#include "Core/DebugLog.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram(std::string vertexshader, std::string fragmentshader) 
{
    // Initially, we have zero shaders attached to the program
	m_shaderCount = 0;

	// Generate a unique Id / handle for the shader program
	// Note: We MUST have a valid rendering context before generating
	// the m_shaderProgramHandle or it causes a segfault!
	m_shaderProgramHandle = glCreateProgram();

    //Set up fragment shader
    Shader vertexShader(GL_VERTEX_SHADER);
	vertexShader.loadFromFile(SHADERS_PATH + vertexshader);
	vertexShader.compile();

 	//Set up fragment shader
	Shader fragmentShader(GL_FRAGMENT_SHADER);
	fragmentShader.loadFromFile(SHADERS_PATH + fragmentshader);
	fragmentShader.compile();

	readOutputs(fragmentShader);

	// Set up shader program
	attachShader(vertexShader);
	attachShader(fragmentShader);
	link();
	readUniforms();
}


ShaderProgram::ShaderProgram(std::string vertexshader, std::string fragmentshader, std::string geometryshader) 
{
    
    // Initially, we have zero shaders attached to the program
	m_shaderCount = 0;

	// Generate a unique Id / handle for the shader program
	// Note: We MUST have a valid rendering context before generating
	// the m_shaderProgramHandle or it causes a segfault!
	m_shaderProgramHandle = glCreateProgram();

    Shader vertexShader(GL_VERTEX_SHADER);
	vertexShader.loadFromFile(SHADERS_PATH + vertexshader);
	vertexShader.compile();

	//Set up fragment shader
	Shader fragmentShader(GL_FRAGMENT_SHADER);
	fragmentShader.loadFromFile(SHADERS_PATH + fragmentshader);
	fragmentShader.compile();

	readOutputs(fragmentShader);

	Shader geometryShader(GL_GEOMETRY_SHADER);
	geometryShader.loadFromFile(SHADERS_PATH + geometryshader);
	geometryShader.compile();

	// Set up shader program
	attachShader(vertexShader);
	attachShader(fragmentShader);
	attachShader(geometryShader);
    link();
	readUniforms();
}

ShaderProgram::~ShaderProgram()
{
	// Delete the shader program from the graphics card memory to
	// free all the resources it's been using
	glDeleteProgram(m_shaderProgramHandle);
}

GLint ShaderProgram::getShaderProgramHandle()
{
	return m_shaderProgramHandle;
}

void ShaderProgram::readOutputs(Shader& fragmentShader)
{
	// retrieve source Code of fragmentShader
	std::string source = fragmentShader.getSource();

	//read shader line per line
	std::istringstream stream(source);
	std::string instruction;
	
	// define a state machine
	enum Outputfinding{SEARCHING, OUT, TYPENAME};
	Outputfinding outputfinder = SEARCHING;
	int outputIndex = 0;
	// read each line
	while (std::getline( stream, instruction))
	{
		std::istringstream instrStream(instruction);
		while ( std::getline( instrStream, instruction, ';')) // remove semicolons from lines
		{
			std::string word;
			std::istringstream wordStream(instruction);
			while ( std::getline( wordStream, word, ' ')) // delimit words by whitespace
			{
				
				//print("word: '" + word + "'" );
				
				switch (outputfinder)
				{
				case SEARCHING:	// last state was searching, now out was found
					if (word == "out") outputfinder = OUT;
					break;
				case OUT:		// last state was out, next word must be a type name
					if (word != "") outputfinder = TYPENAME;
					break;
				case TYPENAME:
					if (word != "") // last state was a typename, so this is the buffer name
					{
						//add to buffer list, then return to searching mode
						m_bufferMap[word] = outputIndex;
						outputIndex++;
						outputfinder = SEARCHING;
					}
					break;
				}
			}
		}
	}

	// take note that outputs are assumed to be in order of appearance. 
	// optional layout modifiers are not considered
	for (auto entry : m_bufferMap)
	{
		DEBUGLOG->log("Output " + std::to_string(entry.second) +": " + entry.first);
	}

}

void ShaderProgram::readUniforms()
{
	GLint numUniforms = -1;
	glGetProgramiv(getShaderProgramHandle(), GL_ACTIVE_UNIFORMS, &numUniforms);

	DEBUGLOG->log("Number of uniforms " +  std::to_string(numUniforms));

	for (int i = 0; i < numUniforms; i++)
	{
		//passive variables for glGetActiveUniform
		int nameLength=-1;
		int uniformSize=-1;
		GLenum type = GL_ZERO;
		//string that saves uniformName
		char uniformName[50];
		glGetActiveUniform(getShaderProgramHandle(), GLint(i), sizeof(uniformName)-1, &nameLength, &uniformSize, &type, uniformName);
		uniformName[nameLength] = 0;
		//add uniform variable to map
		m_uniformMap[uniformName] = glGetUniformLocation(getShaderProgramHandle(), uniformName);
		DEBUGLOG->log("Uniform added " + std::to_string(i) +  " : " + uniformName);
	}
}

void ShaderProgram::attachShader(Shader shader)
{	
	// Increment the number of shaders we have associated with the program
	m_shaderCount++;
	// Attach the shader to the program
	// Note: We identify the shader by its unique Id value
	glAttachShader( m_shaderProgramHandle, shader.getId());

}

void ShaderProgram::link()
{
	// If we have at least two shaders (like a vertex shader and a fragment shader)...
	if (m_shaderCount >= 2)
	{
		// Perform the linking process
		glLinkProgram(m_shaderProgramHandle);
		// Check the status
		GLint linkStatus;
		glGetProgramiv(m_shaderProgramHandle, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == GL_FALSE)
		{
			DEBUGLOG->log("Shader program linking failed.");
			glfwTerminate();
		}
		else
		{
			DEBUGLOG->log("Shader program linking OK.");
		}
	}
	else
	{
		DEBUGLOG->log("Can't link shaders - you need at least 2, but attached shader count is only: " + std::to_string(m_shaderCount));
		glfwTerminate();
	}
}

int ShaderProgram::addUniform(const std::string &uniformName)
{	
	m_uniformMap[uniformName] = glGetUniformLocation(m_shaderProgramHandle, uniformName.c_str());
	// Check to ensure that the shader contains a uniform with this name
	if (m_uniformMap[uniformName] == -1)
	{
		DEBUGLOG->log("Could not add uniform: " + uniformName + " - location returned -1!");
	}
	else
	{
		DEBUGLOG->log("Uniform " + uniformName + " bound to location: " + std::to_string(m_uniformMap[uniformName]));
	}
	
	return m_uniformMap[uniformName];
}

int ShaderProgram::addBuffer(const std::string &bufferName)
{
	m_bufferMap[bufferName] = static_cast<int>(m_bufferMap.size());
	DEBUGLOG->log("ADD BUFFER: " + bufferName + " " + std::to_string(m_bufferMap[bufferName]));
	return m_bufferMap[bufferName];
}

// int ShaderProgram::addTexture(const std::string &textureName, const std::string &path)
// {	
// 	GLuint textureHandle = Texture::load(path);
// 	m_textureMap[textureName] = textureHandle;
// 	return textureHandle;
// }

void ShaderProgram::addTexture(const std::string &textureName, GLuint textureHandle)
{	
	m_textureMap[textureName] = textureHandle;
}

GLuint ShaderProgram::uniform(const std::string &uniform)
{
	// Note: You could do this method with the single line:
	//
	// 		return m_uniformMap[uniform];
	//
	// But we're not doing that. Explanation in the attribute() method above
	// Create an iterator to look through our uniform map and try to find the named uniform
	std::map<std::string, int>::iterator it = m_uniformMap.find(uniform);
	// Found it? Great - pass it back! Didn't find it? Alert user and halt.
	if ( it != m_uniformMap.end() )
	{
		return m_uniformMap[uniform];
	}
	else
	{
		DEBUGLOG->log("Could not find uniform in shader program: " + uniform);
		return 0;
	}
}

GLuint ShaderProgram::buffer(const std::string &buffer)
{
	// Note: You could do this method with the single line:
	//
	// 		return m_uniformMap[uniform];
	//
	// But we're not doing that. Explanation in the attribute() method above
	// Create an iterator to look through our uniform map and try to find the named uniform
	std::map<std::string, int>::iterator it = m_bufferMap.find(buffer);
	// Found it? Great - pass it back! Didn't find it? Alert user and halt.
	if ( it != m_bufferMap.end() )
	{
		return m_bufferMap[buffer];
	}
	else
	{
		DEBUGLOG->log("Could not find buffer in shader program: " + buffer);
		return 0;
	}
}

GLuint ShaderProgram::texture(const std::string &texture)
{
	// Note: You could do this method with the single line:
	//
	// 		return m_uniformMap[uniform];
	//
	// But we're not doing that. Explanation in the attribute() method above
	// Create an iterator to look through our uniform map and try to find the named uniform
	std::map<std::string, int>::iterator it = m_textureMap.find(texture);
	// Found it? Great - pass it back! Didn't find it? Alert user and halt.
	if ( it != m_textureMap.end() )
	{
		return m_textureMap[texture];
	}
	else
	{
		DEBUGLOG->log("Could not find texture in shader program: " +texture);
		return 0;
	}
}

ShaderProgram* ShaderProgram::update(std::string name, bool value) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform1i(uniform(name), value);
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, int value) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform1i(uniform(name), value);
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, float value) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform1f(uniform(name), value);

	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, double value) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform1f(uniform(name), value);
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, glm::ivec2 vector) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform2iv(uniform(name), 1, glm::value_ptr(vector));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, glm::ivec3 vector) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform3iv(uniform(name), 1, glm::value_ptr(vector));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, glm::ivec4 vector) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform4iv(uniform(name), 1, glm::value_ptr(vector));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, glm::vec2 vector) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform2fv(uniform(name), 1, glm::value_ptr(vector));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, glm::vec3 vector) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform3fv(uniform(name), 1, glm::value_ptr(vector));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, glm::vec4 vector) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform4fv(uniform(name), 1, glm::value_ptr(vector));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, glm::mat2 matrix) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniformMatrix2fv(uniform(name), 1, GL_FALSE, glm::value_ptr(matrix));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, glm::mat3 matrix) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniformMatrix3fv(uniform(name), 1, GL_FALSE, glm::value_ptr(matrix));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, glm::mat4 matrix) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniformMatrix4fv(uniform(name), 1, GL_FALSE, glm::value_ptr(matrix));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, std::vector<glm::vec2> vector) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform2fv(uniform(name), sizeof(vector), glm::value_ptr((&vector[0])[0]));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, std::vector<glm::vec3> vector) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform3fv(uniform(name), sizeof(vector), glm::value_ptr((&vector[0])[0]));
	return this;
}

ShaderProgram* ShaderProgram::update(std::string name, std::vector<glm::vec4> vector) 
{
	glUseProgram(m_shaderProgramHandle);
	glUniform4fv(uniform(name), sizeof(vector), glm::value_ptr((&vector[0])[0]));
	return this;
}

void ShaderProgram::use()
{	
	int i = 0;

	// for(auto texture : m_textureMap)
	// {	
	// 	glUniform1i(uniform(texture.first), i);
	// 	glActiveTexture(GL_TEXTURE0+i);
	// 	glBindTexture(GL_TEXTURE_2D,texture.second);
	// 	i++;
	// }

	glUseProgram(m_shaderProgramHandle);
}

void ShaderProgram::disable()
{
	glUseProgram(0);
}

