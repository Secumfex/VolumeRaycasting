#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "TextureTools.h"

#include "Core/DebugLog.h"

namespace TextureTools {
    GLuint loadTexture(std::string fileName){

    	std::string fileString = std::string(fileName);
    	fileString = fileString.substr(fileString.find_last_of("/"));

    	std::string fileContent;
    	std::string line;

    	int width, height, bytesPerPixel;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &bytesPerPixel, 0);

        if(data == NULL){
//        	std::cout << "ERROR: Unable to open image "  << fileName << std::endl;
//        	DEBUGLOG->log("ERROR : Unable to open image " + fileName);
        	DEBUGLOG->log("ERROR : Unable to open image " + fileString);
        	  return -1;}


        // for ( unsigned int i = 0; i < width*height;i++)
        // {
        //     std::cout<< (int)data[i] << std::endl;
        //     std::cout<< (int)data[i+1] << std::endl;
        //     std::cout<< (int)data[i+2] << std::endl;
        //     std::cout<< (int)data[i+3] << std::endl;
        // }

        //create new texture
        GLuint textureHandle;
        glGenTextures(1, &textureHandle);
     
        //bind the texture
        glBindTexture(GL_TEXTURE_2D, textureHandle);
     
        //send image data to the new texture
        if (bytesPerPixel < 3) {
        	DEBUGLOG->log("ERROR : Unable to open image " + fileString);
//        	DEBUGLOG->log("ERROR : Unable to open image " + fileName);
//            std::cout << "ERROR: Unable to open image"  << fileName << std::endl;
            return -1;
        } else if (bytesPerPixel == 3){
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (bytesPerPixel == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else {
        	DEBUGLOG->log("Unknown format for bytes per pixel... Changed to \"4\"");
//            std::cout << "Unknown format for bytes per pixel... Changed to \"4\"" << std::endl;
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        //texture settings
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, true);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
//        DEBUGLOG->log("SUCCESS: image loaded from " + fileName );
        DEBUGLOG->log( "SUCCESS: image loaded from " + fileString );
//        std::cout << "SUCCESS: image loaded from " << fileName << std::endl;
        return textureHandle;
    }
}
