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

////////////////////// PARAMETERS /////////////////////////////
static bool s_isRotating = false;

static glm::vec4 s_color = glm::vec4(0.45 * 0.3f, 0.44f * 0.3f, 0.87f * 0.3f, 1.0f); // far : blueish
static glm::vec4 s_lightPos = glm::vec4(2.0,2.0,2.0,1.0);

static float s_strength = 0.5f;

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
	glm::vec4 eye(0.0f, 0.0f, 3.0f, 1.0f);
	glm::vec4 center(0.0f,0.0f,0.0f,1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(eye), glm::vec3(center), glm::vec3(0,1,0));

	// glm::mat4 perspective = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 6.0f);
	/// perspective projection is experimental; yields weird warping effects due to vertex interpolation of uv-coordinates
	glm::mat4 perspective = glm::perspective(glm::radians(65.f), getRatio(window), 0.1f, 10.f);

	// object sizes
	float object_size = 0.25f;
	float bb_height = 1.0f;
	float bb_width = 1.0f;
	float ground_size = 3.0f;

	std::vector<Renderable* > objects;
	objects.push_back(new Sphere( 20, 40, object_size ) );
	objects.push_back(new Grid( 1,1, ground_size, ground_size, true ) );
	objects.push_back(new Grid( 1,1, bb_width, bb_height, false ) ); // origin bottom left
	objects.push_back(new Sphere( 20, 40, object_size * 1.5f ) );
	// objects.push_back(new Sphere( 20, 40, object_size * 2.0f) );
	objects.push_back(new Volume( object_size * 4.0f, object_size * 3.0f, object_size ) );
	
	// Billboard Texture
	GLuint bbTexture = TextureTools::loadTexture( RESOURCES_PATH "/neon_sign.png");

	DEBUGLOG->log("Setup: model matrices"); DEBUGLOG->indent();
	std::vector<glm::mat4 > modelMatrices;
	modelMatrices.resize(5);
	modelMatrices[0] = glm::translate( glm::mat4(1.0f), glm::vec3(0.0f,-0.25f,0.0f) ); // sphere 
	modelMatrices[1] = glm::translate( glm::mat4(1.0f), glm::vec3(0.0f,-0.5f,0.0f) ) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f,0.0,0.0) );
	modelMatrices[2] = glm::translate( glm::mat4(1.0f), glm::vec3(- 0.5f * bb_width,0.0f,0.0f) ); // billboard
	modelMatrices[3] = glm::translate( glm::mat4(1.0f), glm::vec3(0.5f, 0.4f,1.1f) ); // sphere 
	modelMatrices[4] = glm::translate( glm::mat4(1.0f), glm::vec3(-1.0f, 0.5f,-0.5f) ) * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f,1.0,0.0) ); // sphere 
	glm::mat4 model = modelMatrices[0];

	/////////////////////// 	Renderpasses     ///////////////////////////
	// regular GBuffer
	DEBUGLOG->log("Shader Compilation: GBuffer"); DEBUGLOG->indent();
	ShaderProgram shaderProgram("/modelSpace/GBuffer.vert", "/modelSpace/GBuffer.frag"); DEBUGLOG->outdent();
	shaderProgram.update("model", model);
	shaderProgram.update("view", view);
	shaderProgram.update("projection", perspective);

	DEBUGLOG->log("FrameBufferObject Creation: GBuffer"); DEBUGLOG->indent();
	FrameBufferObject fbo(getResolution(window).x, getResolution(window).y);
	FrameBufferObject::s_internalFormat  = GL_RGBA32F; // to allow arbitrary values in G-Buffer
	fbo.addColorAttachments(4); DEBUGLOG->outdent();   // G-Buffer
	FrameBufferObject::s_internalFormat  = GL_RGBA;	   // restore default

	DEBUGLOG->log("RenderPass Creation: GBuffer"); DEBUGLOG->indent();
	RenderPass renderPass(&shaderProgram, &fbo);
	renderPass.addEnable(GL_DEPTH_TEST);	
	// renderPass.addEnable(GL_BLEND);
	renderPass.setClearColor(0.0,0.0,0.0,0.0);
	renderPass.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	for (auto r : objects){renderPass.addRenderable(r);}

	// regular GBuffer compositing
	DEBUGLOG->log("Shader Compilation: GBuffer compositing"); DEBUGLOG->indent();
	ShaderProgram compShader("/screenSpace/fullscreen.vert", "/screenSpace/finalCompositing.frag"); DEBUGLOG->outdent();
	// set texture references
	compShader.addTexture("colorMap", 	 fbo.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT0));
	compShader.addTexture("normalMap", 	 fbo.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT1));
	compShader.addTexture("positionMap", fbo.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT2));

	DEBUGLOG->log("RenderPass Creation: GBuffer Compositing"); DEBUGLOG->indent();
	Quad quad;
	RenderPass compositing(&compShader, 0);
	compositing.addClearBit(GL_COLOR_BUFFER_BIT);
	compositing.setClearColor(0.25,0.25,0.35,0.0);
	// compositing.addEnable(GL_BLEND);
	compositing.addRenderable(&quad);

	// offscreen screen space reflection (billboard reflection) 
	DEBUGLOG->log("Shader Compilation: SSR"); DEBUGLOG->indent();
	ShaderProgram ssrShader("/screenSpace/fullscreen.vert", "/screenSpace/ssr.frag");	DEBUGLOG->outdent();
	ssrShader.update("view", view);

	ssrShader.update("bbModel",   modelMatrices[2]);
	ssrShader.update("bbWidth",   bb_width);
	ssrShader.update("bbHeight",  bb_height);
	ssrShader.addTexture("bbTex", bbTexture);

	ssrShader.addTexture("positionMap", fbo.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT2));
	ssrShader.addTexture("normalMap",   fbo.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT1));

	DEBUGLOG->log("FrameBufferObject Creation: SSR"); DEBUGLOG->indent();
	FrameBufferObject ssrFBO(getResolution(window).x, getResolution(window).y);
	ssrFBO.addColorAttachments(1); DEBUGLOG->outdent();

	DEBUGLOG->log("RenderPass Creation: SSR"); DEBUGLOG->indent();
	RenderPass ssrRenderPass(&ssrShader, &ssrFBO );
	ssrRenderPass.setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ssrRenderPass.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	ssrRenderPass.addRenderable(&quad);
	DEBUGLOG->outdent();

	DEBUGLOG->log("Shader Compilation: Simple Alpha Texture"); DEBUGLOG->indent();
	ShaderProgram texShader("/screenSpace/fullscreen.vert", "/screenSpace/simpleAlphaTexture.frag");	DEBUGLOG->outdent();
	texShader.addTexture("tex", ssrFBO.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT0) );

	DEBUGLOG->log("RenderPass Creation: Simple Alpha Texture"); DEBUGLOG->indent();
	RenderPass simpleTexture( &texShader, 0 );
	simpleTexture.addEnable(GL_BLEND);
	simpleTexture.addRenderable(&quad);

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

	// model matrices / texture update function
	std::function<void(Renderable*)> perRenderableFunction = [&](Renderable* r){ 
		static int i = 0;
		shaderProgram.update("model", turntable.getRotationMatrix() * modelMatrices[i]);
		shaderProgram.update("mixTexture", 0.0);

		if (i == 2) // is billboard
		{
			shaderProgram.update("mixTexture", 1.0f);
			shaderProgram.addTexture("tex", bbTexture);
		}

		i = (i+1)%modelMatrices.size();
		};
	renderPass.setPerRenderableFunction( &perRenderableFunction );

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// RENDER LOOP /////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	double elapsedTime = 0.0;
	render(window, [&](double dt)
	{
		elapsedTime += dt;
		std::string window_header = "Screen Space Reflections - " + std::to_string( 1.0 / dt ) + " FPS";
		glfwSetWindowTitle(window, window_header.c_str() );

		////////////////////////////////     GUI      ////////////////////////////////
        ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplGlfwGL3_NewFrame(); // tell ImGui a new frame is being rendered
		
		ImGui::PushItemWidth(-100);

		ImGui::ColorEdit4( "color", glm::value_ptr( s_color)); // color mixed at max distance
        ImGui::SliderFloat("strength", &s_strength, 0.0f, 2.0f); // influence of color shift
        
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
		shaderProgram.update( "color", s_color);
		shaderProgram.update( "view", view);
		ssrShader.update( "view", view);
		ssrShader.update( "strength", s_strength);
		ssrShader.update("bbModel", turntable.getRotationMatrix() * modelMatrices[2]);

		compShader.update("vLightPos", view * turntable.getRotationMatrix() * s_lightPos);
		//////////////////////////////////////////////////////////////////////////////
		
		////////////////////////////////  RENDERING //// /////////////////////////////
		renderPass.render();

		compositing.render();

		ssrRenderPass.render();

		simpleTexture.render();

		ImGui::Render();
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // this is altered by ImGui::Render(), so reset it every frame
		//////////////////////////////////////////////////////////////////////////////

	});

	destroyWindow(window);

	return 0;
}