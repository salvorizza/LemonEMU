#include "pch.h"
#include "LemonEMU/lemonEMU.h"

int main(int argc, char** argv) {
	LemonEMU* emulator = new LemonEMU();
	int32_t ret = emulator->Run();
	return ret;
}
