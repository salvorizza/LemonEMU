#include "pch.h"
#include "LemonEMU/lemonEMU.h"

int main(int argc, char** argv) {
	std::string rom_path = "";
	if (argc >= 2) {
		rom_path = argv[1];
	}
	LemonEMU* emulator = new LemonEMU();
	int32_t ret = emulator->Run(rom_path);
	return ret;
}
