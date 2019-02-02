#ifndef CHIP8_H
#define CHIP8_H

//C/C++ headers
#include "../pch.h"


//Additional headers
#include "ROMLoader.h"

//Preprocessors definitions
#define CHIP8_MEMORY_START				0x0000
#define CHIP8_PROGRAM_START				0x0200
#define CHIP8_MEMORY_SIZE				0x1000
#define CHIP8_VREGISTERS_N				0x0010
#define CHIP8_STACK_SIZE				0x0010
#define CHIP8_KEYBOARD_SIZE				0x0010
#define CHIP8_DISPLAY_WIDTH				0x0040
#define CHIP8_DISPLAY_HEIGHT			0x0020

#define CHIP8_INTERPRETER_MEMORY_SIZE	CHIP8_PROGRAM_START
#define CHIP8_MAX_PROGRAM_SIZE			CHIP8_MEMORY_SIZE - CHIP8_INTERPRETER_MEMORY_SIZE
#define CHIP8_DISPLAY_SIZE				CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

class Chip8;
typedef void(Chip8::*function_t)(const uint16_t& opcode);

class Chip8 {
	friend class LemonEMU;
public:
	Chip8();
	~Chip8();

	uint8_t LoadROM(const ROM& rom);
	void EmulateCycle();
	void StopEmulation();
	
	bool CanDraw() const { return m_draw_flag; } 
	void SetDrawFlag(bool value) { m_draw_flag = value; }
	uint8_t get_pixel(uint8_t x, uint8_t y);
	uint8_t* GetDisplay() { return m_display; }
	uint8_t get_key(uint8_t key) const { return m_keyboard[key]; }
	void set_key(uint8_t key,uint8_t val) {  m_keyboard[key] = val; }
	
	//Chip8 instruction set
	void system(const uint16_t& opcode);
	void jump(const uint16_t& opcode);
	void call(const uint16_t& opcode);
	void equal_condition(const uint16_t& opcode);
	void not_equal_condition(const uint16_t& opcode);
	void equal_condition_registers(const uint16_t& opcode);
	void assign(const uint16_t& opcode);
	void add_byte(const uint16_t& opcode);
	void math(const uint16_t& opcode);
	void skip_not_equal(const uint16_t& opcode);
	void mov_I(const uint16_t& opcode);
	void jump_to_add(const uint16_t& opcode);
	void random(const uint16_t& opcode);
	void draw(const uint16_t& opcode);
	void keyboard(const uint16_t& opcode);
	void utility(const uint16_t& opcode);

private:
	//Chip8 stuff
	uint8_t m_memory[CHIP8_MEMORY_SIZE];
	uint8_t m_V[CHIP8_VREGISTERS_N];
	uint16_t m_I;
	uint8_t m_delay;
	uint8_t m_sound_timer;
	uint16_t m_PC;
	uint8_t m_SP;
	uint16_t m_stack[CHIP8_STACK_SIZE];
	uint8_t m_keyboard[CHIP8_KEYBOARD_SIZE];
	uint8_t m_display[CHIP8_DISPLAY_SIZE];
	
	//utility stuff
	bool m_draw_flag;
	uint8_t m_wait_press;
	function_t m_function_map[0x10];
	
	static uint8_t s_font_set[0x50];

	void boot_strap();
	void init_chip();
	void load_program(const ROM& rom);
	void clear_screen(uint8_t val);
	void set_pixel(uint8_t x, uint8_t y, uint8_t val);
	void cpu_cycle();
	
	
	//Utilities
	void log_memory();

};



#endif
