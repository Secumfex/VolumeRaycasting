#include <iostream>

#include <Importing/Importer.h>

int main()
{

	std::string file = RESOURCES_PATH;
	file += std::string( "/CTHead/CThead");

	VolumeData<int> data = Importer::load3DData<int>(file , 256, 256, 113, 2);
	
	short _exit;
	std::cin >> _exit;

	return 0;
}