#include "pch.h"
#include "ROMLoader.h"


ROM ROMLoader::LoadROM(const char* rom_path)
{
	auto file = std::unique_ptr<FILE, FileDeleter>(fopen(rom_path, "rb"));
	fseek(file.get(), 0, SEEK_END);
	long rom_size = ftell(file.get());
	fseek(file.get(), 0, SEEK_SET);
	ROM rom = {};
	if (rom_size <= ROM_MAX_SIZE) {
		fread(&rom.data, rom_size, 1, file.get());
		rom.length = (uint16_t)rom_size;
	}
	return rom;
}
