#ifndef ROM_LOADER_H
#define ROM_LOADER_H

//C/C++ headers
#include "../pch.h"

//Preprocessors Definitions
#define ROM_MAX_SIZE 0x1000 - 0x200 //The program has the length of 0xE00 by default but in ETI start at 0x600 so it is less

//Structs
struct ROM {
	uint8_t data[ROM_MAX_SIZE];
	uint16_t length;
};

//file destructor
class FileDeleter {
public:
	void operator()(FILE* f) {
		if (f != NULL) {
			fclose(f);
		}
	}
};

class ROMLoader {
public:
	static ROM LoadROM(const char* rom_path);
};

#endif // !ROM_LOADER_H
