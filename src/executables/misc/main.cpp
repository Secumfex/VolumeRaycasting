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

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

////////////////////// PARAMETERS /////////////////////////////
static bool s_isRotating = false;

static glm::vec4 s_color = glm::vec4(0.75, 0.74f, 0.82f, 1.0f); // far : blueish

static float s_strength = 0.1f;

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MAIN ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
static GLuint s_vectorTexture = 0;
static std::vector<float> s_vectorTexData;
static const int VECTOR_TEXTURE_SIZE = 128;
void updateVectorTexture(double elapsedTime)
{
	// first time, create it
	if (s_vectorTexture == 0)
	{
		s_vectorTexData.resize(VECTOR_TEXTURE_SIZE*VECTOR_TEXTURE_SIZE*3, 0.0);
		DEBUGLOG->log("VectorSize: ", s_vectorTexData.size());

		glGenTextures(1, &s_vectorTexture);
		glBindTexture(GL_TEXTURE_2D, s_vectorTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, VECTOR_TEXTURE_SIZE, VECTOR_TEXTURE_SIZE);	
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_vectorTexture);

		auto evaluate = [&](float t, float offsetX, float offsetY){
			float x = sin(t + offsetX) * 0.5f + 0.5f;
			float y = cos(t + offsetY) * 0.5f + 0.5f;
			float z = 0.0f;
			return glm::vec3(x,y,z);
		};

		for (int i = 0; i < VECTOR_TEXTURE_SIZE; i++)
		{
			for (int j = 0; j < VECTOR_TEXTURE_SIZE; j++)
			{
				auto vector = evaluate( (float) elapsedTime, ((float) i / (float) VECTOR_TEXTURE_SIZE) * 5.0f , ((float) j / (float) VECTOR_TEXTURE_SIZE) * 5.0f );
				s_vectorTexData[ (i * VECTOR_TEXTURE_SIZE * 3) + (j * 3 )] = vector.x;
				s_vectorTexData[ (i * VECTOR_TEXTURE_SIZE * 3) + (j * 3 + 1)] = vector.y;
				s_vectorTexData[ (i * VECTOR_TEXTURE_SIZE * 3) + (j * 3 + 2)] = vector.z;
			}
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, VECTOR_TEXTURE_SIZE, VECTOR_TEXTURE_SIZE, GL_RGB, GL_FLOAT, &s_vectorTexData[0] );
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

int main()
{
	DEBUGLOG->setAutoPrint(true);

	//////////////////////////////////////////////////////////////////////////////
	/////////////////////// INIT //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	// create window and opengl context
	auto window = generateWindow(800,800);

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

	// create Sphere
	// Sphere sphere;
	Grid sphere(10,10,0.1f,0.1f);
	// Volume sphere;

	/////////////////////// 	Renderpass     ///////////////////////////
	DEBUGLOG->log("Shader Compilation: volume uvw coords"); DEBUGLOG->indent();
	ShaderProgram shaderProgram("/modelSpace/GBuffer.vert", "/modelSpace/GBuffer.frag"); DEBUGLOG->outdent();
	shaderProgram.update("model", model);
	shaderProgram.update("view", view);
	shaderProgram.update("projection", perspective);
	shaderProgram.update("color", glm::vec4(1.0f,0.0f,0.0f,1.0f));

	DEBUGLOG->log("FrameBufferObject Creation: volume uvw coords"); DEBUGLOG->indent();
	FrameBufferObject fbo(getResolution(window).x, getResolution(window).y);
	fbo.addColorAttachments(4); DEBUGLOG->outdent(); // G-Buffer
	
	RenderPass renderPass(&shaderProgram, &fbo);
	renderPass.addEnable(GL_DEPTH_TEST);
	renderPass.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	renderPass.addRenderable(&sphere);

	ShaderProgram compShader("/screenSpace/fullscreen.vert", "/screenSpace/finalCompositing.frag");
	// ShaderProgram compShader("/screenSpace/fullscreen.vert", "/screenSpace/simpleAlphaTexture.frag");

	Quad quad;
	RenderPass compositing(&compShader, 0);
	compositing.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	compositing.addRenderable(&quad);

	// Geometry test shader
	ShaderProgram geomShader("/modelSpace/geometry.vert", "/modelSpace/simpleColor.frag", "/geometry/simpleGeom.geom");
	RenderPass geom(&geomShader, 0);
	geom.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	geom.addRenderable(&sphere);

	geomShader.update("projection", perspective);


	//////////////////////////////////////////////////////////////////////////////
	///////////////////////    GUI / USER INPUT   ////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	// Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);
    bool show_test_window = true;

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
		if (ImGui::CollapsingHeader("Geometry Shader Settings"))
    	{
    		ImGui::ColorEdit4( "color", glm::value_ptr( s_color)); // color mixed at max distance
	        ImGui::SliderFloat("strength", &s_strength, 0.0f, 2.0f); // influence of color shift
        }
        
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
		shaderProgram.update(   "view", view);
		shaderProgram.update(   "model", turntable.getRotationMatrix() * model);

		geomShader.update(   "view", view);
		geomShader.update(   "model", turntable.getRotationMatrix() * model);

		updateVectorTexture(elapsedTime);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_vectorTexture);
		geomShader.update("tex", 0);
		geomShader.update("blendColor", 1.0);
		geomShader.update("color", s_color);
		geomShader.update("strength", s_strength);
		//////////////////////////////////////////////////////////////////////////////
		
		////////////////////////////////  RENDERING //// /////////////////////////////
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, fbo.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT0)); // color
		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_2D, fbo.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT1)); // normal
		// glActiveTexture(GL_TEXTURE2);
		// glBindTexture(GL_TEXTURE_2D, fbo.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT2)); // position
		// glActiveTexture(GL_TEXTURE0);

		// compShader.update("colorMap",    0);
		// compShader.update("normalMap",   1);
		// compShader.update("positionMap", 2);

		// renderPass.render();
		// compositing.render();

		geom.render();

		ImGui::Render();
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // this is altered by ImGui::Render(), so reset it every frame
		//////////////////////////////////////////////////////////////////////////////

	});

	destroyWindow(window);

	return 0;
}