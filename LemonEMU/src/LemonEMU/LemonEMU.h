#ifndef LEMON_EMU_H
#define LEMON_EMU_H

//Defines
#define IDM_QUIT 1
#define IDM_LOAD 2
#define IDM_PLAY 3
#define IDM_PAUSE 4
#define IDM_STOP 5

#define FRAME_RATE 200

//Additional Headers
#include "../pch.h"
#include "../Chip8/Chip8.h"
#include "../Chip8/RomLoader.h"
#include "../Window/Window.h"
#include "Timer.h"

class LemonEMU{
public:
	LemonEMU();
	~LemonEMU();
	
	int32_t Run(const std::string& rom_path);
	void StopEmulation();
	void PauseEmulation();
	void PlayEmulation();
	
	Chip8* GetVM() { return m_vm; }
	
private:
	bool Initialize();
	void HandleInput();
	void Render();
private:
	Window* m_window;
	Chip8* m_vm;
	Timer* m_timer;
	const int FRAMES_PER_SECOND = 20;
	SDL_Surface* m_display;
	bool m_stop_emulation;
	
	HMENU file_menu;
	HMENU emulation_menu;
	
};

#endif
