#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader
{
public:

    /**
    * @brief Constructor
    * 
    * @param type type of the shader (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER)
    */
    Shader(const GLuint &type);

    /**
    * @brief Destructor
    * 
    */
    ~Shader();

    /**
    * @brief Loads the shader contents from a file
    * 
    * @param sourceString string of the shader source 
    */
    void loadFromString(const std::string &sourceString);

    /**
    * @brief Loads the the shader contents from a string
    * 
    * @param filename filename of the shader
    */
    void loadFromFile(const std::string &filename);
    
    /**
    * @brief Compile a shader and display any problems if compilation fails.
    * 
    */
    void compile();

    inline GLuint getId()           {return m_id;}  //!< Get the shader id (handle).
    inline std::string getSource()  {return m_source;} //!< get the shader source code as string.

private:
    GLuint m_id;            //!< The unique ID / handle for the shader
    std::string m_typeString; //!< String representation of the shader type (i.e. "Vertex" or such)
    std::string m_source;     //!< The shader source code (i.e. the GLSL code itself)
};

#endif //SHADER_H