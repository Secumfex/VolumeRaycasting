#include <iostream>

#include <Rendering/GLTools.h>
#include <Rendering/VertexArrayObjects.h>
#include <Importing/Importer.h>

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
		, GL_R16F
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
		, GL_RED
		, GL_SHORT
		, &(volumeData.data[0])
	);

	checkGLError(true);
	
	// create Cube
	Cube cube(1.0f, 1.0f, 0.5f);

	render(window, [&](double dt)
	{

	}
	);

	destroyWindow(window);

	return 0;
}