/*******************************************
 * **** DESCRIPTION ****
 * This program demonstrates the MIP algorithm in an interactive way.
 *
 * It implements screen space volume ray-casting using the OpenGL 4.3 GLSL.
 * The open-source GUI library ImGui is used to allow for easy and quick interaction.
 * The MIP is fully parametrized through dynamic shader variables.
 * 
 * Additionally, an experimental concept of depth-based color effects is applied
 * to enhance the visual perception of depth.
 * It consists of two effects which are applied based on the displayed voxel's 
 * distance to the camera.
 * 
 * 1) The displayed grayscale value range (contrast) decreases with the distance.
 * 2) The displayed color is shifted towards colors specified for min/max distance.
 * 
 * Two data sets are provided: 
 * 1) the Stanford CTHead Volume Dataset
 * 2) a private MRT data set of a family member 
 * 
 * CODE LINES OF INTEREST 
 * Line 90 ; change used data set location
 * Line 92 : switch to MRT data set
 * Line 133: flip y-axis if data set is upside down
 * 
 * Additionally, there are some changes that may be applied to the volume ray-casting fragment shader, located in volume.frag
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
static float s_minValue = INT_MIN; // minimal value in data set; to be overwitten after import
static float s_maxValue = INT_MAX;  // maximal value in data set; to be overwitten after import

static bool  s_isRotating = false; 	// initial state for rotating animation
static float s_rayStepSize = 0.1f;  // ray sampling step size; to be overwritten after volume data import

static float s_rayParamEnd  = 1.0f; // parameter of uvw ray start in volume
static float s_rayParamStart= 0.0f; // parameter of uvw ray end   in volume

static float 	 s_colorEffectInfluence = 1.0f;
static float 	 s_contrastEffectInfluence = 0.5f;
static glm::vec4 s_maxDistColor = glm::vec4(170.0f / 255.0f, 192.0f / 255.0f, 209.0f/255.0f, 1.0f); // far : blueish
static glm::vec4 s_minDistColor = glm::vec4(255.0f / 255.0f, 156.0f / 255.0f, 156.0f/255.0f, 1.0f); // near: reddish
static int 		 s_mixMode = 2;
static const char* s_mixModeLabels[] = {"Multiply", "Add", "Subtract (experimental)"};

static int 		 s_activeModel = 1;
static int 	     s_lastTimeModel = 1;
static const char* s_models[] = {"MRT Brain", "CT Head"};

static float s_LMIP_threshold = FLT_MAX; // LMIP threshold 
static bool  s_LMIP_isEnabled = false;
static int   s_LMIP_minStepsToLocalMaximum = 3; // steps before Local Maximum is accepted

static int   s_minValThreshold = INT_MIN;
static int   s_maxValThreshold = INT_MAX;

static float s_windowingMinValue = -FLT_MAX / 2.0f;
static float s_windowingMaxValue = FLT_MAX / 2.0f;
static float s_windowingRange = FLT_MAX;

static float s_minDepthRange = 0.0f;
static float s_maxDepthRange = 1.0f;



//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MAIN ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

template <class T>
void activateVolume(VolumeData<T>& volumeData ) // set static variables
{
	DEBUGLOG->log("File Info:");
	DEBUGLOG->indent();
		DEBUGLOG->log("min value: ", volumeData.min);
		DEBUGLOG->log("max value: ", volumeData.max);
		DEBUGLOG->log("res. x   : ", volumeData.size_x);
		DEBUGLOG->log("res. y   : ", volumeData.size_y);
		DEBUGLOG->log("res. z   : ", volumeData.size_z);
	DEBUGLOG->outdent();

	// set volume specific parameters
	s_minValue = volumeData.min;
	s_maxValue = volumeData.max;
	s_rayStepSize = 1.0f / (2.0f * volumeData.size_x); // this seems a reasonable size
	s_LMIP_threshold = (float) volumeData.max;
	s_windowingMinValue = (float) volumeData.min;
	s_windowingMaxValue = (float) volumeData.max;
	s_windowingRange = s_windowingMaxValue - s_windowingMinValue;
	s_minValThreshold = volumeData.min;
	s_maxValThreshold = volumeData.max;


}

int main()
{
	DEBUGLOG->setAutoPrint(true);

	//////////////////////////////////////////////////////////////////////////////
	/////////////////////// VOLUME DATA LOADING //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	std::string file = RESOURCES_PATH;
	file += std::string( "/CTHead/CThead");

	// load data set: CT of a Head
	VolumeData<short> volumeDataCTHead = Importer::load3DData<short>(file, 256, 256, 113, 2);
	// alternative data set: MRT of a brain; comment this line in and the above out to use 
	VolumeData<short> volumeDataMRTBrain = Importer::loadBruder();

	activateVolume<short>(volumeDataCTHead);

	DEBUGLOG->log("Initial ray sampling step size: ", s_rayStepSize);

	// create window and opengl context
	auto window = generateWindow(800,800);

	// load into 3d texture
	DEBUGLOG->log("Loading Volume Data to 3D-Texture.");
	
	GLuint volumeTextureCT = loadTo3DTexture<short>(volumeDataCTHead);
	GLuint volumeTextureMRT =  loadTo3DTexture<short>(volumeDataMRTBrain);

	GLuint volumeTexture = volumeTextureCT; // startup volume data

	DEBUGLOG->log("OpenGL error state after 3D-Texture creation: ");
	DEBUGLOG->indent(); checkGLError(true); DEBUGLOG->outdent();


	//////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// RENDERING  ///////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	
	/////////////////////     Scene / View Settings     //////////////////////////
	glm::mat4 model = glm::mat4(1.0f);
	model[1] = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f); // flip y, based on data set
	glm::vec4 eye(2.5f, 0.5f, 2.5f, 1.0f);
	glm::vec4 center(0.0f,0.0f,0.0f,1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(eye), glm::vec3(center), glm::vec3(0,1,0));

	glm::mat4 perspective = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 6.0f);
	/// perspective projection is experimental; yields weird warping effects due to vertex interpolation of uv-coordinates
	// glm::mat4 perspective = glm::perspective(glm::radians(45.f), getRatio(window), 1.0f, 10.f);

	// create Volume
	Volume volume(1.0f, 1.0f, 1.26315f);

	///////////////////////     UVW Map Renderpass     ///////////////////////////
	DEBUGLOG->log("Shader Compilation: volume uvw coords"); DEBUGLOG->indent();
	ShaderProgram uvwShaderProgram("/modelSpace/volumeMVP.vert", "/modelSpace/volumeUVW.frag"); DEBUGLOG->outdent();
	uvwShaderProgram.update("model", model);
	uvwShaderProgram.update("view", view);
	uvwShaderProgram.update("projection", perspective);

	DEBUGLOG->log("FrameBufferObject Creation: volume uvw coords"); DEBUGLOG->indent();
	FrameBufferObject uvwFBO(getResolution(window).x, getResolution(window).y);
	uvwFBO.addColorAttachments(2); DEBUGLOG->outdent(); // front UVRs and back UVRs
	
	RenderPass uvwRenderPass(&uvwShaderProgram, &uvwFBO);
	uvwRenderPass.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	uvwRenderPass.addDisable(GL_DEPTH_TEST); // to prevent back fragments from being discarded
	uvwRenderPass.addEnable(GL_BLEND); // to prevent vec4(0.0) outputs from overwriting previous results
	uvwRenderPass.addRenderable(&volume);

	
	///////////////////////   Ray-Casting Renderpass    //////////////////////////
	DEBUGLOG->log("Shader Compilation: ray casting shader"); DEBUGLOG->indent();
	ShaderProgram shaderProgram("/modelSpace/volumeMVP.vert", "/modelSpace/volume.frag"); DEBUGLOG->outdent();
	shaderProgram.update("model", model);
	shaderProgram.update("view", view);
	shaderProgram.update("projection", perspective);
	
	shaderProgram.update("uStepSize", s_rayStepSize);
		
	// bind volume texture, back uvw textures, front uvws
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, volumeTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, uvwFBO.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT0)); //back uvw buffer
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, uvwFBO.getColorAttachmentTextureHandle(GL_COLOR_ATTACHMENT1)); //front uvw buffer
	glActiveTexture(GL_TEXTURE0);
	
	shaderProgram.update("volume_texture", 0); // volume texture
	shaderProgram.update("back_uvw_map",  1);
	shaderProgram.update("front_uvw_map", 2);

	// ray casting render pass
	RenderPass renderPass(&shaderProgram);
	renderPass.addClearBit(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	renderPass.addRenderable(&volume);
	renderPass.addEnable(GL_DEPTH_TEST);
	renderPass.addDisable(GL_BLEND);

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
		if (ImGui::CollapsingHeader("Depth Effect Settings"))
    	{
			ImGui::ColorEdit4( "max color", glm::value_ptr( s_maxDistColor)); // color mixed at max distance
			ImGui::ColorEdit4( "min color", glm::value_ptr( s_minDistColor)); // color mixed at min distance
	        ImGui::SliderFloat("color influence",   &s_colorEffectInfluence, 0.0f, 1.0f); 	// influence of color shift
	        ImGui::SliderFloat("contrast influence",&s_contrastEffectInfluence, 0.0f, 1.0f); // influence of contrast attenuation
        	ImGui::ListBox(    "mix mode", &s_mixMode, s_mixModeLabels, IM_ARRAYSIZE(s_mixModeLabels), 3);
        }
        if (ImGui::CollapsingHeader("LMIP Settings"))
    	{
			ImGui::SliderFloat("LMIP threshold", &s_LMIP_threshold, s_minValue, s_maxValue); // LMIP threshold
			if ( ImGui::TreeNode("experimental") )
			{
				ImGui::DragInt("LMIP min steps", &s_LMIP_minStepsToLocalMaximum, 1.0f, 0, 100);
				ImGui::TreePop();
			}
		}
		if (ImGui::CollapsingHeader("Volume Rendering Settings"))
    	{
            ImGui::Text("Parameters related to volume rendering");
            ImGui::DragFloatRange2("windowing range", &s_windowingMinValue, &s_windowingMaxValue, 5.0f, (float) s_minValue, (float) s_maxValue); // grayscale ramp boundaries
	        ImGui::DragFloatRange2("ray range",   &s_rayParamStart, &s_rayParamEnd,  0.001f, 0.0f, 1.0f);
        	ImGui::SliderFloat("ray step size",   &s_rayStepSize,  0.0001f, 0.1f, "%.5f", 2.0f);
        	
			if ( ImGui::TreeNode("experimental") )
			{
            	ImGui::Text("Constrain considered values or reduce effective depth range in frustum");
            	ImGui::DragIntRange2(  "value range", &s_minValThreshold, &s_maxValThreshold, 5, s_minValue, s_maxValue);
	    	    ImGui::DragFloatRange2("depth range", &s_minDepthRange, &s_maxDepthRange,0.001f, 0.0f, 1.0f);
				ImGui::TreePop();
			}

        }
		if (ImGui::CollapsingHeader("Experimental Settings"))
    	{
            ImGui::Text("Experimental Parameters at a glance");
	        ImGui::DragInt("LMIP min steps", &s_LMIP_minStepsToLocalMaximum, 1.0f, 0, 100);
   	    	ImGui::DragIntRange2(  "value range", &s_minValThreshold, &s_maxValThreshold, 5, s_minValue, s_maxValue);
    	    ImGui::DragFloatRange2("depth range", &s_minDepthRange, &s_maxDepthRange,0.001f, 0.0f, 1.0f);
		}
        
		ImGui::Checkbox("auto-rotate", &s_isRotating); // enable/disable rotating volume
    	ImGui::ListBox("active model", &s_activeModel, s_models, IM_ARRAYSIZE(s_models), 2);
    	if (s_lastTimeModel != s_activeModel)
    	{
    		if ( s_activeModel == 0) //MRT
    		{
				activateVolume(volumeDataMRTBrain);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, volumeTextureMRT);
				s_lastTimeModel = 0;
    		}
    		else
    		{
    			activateVolume(volumeDataCTHead);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, volumeTextureCT);
				s_lastTimeModel = 1;
    		}
    	}
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
		uvwShaderProgram.update("view", view);
		shaderProgram.update(   "model", turntable.getRotationMatrix() * model);
		uvwShaderProgram.update("model", turntable.getRotationMatrix() * model);

		/************* update color mapping parameters ******************/
		// ray start/end parameters
		shaderProgram.update("uRayParamStart", s_rayParamStart);  // ray start parameter
		shaderProgram.update("uRayParamEnd",   s_rayParamEnd);    // ray end   parameter
		shaderProgram.update("uStepSize", s_rayStepSize); 	  // ray step size
		// color mapping parameters
		shaderProgram.update("uWindowingMinVal", s_windowingMinValue); 	  // lower grayscale ramp boundary
		shaderProgram.update("uWindowingMaxVal", s_windowingMaxValue); 	  // upper grayscale ramp boundary
		shaderProgram.update("uWindowingRange",  s_windowingMaxValue - s_windowingMinValue); // full range of values in window
		shaderProgram.update("uMaxDistColor", s_maxDistColor);    // color at full distance
		shaderProgram.update("uMinDistColor", s_minDistColor);    // color at min depth
		shaderProgram.update("uMixMode", 	  s_mixMode);		  // color mixing mode
		shaderProgram.update("uColorEffectInfl",    s_colorEffectInfluence);  	// color shift effect influence
		shaderProgram.update("uContrastEffectInfl", s_contrastEffectInfluence); // contrast attenuation effect influence

		// LMIP parameter
		shaderProgram.update("uThresholdLMIP", 	s_LMIP_threshold);

		/************* update experimental  parameters ******************/
		/// experimental: LMIP 'smoothing'
		shaderProgram.update("uMinStepsLMIP", 	s_LMIP_minStepsToLocalMaximum);

		/// experimental: constrained depth range
		shaderProgram.update("uMinDepthRange",  s_minDepthRange);
		shaderProgram.update("uMaxDepthRange",  s_maxDepthRange);

		/// experimental: value thresholds; out-of-range values are ignored
		shaderProgram.update("uMinValThreshold", s_minValThreshold);
		shaderProgram.update("uMaxValThreshold", s_maxValThreshold);
		//////////////////////////////////////////////////////////////////////////////
		
		////////////////////////////////  RENDERING //// /////////////////////////////
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // this is altered by ImGui::Render(), so set it every frame
		uvwRenderPass.render();
		renderPass.render();
		ImGui::Render();
		//////////////////////////////////////////////////////////////////////////////
	});

	destroyWindow(window);

	return 0;
}