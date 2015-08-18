#include <iostream>

#include <Rendering/GLTools.h>
#include <Rendering/VertexArrayObjects.h>
#include <Rendering/RenderPass.h>

#include "UI/imgui/imgui.h"
#include <UI/imguiTools.h>

#include <glm/gtc/matrix_transform.hpp>

int main()
{
	DEBUGLOG->setAutoPrint(true);

	//////////////////////////////////////////////////////////////////////////////
	/////////////////////// VOLUME DATA LOADING //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	std::string file = RESOURCES_PATH;
	file += std::string( "/CTHead/CThead");

	// load data
//	VolumeData<short> volumeData = Importer::load3DData<short>(file, 256, 256, 113, 2);
	VolumeData<short> volumeData = Importer::loadBruder();

	DEBUGLOG->log("File Info:");
	DEBUGLOG->indent();
		DEBUGLOG->log("min value: ", volumeData.min);
		DEBUGLOG->log("max value: ", volumeData.max);
	DEBUGLOG->outdent();
	// create window and opengl context
	auto window = generateWindow(600,600);

	// load into 3d texture
	DEBUGLOG->log("Loading Volume Data to 3D-Texture.");
	GLuint volumeTexture = loadTo3DTexture<short>(volumeData);

	DEBUGLOG->log("OpenGL error state after 3D-Texture creation: ");
	DEBUGLOG->indent(); checkGLError(true); DEBUGLOG->outdent();


	//////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// RENDERING  ///////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	
	// scene/view settings
	glm::mat4 model = glm::mat4(1.0f);
//	model[1] = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f); // flip y
	glm::mat4 view = glm::lookAt(glm::vec3(2,2,2), glm::vec3(0), glm::vec3(0,1,0));
	// glm::mat4 perspective = glm::perspective(45.f, getRatio(window), 0.1f, 100.f);
	glm::mat4 perspective = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 10.0f);

	// create Volume
	Volume volume(1.0f, 1.0f, 1.26315f);

	// create renderpass for UV maps
	DEBUGLOG->log("Shader Compilation: volume uvw coords"); DEBUGLOG->indent();
	ShaderProgram uvwShaderProgram("/modelSpace/volumeMVP.vert", "/modelSpace/volumeUVW.frag"); DEBUGLOG->outdent();
	uvwShaderProgram.update("model", model);
	uvwShaderProgram.update("view", view);
	uvwShaderProgram.update("projection", perspective);

	DEBUGLOG->log("FrameBufferObject Creation: volume uvw coords"); DEBUGLOG->indent();
	FrameBufferObject uvwFBO(getResolution(window).x, getResolution(window).y);
	uvwFBO.addColorAttachments(2); DEBUGLOG->outdent(); // front UVRs and back UVRs
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderPass uvwRenderPass(&uvwShaderProgram, &uvwFBO);
	uvwRenderPass.addClearBit(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	uvwRenderPass.addDisable(GL_DEPTH_TEST);
	uvwRenderPass.addEnable(GL_BLEND);
	uvwRenderPass.addRenderable(&volume);

	// create ray casting shader and render pass
	DEBUGLOG->log("Shader Compilation: ray casting shader"); DEBUGLOG->indent();
	ShaderProgram shaderProgram("/modelSpace/volumeMVP.vert", "/modelSpace/volume.frag"); DEBUGLOG->outdent();
	shaderProgram.update("model", model);
	shaderProgram.update("view", view);
	shaderProgram.update("projection", perspective);
	
	shaderProgram.update("uMinVal", (float) volumeData.min);
	shaderProgram.update("uRange", (float) volumeData.max - (float) volumeData.min);

	//shaderProgram.update("uMinVal", (float) 0);
	//shaderProgram.update("uRange", (float) 5972;
	shaderProgram.update("uStepSize", 1.0f / (2.0f * volumeData.size_x));
		
	// bind volume texture, front uvws, back uvw textures
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, volumeTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, uvwFBO.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT0));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, uvwFBO.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT1));
	glActiveTexture(GL_TEXTURE0);
	
	shaderProgram.update("volume_texture", 0); // volume texture
	shaderProgram.update("back_uvw_map",  1);
	shaderProgram.update("front_uvw_map", 2);

	// ray casting render pass
	RenderPass renderPass(&shaderProgram);
	renderPass.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	renderPass.addRenderable(&volume);

	//////////////////////////////////////////////////////////////////////////////
	////////////////////////////////     GUI      ////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	// Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);
    bool show_test_window = true;

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// RENDER LOOP /////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	double time = 0.0;
	render(window, [&](double dt)
	{
		////////////////////////////////     GUI      ////////////////////////////////
		static float f = volumeData.min;
        ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::SliderFloat("float", &f, volumeData.min, volumeData.max);
        //////////////////////////////////////////////////////////////////////////////
		
		////////////////////////////////  SHADER //// ////////////////////////////////
		time += dt;
		glm::vec4 eye = glm::rotate(glm::mat4(), (float) time, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(2.5f, 0.5f, 2.5f, 1.0f);
		view = glm::lookAt(glm::vec3(eye), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// update view direction
		shaderProgram.update("view", view);
		uvwShaderProgram.update("view", view);

		shaderProgram.update("uMinVal", f);
		shaderProgram.update("uRange", volumeData.max - f);

		// render front and back uvr positions
		uvwRenderPass.render();

		renderPass.render();

		ImGui::Render();
	});

	destroyWindow(window);

	return 0;
}