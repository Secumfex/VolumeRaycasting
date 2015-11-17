#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "Rendering/VertexArrayObjects.h"
#include "Rendering/FrameBufferObject.h"
#include "Rendering/ShaderProgram.h"
#include "Rendering/Uniform.h"

#include <vector>
#include <functional>

// template<typename T>
// struct vecUniform : public std::vector< Uniform<T>* > {};

class RenderPass{
protected:
	glm::vec4 m_viewport;
	glm::vec4 m_clearColor;
	
	FrameBufferObject* m_fbo;
	ShaderProgram* m_shaderProgram;

	std::vector< Renderable* > m_renderables;

	std::vector< GLbitfield > m_clearBits;
	std::vector< GLenum > m_enable;
	std::vector< GLenum > m_disable;

	std::vector< bool > m_enableTEMP;
	std::vector< bool > m_disableTEMP;

	std::vector< Uploadable* > m_uniforms;

	std::function<void(Renderable* ) >* p_perRenderableFunction;

public:
	RenderPass(ShaderProgram* shader = 0, FrameBufferObject* fbo = 0);
	virtual ~RenderPass();

	virtual void clearBits();
	virtual void enableStates();
	virtual void disableStates();

	virtual void preRender();
	virtual void uploadUniforms();
	virtual void render();
	virtual void postRender();
	virtual void restoreStates();

	inline void setPerRenderableFunction(std::function<void(Renderable*)>* perRenderableFunction){p_perRenderableFunction = perRenderableFunction;}

	void setViewport(int x, int y, int width, int height);
	void setClearColor(float r, float g, float b, float a = 1.0f);

	void setFrameBufferObject(FrameBufferObject* fbo);
	void setShaderProgram(ShaderProgram* shaderProgram);

	void addRenderable(Renderable* renderable);
	void removeRenderable( Renderable* renderable );
	void clearRenderables();

	std::vector< Renderable* > getRenderables();

	FrameBufferObject* getFrameBufferObject();
	ShaderProgram* getShaderProgram();

	void addClearBit(GLbitfield clearBit);
	void addEnable(GLenum state);
	void addDisable(GLenum state);

	void addUniform(Uploadable* uniform);

	void removeEnable(GLenum state);
	void removeDisable(GLenum state);
	void removeClearBit(GLbitfield clearBit);
};

#endif
