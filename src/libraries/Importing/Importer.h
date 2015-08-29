#ifndef IMPORTER_H
#define IMPORTER_H

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <glm/glm.hpp>

#include <Core/DebugLog.h>

#include <algorithm>

template<class T>
struct VolumeData
{
	unsigned int size_x; //!< x: left
	unsigned int size_y; //!< y: forward
	unsigned int size_z; //!< z: up

	std::vector<T> data; //!< size: x

	//std::vector<T> midSlice;

	float real_size_x; // actual step size in mm
	float real_size_y; // acutal step size in mm
	float real_size_z; // acutal step size in mm

	T min;
	T max;
};

namespace Importer {
	/**
	 * @param path to file prefix relative to resources folder file suffix is assumed to be .1 .2 .. .num_files
	 * @param size_x of slice file
	 * @param size_y of slice file
	 * @param num_files of slice files. will be loaded in ascending order
	 * @return data from files
	 */
	template<class T>
	VolumeData<T> load3DData(std::string path, unsigned size_x, unsigned size_y, unsigned int num_files, unsigned int num_bytes_per_entry = 1)
	{
		DEBUGLOG->log("Loading files with prefix :" + path);
		DEBUGLOG->log("Reading slice data...");

		VolumeData<T> result;
		result.size_x = size_x;
		result.size_y = size_y;
		result.size_z = num_files;
		result.data.clear();

		T min = SHRT_MAX;
		T max = SHRT_MIN;

		//result.midSlice.resize(size_x*size_y);

		DEBUGLOG->indent();
		for (unsigned int i = 1; i <= num_files; i++)
		{
			std::string current_file_path = path + "." + std::to_string(i);

			std::cout << ".";

//			DEBUGLOG->log("current file:" + current_file_path);

			// read file into input vector
			std::ifstream file( current_file_path.c_str(), std::ifstream::binary);	
			std::vector<char> input;

			if (file.is_open()) {
				// get length of file:
				file.seekg (0, file.end);
				int length = file.tellg();
				file.seekg (0, file.beg);

				// allocate memory:
				input.resize(length);

				// read data as a block:
				file.read( &input[0], length );

				file.close();
			}

			// create data vector for this slice
			std::vector<T> slice(size_x * size_y, 0);

			for(unsigned int j = 0 ; j < slice.size(); j++)
			{
				T val = input[num_bytes_per_entry*j];
				for (unsigned int k = 1; k < num_bytes_per_entry; k++)
				{
					val = (val << 8) + input[ num_bytes_per_entry*j + k];
				}
	    		
				slice[j] = val;
				
				//if ( i == num_files / 2)
				//{
				//	result.midSlice[j] = val;
				//	//DEBUGLOG->log("x: ", j - (j / size_x)*size_x);
				//	//DEBUGLOG->log("y: ", j / size_x);
				//	//DEBUGLOG->log("MidSlice: ", val);
				//}

				min = std::min<short>(val, min);
				max = std::max<short>(val, max);
			}

			// push slice to data vector
			result.data.insert(result.data.end(), slice.begin(), slice.end());
		}
		std::cout << std::endl;
		DEBUGLOG->outdent();

		result.min = min;		
		result.max = max;

		return result;
	}

	VolumeData<short> loadBruder();
} // namespace Importer

#endif