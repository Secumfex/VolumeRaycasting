#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Shader.h"

#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>

class ShaderProgram
{

public:

	/**
	 * @brief Constructor
	 * 
	 * @param vertexshader path to the vertexshader
	 * @param fragmentshader path to the fragmentshader
	 * 
	 */
	ShaderProgram(std::string vertexshader, std::string fragmentshader);

	/**
	 * @brief Constructor
	 * 
	 * @param vertexshader path to the vertexshader
	 * @param fragmentshader path to the fragmentshader
	 * @param geometryshader path to the geometryshader
	 * 
	 */
	ShaderProgram(std::string vertexshader, std::string fragmentshader, std::string geometryshader);

	/**
	 * @brief Destructor
	 * 
	 */
	~ShaderProgram();



	GLint getShaderProgramHandle(); //!< returns the program handle

	/**
	 * @brief Updates a boolean uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, bool value);
	/**
	 * @brief Updates an integer uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, int value);
	/**
	 * @brief Updates a float uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, float value);
	/**
	 * @brief Updates a double uniform variable
	 * 
	 * @param name  Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, double value);
	/**
	 * @brief Updates a 2D integer vector uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const glm::ivec2& vector);
	/**
	 * @brief Updates a 3D integer vector uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const glm::ivec3& vector);
	/**
	 * @brief Updates a 4D integer vector uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const glm::ivec4& vector);
	/**
	 * @brief Updates a 2D float vector uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const glm::vec2& vector);
	/**
	 * @brief Updates a 3D float vector uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const glm::vec3& vector);
	/**
	 * @brief Updates a 4D float vector uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const glm::vec4& vector);
	/**
	 * @brief Updates a 2x2 matrix uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const glm::mat2& matrix);
	/**
	 * @brief Updates a 3x3 matrix uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const glm::mat3& matrix);
	/**
	 * @brief Updates a 4x4 matrix uniform variable
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The value to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const glm::mat4& matrix);
	/**
	 * @brief Updates a list of 2D vector uniform variables
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The vector witch stores the values to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const std::vector<glm::vec2>& vector);
	/**
	 * @brief Updates a list of 3D vector uniform variables
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The vector witch stores the values to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const std::vector<glm::vec3>& vector);
	/**
	 * @brief Updates a list of 4D vector uniform variables
	 * 
	 * @param name 	Name of the uniform variable in GLSL
	 * @param value The vector witch stores the values to update the unform with
	 * 
	 * @return The shader program
	 */
	ShaderProgram* update(std::string name, const std::vector<glm::vec4>& vector);

	/**
	 * @brief Method to add a buffer to the shader and return the bound location
	 * 
	 * @param uniformName name of the uniform to add
	 * 
	 */
	int addBuffer(const std::string &bufferName);

	/**
	 * @brief Method to add a texture to the shader and return the bound location
	 * 
	 * @param textureName name of the texture to add
	 * @param path path to the texture
	 *
	 */
	// int addTexture(const std::string &textureName, const std::string &path);

	/**
	 * @brief Method to add a already existing texture to the shader
	 * 
	 * @param textureName name of the texture to add
	 * @param textureHandle texture handle
	 *
	 */
	void addTexture(const std::string &textureName, GLuint textureHandle);

	/**
	 * @brief Method to add a uniform to the shader and return the bound location
	 * 
	 * @param uniformName name of the uniform to add
	 * 
	 */
	int addUniform(const std::string &uniformName);

	/**
	 * @brief Method to enable the shader program
	 * 
	 */
	virtual void use();

	/**
	 * @brief Method to disable the shader program
	 * 
	 */
	void disable();


	inline std::map<std::string,int>* getUniformMap()	{return &m_uniformMap;} //!< returns the Uniformmap
	inline std::map<std::string,int>* getBufferMap()	{return &m_bufferMap;} //!< returns the Buffermap
	inline std::map<std::string,int>* getTextureMap()	{return &m_textureMap;} //!< returns the Texturemap
	
private:

	/**
	*@brief Method that reads out all uniform variables from vertex- and fragmentshader
	*@details Method gets called when creating the shaderprogram
	*/
	void readUniforms();
	
	/**
	*@brief Method that reads out all outputs of the fragmentshader
	*@details Method gets called when creating the shaderprogram
	*/
	void readOutputs(Shader& fragmentShader);

	/**
	 * @brief Method to attach a shader to the shader program
	 * 
	 * @param shader shader to attach
	 * 
	 */
	void attachShader(Shader shader);

	/**
	 * @brief Method to link the shader program and display the link status
	 * 
	 * @param shader shader to attach
	 * 
	 */
	void link();

	/**
	 * @brief Method to returns the bound location of a named uniform
	 * 
	 * @param uniform name of the uniform
	 * 
	 */
	 GLuint uniform(const std::string &uniform);

	/**
	 * @brief Method to returns the bound location of a named buffer
	 * 
	 * @param buffer name of the buffer
	 * 
	 */
	GLuint buffer(const std::string &buffer);

	/**
	 * @brief Method to returns the bound location of a named texture
	 * 
	 * @param texture name of the texture
	 * 
	 */
	GLuint texture(const std::string &texture);

	// Handle of the shader program
	GLuint m_shaderProgramHandle;

	// Number of attached shader
	int m_shaderCount;

	// Map of uniforms and their binding locations
	std::map<std::string,int> m_uniformMap;

	// Map of uniforms and their binding locations
	std::map<std::string,int> m_bufferMap;

	// Map of textures and their binding locations
	std::map<std::string,int> m_textureMap;

};

#endif // SHADER_PROGRAM_H