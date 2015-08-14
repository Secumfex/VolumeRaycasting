#ifndef IMPORTER_H
#define IMPORTER_H

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <glm/glm.hpp>

#include <Core/DebugLog.h>

template<typename T>
struct VolumeData
{
	unsigned int size_x; //!< x: left
	unsigned int size_y; //!< y: forward
	unsigned int size_z; //!< z: up

	std::vector<T> data; //!< size: x

	float real_size_x; // actual step size in mm
	float real_size_y; // acutal step size in mm
	float real_size_z; // acutal step size in mm
};

namespace Importer {
	/**
	 * @param path to file prefix relative to resources folder file suffix is assumed to be .1 .2 .. .num_files
	 * @param size_x of slice file
	 * @param size_y of slice file
	 * @param num_files of slice files. will be loaded in ascending order
	 * @return data from files
	 */
	template<typename T>
	VolumeData<T> load3DData(std::string path, unsigned size_x, unsigned size_y, unsigned int num_files, unsigned int num_bytes_per_entry = 1)
	{

	std::string current_file_path = path + ".1";
	
	std::ifstream file( current_file_path.c_str(), ios::in|ios::binary);
	
	std::vector<char> input(1024,0);

	if (file.is_open())
	{
	    file.seekg(0, ios::beg);
	    file.getline(&input[0], 1024);
	}

    std::stringstream stream;    
    stream << file.rdbuf();

    // Close the file
    file.close();
        
    // Convert the StringStream into a string
    std::string fileStr = stream.str();

    std::cout << fileStr << std::endl;
	VolumeData<T> result;
    
    return result;
	}
} // namespace Importer

#endif