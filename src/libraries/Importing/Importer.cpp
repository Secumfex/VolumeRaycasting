#include "Importer.h"

VolumeData<short> Importer::loadBruder()
{
	std::string path = RESOURCES_PATH +  std::string( "/Bruder/psirInt16Signed.raw");
		
	DEBUGLOG->log("Loading file: " + path);
	DEBUGLOG->log("Reading slice data...");

	VolumeData<short> result;
	result.size_x = 240;
	result.size_y = 240;
	result.size_z = 190;
	result.data.clear();

	short min = SHRT_MAX;
	short max = SHRT_MIN;

	std::string current_file_path = path;

	// read file into input vector
	std::ifstream file( current_file_path.c_str(), std::ifstream::binary);	
	std::vector<short> input;

	// load data
	if (file.is_open()) 
	{

		// get (byte) length of file:
		file.seekg (0, file.end);
		int length = file.tellg();
		file.seekg (0, file.beg);

		// allocate memory:
		input.resize(length / 2 + 1 );

		//DEBUGLOG->log("file length: ", length);
		//DEBUGLOG->log("input size : ", input.size());
		//DEBUGLOG->log("shorts to read:", 10944000);

		int i = 0;
		while ( file.good())
		{
			short val;
			file.read(reinterpret_cast<char*>(&val), sizeof val);

			input[i] = val;

			min = std::min<short>(val, min);
			max = std::max<short>(val, max);

			if (i % (result.size_x * result.size_y) == 0 )
			{ std::cout << "."; }

			i++;
		}
		std::cout << std::endl;

		file.close();
	}

	result.data.insert(result.data.end(), input.begin(), input.end());
	result.min = min;		
	result.max = max;

	return result;
}