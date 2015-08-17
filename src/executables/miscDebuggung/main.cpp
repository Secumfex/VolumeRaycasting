#include <iostream>

#include <Rendering/GLTools.h>
#include <Rendering/VertexArrayObjects.h>
#include <Rendering/RenderPass.h>
#include <Importing/Importer.h>


#include <glm/gtc/matrix_transform.hpp>

int main()
{
	DEBUGLOG->setAutoPrint(true);
	std::cout << "Hello World."<< std::endl;

	std::string file = RESOURCES_PATH;
	file += std::string( "/CTHead/CThead");

	// load data
	VolumeData<short> volumeData = Importer::load3DData<short>(file, 256, 256, 113, 2);
	
	DEBUGLOG->log("File Info:");
	DEBUGLOG->indent();
		DEBUGLOG->log("min value: ", volumeData.min);
		DEBUGLOG->log("max value: ", volumeData.max);
	DEBUGLOG->outdent();
	// create window and opengl context
	auto window = generateWindow();

	// load into 3d texture
	GLuint volumeTexture;
	glEnable(GL_TEXTURE_3D);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &volumeTexture);
    glBindTexture(GL_TEXTURE_3D, volumeTexture);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	
	checkGLError(true);

	// allocate GPU memory
	glTexStorage3D(GL_TEXTURE_3D
		, 1
		, GL_R16I
		, volumeData.size_x
		, volumeData.size_y
		, volumeData.size_z
	);

	checkGLError(true);

	// upload data
	glTexSubImage3D(GL_TEXTURE_3D
		, 0
		, 0
		, 0
		, 0
		, volumeData.size_x
		, volumeData.size_y
		, volumeData.size_z
		, GL_RED_INTEGER
		, GL_SHORT
		, &(volumeData.data[0])
	);

	checkGLError(true);
	
	// create Volume
	Volume volume(1.0f, 1.0f, 0.5f);

	// create render pass
	ShaderProgram shaderProgram("/modelSpace/modelViewProjection.vert", "/modelSpace/volume.frag");
	shaderProgram.update("model", glm::mat4(1.0f));
	shaderProgram.update("view", glm::lookAt(glm::vec3(2,2,2), glm::vec3(0), glm::vec3(0,1,0)));
	shaderProgram.update("projection", glm::perspective(45.f, getRatio(window), 0.1f, 100.f));

	shaderProgram.update("uColor", glm::vec4(0.9f, 0.3f, 0.5f, 1.0f));
	shaderProgram.update("uRange", (float) volumeData.max - (float) volumeData.min);
	
	shaderProgram.update("tex", 0); // volume texture

	RenderPass renderPass(&shaderProgram);
	renderPass.addClearBit(GL_DEPTH_BUFFER_BIT);
	renderPass.addClearBit(GL_COLOR_BUFFER_BIT);
	renderPass.addRenderable(&volume);

	double time = 0.0f;

	render(window, [&](double dt)
	{
		time += dt;
		shaderProgram.update("uSinus", std::sin(time));

		renderPass.render();
	});

	destroyWindow(window);

	return 0;
}