#include "Rendering/FrameBufferObject.h"

#include "Core/DebugLog.h"

GLenum FrameBufferObject::s_internalFormat  = GL_RGBA;	// default
GLenum FrameBufferObject::s_format 			= GL_RGBA;	// default
GLenum FrameBufferObject::s_type 			= GL_UNSIGNED_BYTE;	// default
bool FrameBufferObject::s_useTexStorage2D	= false;	// default

FrameBufferObject::FrameBufferObject(int width, int height)
{
	glGenFramebuffers(1, &m_frameBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);

	m_width = width;
	m_height = height;

	createDepthTexture();

	m_numColorAttachments = 0;
}

void FrameBufferObject::createDepthTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);

	glGenTextures(1, &m_depthTextureHandle);
	glBindTexture(GL_TEXTURE_2D, m_depthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureHandle, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBufferObject::createFramebufferTexture()
{
	GLuint textureHandle;
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	if ( s_useTexStorage2D )
	{
		// for testing purposes
		glTexStorage2D(GL_TEXTURE_2D, 1, s_internalFormat, m_width, m_height);	

		//TODO what else must be used to ensure same behaviour as TexImage2D ?
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, s_internalFormat, m_width, m_height, 0, s_format, s_type, 0);	
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureHandle;
}

void FrameBufferObject::addColorAttachments(int amount)
{
	int maxColorAttachments;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	if ( m_numColorAttachments + amount <=  maxColorAttachments)
	{
//		DEBUGLOG->log("max color attachments: ", maxColorAttachments);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);

		DEBUGLOG->log("Creating Color Attachments: ", amount);
		DEBUGLOG->indent();
		for (int i = 0; i < amount; i ++)
		{
			GLuint textureHandle = createFramebufferTexture();
			
			glBindTexture(GL_TEXTURE_2D, textureHandle);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_numColorAttachments + i, GL_TEXTURE_2D, textureHandle, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
			
			m_colorAttachments[GL_COLOR_ATTACHMENT0 + m_numColorAttachments + i] = textureHandle;
			m_drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + m_numColorAttachments + i);
		}
		DEBUGLOG->outdent();

		m_numColorAttachments = m_colorAttachments.size();
		glDrawBuffers(m_drawBuffers.size(), &m_drawBuffers[0]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void FrameBufferObject::setColorAttachmentTextureHandle( GLenum attachment, GLuint textureHandle )
{
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	int width, height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &height);

	if (  width != m_width || height != m_height )
	{
		DEBUGLOG->log("ERROR : size of texture differs from frame buffer size");
		return;
	}
	if ( m_colorAttachments.find( attachment ) != m_colorAttachments.end() )
	{
		GLuint oldAttachment = m_colorAttachments[ attachment ];
		DEBUGLOG->log("WARNING : remember to delete the old texture handle", oldAttachment);

		m_colorAttachments[ attachment ] = textureHandle;
		glBindFramebuffer( GL_FRAMEBUFFER, m_frameBufferHandle);

		glBindTexture(GL_TEXTURE_2D, textureHandle);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, textureHandle, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		DEBUGLOG->log("ERROR : specified color attachment does not exist");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBufferObject::getColorAttachmentTextureHandle(GLenum attachment)
{
	if ( m_colorAttachments.find(attachment) != m_colorAttachments.end())
	{
		return m_colorAttachments[attachment];
	}
	else{
		return 0;
	}
}

GLuint FrameBufferObject::getFramebufferHandle()
{
	return m_frameBufferHandle;
}

int FrameBufferObject::getWidth()
{
	return m_width;
}

const std::map<GLenum, GLuint>& FrameBufferObject::getColorAttachments() const {
	return m_colorAttachments;
}

void FrameBufferObject::setColorAttachments(
		const std::map<GLenum, GLuint>& colorAttachments) {
	m_colorAttachments = colorAttachments;
}

GLuint FrameBufferObject::getDepthTextureHandle() const {
	return m_depthTextureHandle;
}

void FrameBufferObject::setDepthTextureHandle(GLuint depthTextureHandle) {
	m_depthTextureHandle = depthTextureHandle;
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureHandle, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const std::vector<GLenum>& FrameBufferObject::getDrawBuffers() const {
	return m_drawBuffers;
}

void FrameBufferObject::setDrawBuffers(const std::vector<GLenum>& drawBuffers) {
	m_drawBuffers = drawBuffers;
}

void FrameBufferObject::setFramebufferHandle(GLuint framebufferHandle) {
	m_frameBufferHandle = framebufferHandle;
}

void FrameBufferObject::setHeight(int height) {
	m_height = height;
}

int FrameBufferObject::getNumColorAttachments() const {
	return m_numColorAttachments;
}

void FrameBufferObject::setNumColorAttachments(int numColorAttachments) {
	m_numColorAttachments = numColorAttachments;
}

FrameBufferObject::~FrameBufferObject() {
	// TODO free OpenGL textures etc.
}

void FrameBufferObject::setWidth(int width) {
	m_width = width;
}

int FrameBufferObject::getHeight()
{
	return m_height;
}

FrameBufferObject::FrameBufferObject(std::map<std::string, int>* outputMap, int width, int height, GLint internalFormat) 
	: m_width(width), m_height(height)
{
	//Generate FBO
	glGenFramebuffers(1, &m_frameBufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);

    //Generate color textures
    int size = outputMap->size();
    std::vector<GLuint> drawBufferHandles(size);

	glActiveTexture(GL_TEXTURE0);
	int i = 0;
    for (auto e : *outputMap) 
    {	
    	GLuint handle;
    	glGenTextures(1, &handle);
	    glBindTexture(GL_TEXTURE_2D, handle);
	    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, s_format, s_type, 0);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	    GLuint currentAttachment = GL_COLOR_ATTACHMENT0 + static_cast<unsigned int>(i); //e.second;

	    glFramebufferTexture2D(GL_FRAMEBUFFER, currentAttachment, GL_TEXTURE_2D, handle, 0);

    	m_textureMap[e.first] = handle;
	    //drawBufferHandles[e.second] = currentAttachment;
	    drawBufferHandles[i] = currentAttachment;
	    m_colorAttachments[currentAttachment] = handle;
	    i++;
    }

    glDrawBuffers(size, &drawBufferHandles[0]);

	glGenTextures( 1, &m_depthTextureHandle);
	glBindTexture( GL_TEXTURE_2D, m_depthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, m_width, m_height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureHandle, 0);

	// Any errors while generating fbo ?
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DEBUGLOG->log("ERROR: Unable to create FBO!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
}

void FrameBufferObject::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);
	glViewport( 0, 0, m_width, m_height);
}

void FrameBufferObject::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBufferObject::getBuffer(std::string name) {
	
	if (m_textureMap.find(name) != m_textureMap.end())
	{
		return m_textureMap[name]; // safe access to map
	}
	return 0; // buffer does not exist
}

void FrameBufferObject::setFrameBufferObject( const GLuint& frameBufferHandle, const int& width, const int& height, const std::map<std::string, GLuint>& textureMap, GLuint depthTexture ) 
{
	this->m_frameBufferHandle = frameBufferHandle;
	m_width = width;
	m_height = height;
	m_textureMap = textureMap;
	m_depthTextureHandle = depthTexture;
}