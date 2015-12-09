/*******************************************
 * **** DESCRIPTION ****
 ****************************************/

#include <iostream>

#include <Rendering/GLTools.h>
#include <Rendering/VertexArrayObjects.h>
#include <Rendering/RenderPass.h>

#include "UI/imgui/imgui.h"
#include <UI/imguiTools.h>
#include <UI/Turntable.h>

#include <Importing/TextureTools.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

 #include "misc.hpp"

////////////////////// PARAMETERS /////////////////////////////
static bool s_isRotating = false;

static glm::vec4 s_color = glm::vec4(0.45, 0.44f, 0.87f, 1.0f); // far : blueish
static glm::vec4 s_lightPos = glm::vec4(2.0,2.0,2.0,1.0);

static float s_strength = 0.05f;
static float s_transparency = 0.05f;

const glm::vec2 WINDOW_RESOLUTION = glm::vec2(800.0f, 600.0f);
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MAIN ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int main()
{
	DEBUGLOG->setAutoPrint(true);

	//////////////////////////////////////////////////////////////////////////////
	/////////////////////// INIT //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	// create window and opengl context
	auto window = generateWindow(WINDOW_RESOLUTION.x,WINDOW_RESOLUTION.y);

	//////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// RENDERING  ///////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	
	/////////////////////     Scene / View Settings     //////////////////////////
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec4 eye(0.0f, 0.0f, 3.0f, 1.0f);
	glm::vec4 center(0.0f,0.0f,0.0f,1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(eye), glm::vec3(center), glm::vec3(0,1,0));

	// glm::mat4 perspective = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 6.0f);
	/// perspective projection is experimental; yields weird warping effects due to vertex interpolation of uv-coordinates
	glm::mat4 perspective = glm::perspective(glm::radians(65.f), getRatio(window), 0.1f, 10.f);

	// initial object size (biggest / outer object)
	float object_size = 1.0f;
	int num_objects = 4;
	
	std::vector<Renderable* > objects;
	for ( int i = 0; i < num_objects; i++)
	{
		objects.push_back(new Volume(object_size));
		// objects.push_back(new Sphere(20,40,object_size));
		object_size *= 0.75f;
	}

	/////////////////////// 	Renderpass     ///////////////////////////
	int num_depth_buffers = 4;
	int num_color_attachments = 3;

	FrameBufferObject::s_internalFormat  = GL_RGBA32F; // to allow arbitrary values in G-Buffer
	DepthPeelingBuffers depthPeelingBuffers(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y,
		num_depth_buffers,
		num_color_attachments);
	FrameBufferObject::s_internalFormat  = GL_RGBA;	   // restore default

	DEBUGLOG->log("Shader Compilation: depth peeling shader"); DEBUGLOG->indent();
	ShaderProgram depthPeelingShader("/modelSpace/GBuffer.vert", "/modelSpace/dpGBuffer.frag");
	depthPeelingShader.update("model", model);
	depthPeelingShader.update("view", view);
	depthPeelingShader.update("projection", perspective);
	depthPeelingShader.update("color", s_color);
	depthPeelingShader.update("texResolution", WINDOW_RESOLUTION);
	DEBUGLOG->outdent();

	DEBUGLOG->log("FrameBufferObject Creation: depth peeling framebuffers"); DEBUGLOG->indent();
	RenderPass depthPeel(&depthPeelingShader, depthPeelingBuffers.m_fbos[0] );
	depthPeel.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	depthPeel.addEnable(GL_DEPTH_TEST);
	DEBUGLOG->outdent();

	// add objects to depth peeling render pass
	for (auto r : objects )
	{
		depthPeel.addRenderable(r);
	}

	// depth peeling compositing shader
	DEBUGLOG->log("Shader Compilation: depth peeling compositing"); DEBUGLOG->indent();
	ShaderProgram compShader("/screenSpace/fullscreen.vert", "/screenSpace/dpCompositing.frag");
	DEBUGLOG->outdent();

	Quad quad;
	RenderPass compositing(&compShader, 0);
	compositing.setClearColor(0.5,0.5,0.5,1.0);
	compositing.addEnable(GL_BLEND);
	compositing.addRenderable(&quad);

	//////////////////////////////////////////////////////////////////////////////
	///////////////////////    GUI / USER INPUT   ////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	// Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

	Turntable turntable;
	double old_x;
    double old_y;
	glfwGetCursorPos(window, &old_x, &old_y);
	
	auto cursorPosCB = [&](double x, double y)
	{
		ImGuiIO& io = ImGui::GetIO();
		if ( io.WantCaptureMouse )
		{ return; } // ImGUI is handling this

		double d_x = x - old_x;
		double d_y = y - old_y;

		if ( turntable.getDragActive() )
		{
			turntable.dragBy(d_x, d_y, view);
		}

		old_x = x;
		old_y = y;
	};

	auto mouseButtonCB = [&](int b, int a, int m)
	{
		if (b == GLFW_MOUSE_BUTTON_LEFT && a == GLFW_PRESS)
		{
			turntable.setDragActive(true);
		}
		if (b == GLFW_MOUSE_BUTTON_LEFT && a == GLFW_RELEASE)
		{
			turntable.setDragActive(false);
		}

		ImGui_ImplGlfwGL3_MouseButtonCallback(window, b, a, m);
	};

	auto keyboardCB = [&](int k, int s, int a, int m)
	{
		if (a == GLFW_RELEASE) {return;} 
		switch (k)
		{
			case GLFW_KEY_W:
				eye += glm::inverse(view)    * glm::vec4(0.0f,0.0f,-0.1f,0.0f);
				center += glm::inverse(view) * glm::vec4(0.0f,0.0f,-0.1f,0.0f);
				break;
			case GLFW_KEY_A:
				eye += glm::inverse(view)	 * glm::vec4(-0.1f,0.0f,0.0f,0.0f);
				center += glm::inverse(view) * glm::vec4(-0.1f,0.0f,0.0f,0.0f);
				break;
			case GLFW_KEY_S:
				eye += glm::inverse(view)    * glm::vec4(0.0f,0.0f,0.1f,0.0f);
				center += glm::inverse(view) * glm::vec4(0.0f,0.0f,0.1f,0.0f);
				break;
			case GLFW_KEY_D:
				eye += glm::inverse(view)    * glm::vec4(0.1f,0.0f,0.0f,0.0f);
				center += glm::inverse(view) * glm::vec4(0.1f,0.0f,0.0f,0.0f);
				break;
			default:
				break;
		}
		ImGui_ImplGlfwGL3_KeyCallback(window,k,s,a,m);
	};

	setCursorPosCallback(window, cursorPosCB);
	setMouseButtonCallback(window, mouseButtonCB);
	setKeyCallback(window, keyboardCB);

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// RENDER LOOP /////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	double elapsedTime = 0.0;
	render(window, [&](double dt)
	{
		elapsedTime += dt;
		std::string window_header = "Volume Renderer - " + std::to_string( 1.0 / dt ) + " FPS";
		glfwSetWindowTitle(window, window_header.c_str() );

		////////////////////////////////     GUI      ////////////////////////////////
        ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplGlfwGL3_NewFrame(); // tell ImGui a new frame is being rendered
		
		ImGui::PushItemWidth(-100);

		ImGui::ColorEdit4( "color", glm::value_ptr( s_color)); // color mixed at max distance
        ImGui::SliderFloat("strength", &s_strength, 0.0f, 2.0f); // influence of color shift
        ImGui::SliderFloat("transparency", &s_transparency, 0.0f, 1.0f); // influence of color shift
        
		ImGui::Checkbox("auto-rotate", &s_isRotating); // enable/disable rotating volume
		ImGui::PopItemWidth();

        //////////////////////////////////////////////////////////////////////////////

		///////////////////////////// MATRIX UPDATING ///////////////////////////////
		if (s_isRotating) // update view matrix
		{
			model = glm::rotate(glm::mat4(1.0f), (float) dt, glm::vec3(0.0f, 1.0f, 0.0f) ) * model;
		}

		view = glm::lookAt(glm::vec3(eye), glm::vec3(center), glm::vec3(0.0f, 1.0f, 0.0f));
		//////////////////////////////////////////////////////////////////////////////
				
		////////////////////////  SHADER / UNIFORM UPDATING //////////////////////////
		// update view related uniforms
		depthPeelingShader.update( "color", s_color);
		depthPeelingShader.update( "view", view);
		depthPeelingShader.update( "model", turntable.getRotationMatrix() * model);

		compShader.update("vLightPos", view * turntable.getRotationMatrix() * s_lightPos);
		compShader.update("strength", s_strength);
		compShader.update("transparency", s_transparency);
		//////////////////////////////////////////////////////////////////////////////
		
		////////////////////////////////  RENDERING //// /////////////////////////////
		for ( int i = 0; i < num_depth_buffers; i++)
		{
			// current fbo to fill
			FrameBufferObject* currentFBO = depthPeelingBuffers.m_fbos[i];
			
			// depth texture from last pass exists
			if ( i > 0)
			{
				FrameBufferObject* beforeFBO = depthPeelingBuffers.m_fbos[i-1];
				depthPeelingShader.addTexture("lastDepth", beforeFBO->getDepthTextureHandle() );
			}
			else
			{
				depthPeelingShader.addTexture("lastDepth", currentFBO->getDepthTextureHandle() );
			}
			depthPeelingShader.update("peel_level",i);

			depthPeel.setFrameBufferObject(currentFBO);

			// render current fbo
			depthPeel.render();
		}

		// render depth peeling compositing
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (int i = num_depth_buffers - 1; i >= 0; i--) // render from back to front
		{
			if ( i == num_depth_buffers - 1)
			{
				compositing.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			}
			else if ( i == num_depth_buffers - 2)
			{
				compositing.removeClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			}

			auto buffer = depthPeelingBuffers.m_fbos[i];

			// set texture references, execute compositing
			compShader.addTexture("colorMap", buffer->getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT0));
			compShader.addTexture("normalMap", buffer->getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT1));
			compShader.addTexture("positionMap", buffer->getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT2));

			compositing.render();
		}

		ImGui::Render();
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // this is altered by ImGui::Render(), so reset it every frame
		//////////////////////////////////////////////////////////////////////////////

	});

	destroyWindow(window);

	return 0;
}