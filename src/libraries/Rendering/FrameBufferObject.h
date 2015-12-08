#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
#include <vector>

class FrameBufferObject
{
protected:
	GLuint m_frameBufferHandle;

	int m_width;
	int m_height;

	int m_numColorAttachments;

	GLuint m_depthTextureHandle;

	std::map< GLenum, GLuint > m_colorAttachments;
	std::map<std::string, GLuint> m_textureMap;
	std::vector<GLenum > m_drawBuffers;
public:

	static GLenum s_internalFormat;
	static GLenum s_format;
	static bool s_useTexStorage2D;
	static GLenum s_type;

	FrameBufferObject(int width = 800, int height = 600);

	FrameBufferObject(std::map<std::string, int>* outputMap, int width, int height, GLint internalFormat = GL_RGBA);
	~FrameBufferObject();

	void createDepthTexture();

	GLuint createFramebufferTexture();
	void addColorAttachments(int amount);

	GLuint getColorAttachmentTextureHandle(GLenum attachment);
	void setColorAttachmentTextureHandle(GLenum attachment, GLuint textureHandle);

	GLuint getFramebufferHandle();

	int getWidth();
	int getHeight();

	const std::map<GLenum, GLuint>& getColorAttachments() const;
	void setColorAttachments(const std::map<GLenum, GLuint>& colorAttachments);
	GLuint getDepthTextureHandle() const;
	void setDepthTextureHandle(GLuint depthTextureHandle);
	const std::vector<GLenum>& getDrawBuffers() const;
	void setDrawBuffers(const std::vector<GLenum>& drawBuffers);
	void setFramebufferHandle(GLuint framebufferHandle);
	void setHeight(int height);
	int getNumColorAttachments() const;
	void setNumColorAttachments(int numColorAttachments);
	void setWidth(int width);

	void bind();
	void unbind();

	GLuint getBuffer(std::string name); //!< Get the texture handle corresponding to a certain buffer name.

	void setFrameBufferObject(const GLuint& frameBufferObjectHandle, const int& width, const int& height, const std::map<std::string, GLuint>& textureMap, GLuint depthTexture);

};

#endif