#include "Shader.h"
   
#include "Core/DebugLog.h"

#include <iostream>
#include <sstream>
#include <fstream>

Shader::Shader(const GLuint &type)
{
        // Get the type of the shader
    switch(type)
    {
        case GL_VERTEX_SHADER:
            m_typeString = "Vertex";
            break;
        case GL_FRAGMENT_SHADER:
            m_typeString = "Fragment";
            break;
        case GL_GEOMETRY_SHADER:
            m_typeString = "Geometry";
            break;
    }
        
    // Create the vertex shader id / handle
    // Note: If you segfault here you probably don't have a valid rendering context.
    m_id = glCreateShader(type);
}

Shader::~Shader()
{

}
    
void Shader::loadFromString(const std::string &sourceString)
{
    // Keep hold of a copy of the source
    m_source = sourceString;
        
    // Get the source as a pointer to an array of characters
    const char *sourceChars = m_source.c_str();
        
    // Associate the source with the shader id
    glShaderSource(m_id, 1, &sourceChars, NULL);
}
    
void Shader::loadFromFile(const std::string &filename)
{
    std::ifstream file;
        
    file.open( filename.c_str() );
        
    if (!file.good() )
    {
		DEBUGLOG->log("Failed to open file: " + filename);
        exit(-1);
    }
    
    // Create a string stream
    std::stringstream stream;
    
    // Dump the contents of the file into it
    stream << file.rdbuf();

    // Close the file
    file.close();
        
    // Convert the StringStream into a string
    m_source = stream.str();
    
    // Get the source string as a pointer to an array of characters
    const char *sourceChars = m_source.c_str();
    
    // Associate the source with the shader id
    glShaderSource(m_id, 1, &sourceChars, NULL);
}

void Shader::compile()
{
    // Compile the shader
    glCompileShader(m_id);
    
    // Check the compilation status and report any errors
    GLint shaderStatus;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &shaderStatus);
    
    // If the shader failed to compile, display the info log and quit out
    if (shaderStatus == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(m_id, infoLogLength, NULL, strInfoLog);
        
		DEBUGLOG->log(m_typeString + " shader compilation failed: " + strInfoLog );
        delete[] strInfoLog;
        glfwTerminate();
    }
    else
    {
		DEBUGLOG->log(m_typeString + " shader compilation OK" );
    }
}
