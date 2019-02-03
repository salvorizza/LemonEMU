#include "pch.h"
#include "Chip8.h"

uint8_t Chip8::s_font_set[0x50] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

#pragma region Public Methods



Chip8::Chip8()
	:m_draw_flag(false),
	m_wait_press(0xFF),
	m_function_map{
		&Chip8::system,
		&Chip8::jump,
		&Chip8::call,
		&Chip8::equal_condition,
		&Chip8::not_equal_condition,
		&Chip8::equal_condition_registers,
		&Chip8::assign,
		&Chip8::add_byte,
		&Chip8::math,
		&Chip8::skip_not_equal,
		&Chip8::mov_I,
		&Chip8::jump_to_add,
		&Chip8::random,
		&Chip8::draw,
		&Chip8::keyboard,
		&Chip8::utility
	}
{
	
	this->boot_strap();
}

Chip8::~Chip8()
{
}

uint8_t Chip8::LoadROM(const ROM& rom)
{
	init_chip();
	load_program(rom);
	return 0;
}

void Chip8::EmulateCycle(){
	cpu_cycle();
}

void Chip8::UpdateTimers()
{
	update_timers();
}

void Chip8::StopEmulation(){
	init_chip();
}

#pragma endregion

#pragma region Private Methods

uint8_t Chip8::get_pixel(uint8_t x, uint8_t y)
{
	return m_display[y * CHIP8_DISPLAY_WIDTH + x];
}


void Chip8::boot_strap()
{
	memcpy(m_memory,s_font_set,80);
}

void Chip8::init_chip()
{
	memset(&m_memory[CHIP8_PROGRAM_START], 0, CHIP8_MAX_PROGRAM_SIZE);
	memset(m_stack, 0, sizeof(uint16_t) * CHIP8_STACK_SIZE);
	memset(m_V, 0, sizeof(uint16_t) * CHIP8_VREGISTERS_N);
	clear_screen(0);
	m_PC = CHIP8_PROGRAM_START;
	m_SP = 0;
}

void Chip8::load_program(const ROM& rom)
{
	memcpy(&m_memory[CHIP8_PROGRAM_START], rom.data, rom.length);
	//log_memory();
}

void Chip8::clear_screen(uint8_t val)
{
	memset(m_display, val, CHIP8_DISPLAY_SIZE);
}

void Chip8::set_pixel(uint8_t x, uint8_t y, uint8_t val)
{
	m_display[y * CHIP8_DISPLAY_WIDTH + x] = val;
}

void Chip8::cpu_cycle()
{
	if (m_wait_press != 0xFF) {
		uint8_t pressed = 0x0;
		for (int i = 0; i < CHIP8_KEYBOARD_SIZE; i++) {
			if (m_keyboard[i]) {
				pressed = 0x1;
				m_V[m_wait_press] = i;
				break;
			}
		}
		if (pressed == 0x0) {
			return;
		}
		else {
			m_wait_press = 0xFF;
		}
	}
	uint16_t opcode = m_memory[m_PC] << 8 | m_memory[m_PC + 1];
	uint8_t code = opcode >> 12;
	switch (code) {
		case 0x0:
			system(opcode);
			break;
		case 0x1:
			jump(opcode);
			break;
		case 0x2:
			call(opcode);
			break;
		case 0x3:
			equal_condition(opcode);
			break;
		case 0x4:
			not_equal_condition(opcode);
			break;
		case 0x5:
			equal_condition_registers(opcode);
			break;
		case 0x6:
			assign(opcode);
			break;
		case 0x7:
			add_byte(opcode);
			break;
		case 0x8:
			math(opcode);
			break;
		case 0x9:
			skip_not_equal(opcode);
			break;
		case 0xA:
			mov_I(opcode);
			break;
		case 0xB:
			jump_to_add(opcode);
			break;
		case 0xC:
			random(opcode);
			break;
		case 0xD:
			draw(opcode);
			break;
		case 0xE:
			keyboard(opcode);
			break;
		case 0xF:
			utility(opcode);
			break;

	}
	//(this->*m_function_map[code])(opcode);	
}

void Chip8::update_timers()
{
	if (m_delay > 0) m_delay--;

	if (m_sound_timer > 0) m_sound_timer--;

	if (m_sound_timer > 0) std::cout << '\a';
}

void Chip8::log_memory(){
	for(uint16_t i=0;i<64;i++){
		for(uint16_t j=0;j<64;j++){
			printf("0x%hhX ",m_memory[i * 64 + j]);
		}
		printf("\n");
	}
}

#pragma endregion


#pragma region Chip8 Instruction Set

void Chip8::system(const uint16_t& opcode){
	switch(opcode & 0x00FF){
		case 0x00E0:{
			clear_screen(0);
			SetDrawFlag(true);
			break;
		}
		case 0x00EE:{
			m_PC = m_stack[--m_SP];
			break;
		}
	}
	m_PC += 2;
}

void Chip8::jump(const uint16_t& opcode){
	uint16_t addr = opcode & 0x0FFF;
	m_PC = addr;
}

void Chip8::call(const uint16_t& opcode){
	uint16_t addr = opcode & 0x0FFF;
	m_stack[m_SP++] = m_PC;
	m_PC = addr;
}

void Chip8::equal_condition(const uint16_t& opcode){
	uint8_t kk = opcode & 0xFF;
	uint8_t x = (opcode >> 8) & 0x0F;
	if(m_V[x] == kk){
		m_PC += 2;
	}
	m_PC += 2;
}

void Chip8::not_equal_condition(const uint16_t& opcode){
	uint8_t kk = opcode & 0xFF;
	uint8_t x = (opcode >> 8) & 0x0F;
	if(m_V[x] != kk){
		m_PC += 2;
	}
	m_PC += 2;
}

void Chip8::equal_condition_registers(const uint16_t& opcode){
	uint8_t x = (opcode >> 8) & 0x0F;
	uint8_t y = (opcode & 0x00F0) >> 4;
	if(m_V[x] == m_V[y]){
		m_PC += 2;
	}
	m_PC += 2;
}

void Chip8::assign(const uint16_t& opcode){
	uint8_t x = (opcode >> 8) & 0x0F;
	uint8_t kk = opcode & 0xFF;
	m_V[x] = kk;
	m_PC += 2;
}

void Chip8::add_byte(const uint16_t& opcode){
	uint8_t kk = opcode & 0xFF;
	uint8_t x = (opcode >> 8) & 0x0F;
	m_V[x] += kk;
	m_PC += 2;
}

void Chip8::math(const uint16_t& opcode){
	uint8_t x = (opcode >> 8) & 0x0F;
	uint8_t y = (opcode & 0x00F0) >> 4;
	switch(opcode & 0x000F){
		case 0x0000:{
			m_V[x] = m_V[y];
			break;
		}
		case 0x0001:{
			m_V[x] |= m_V[y];
			break;
		}
		case 0x0002:{
			m_V[x] &= m_V[y];
			break;
		}
		case 0x0003:{
			m_V[x] ^= m_V[y];
			break;
		}
		case 0x0004:{
			m_V[x] += m_V[y];
			m_V[0xF] = (m_V[x] > 0xFF)?1:0;
			break;
		}
		case 0x0005:{
			m_V[0xF] = (m_V[x] > m_V[y])?1:0;
			m_V[x] -= m_V[y];
			break;
		}
		case 0x0006:{
			m_V[0x0F] = m_V[x] & 0x1;
			m_V[x] /= 2;
			break;
		}
		case 0x0007:{
			m_V[0xF] = (m_V[y] > m_V[x]) ? 1 : 0;
			m_V[x] = m_V[y] - m_V[x];
			break;
		}
		case 0x000E:{
			m_V[0x0F] = m_V[x] & 0x80;
			m_V[x] *= 2;
			break;
		}
	}
	m_PC += 2;
}

void Chip8::skip_not_equal(const uint16_t& opcode){
	uint8_t x = (opcode >> 8) & 0x0F;
	uint8_t y = (opcode & 0x00F0) >> 4;
	if(m_V[x]!= m_V[y]){
		m_PC += 2;
	}
	m_PC += 2;
}

void Chip8::mov_I(const uint16_t& opcode){
	uint16_t addr = opcode & 0x0FFF;
	m_I = addr;
	m_PC += 2;
}

void Chip8::jump_to_add(const uint16_t& opcode){
	uint16_t addr = opcode & 0x0FFF;
	m_PC = addr + m_V[0x0];
}

void Chip8::random(const uint16_t& opcode){
	uint8_t kk = opcode & 0xFF;
	uint8_t x = (opcode >> 8) & 0x0F;
	m_V[x] = (uint8_t)(rand()%256) & kk;
	m_PC += 2;
}

void Chip8::draw(const uint16_t& opcode){
	uint8_t nibble = opcode & 0x0F;
	uint8_t x = (opcode >> 8) & 0x0F;
	uint8_t y = (opcode & 0x00F0) >> 4;
	m_V[0xF] = 0;
	for(uint8_t byte_scanner = 0;byte_scanner < nibble;byte_scanner++){
		uint8_t pixel = m_memory[m_I + byte_scanner];
		for(uint8_t bit_scanner = 0;bit_scanner < 8;bit_scanner++){
			if((pixel & (0x80 >> bit_scanner)) != 0x0){
				if(get_pixel(m_V[x] + bit_scanner,m_V[y] + byte_scanner) == 1){
					m_V[0xF] = 1;
				}
				m_display[(m_V[y] + byte_scanner) * CHIP8_DISPLAY_WIDTH + (m_V[x] + bit_scanner)] ^= 1;
			}
		}
	}
	SetDrawFlag(true);
	m_PC += 2;
}

void Chip8::keyboard(const uint16_t& opcode){
	uint8_t x = (opcode >> 8) & 0x0F;
	switch(opcode & 0x00FF){
		case 0x009E:{
			if(m_keyboard[m_V[x]] == 1){
				m_PC += 2;
			}
			break;
		}
		case 0x00A1:{
			if(m_keyboard[m_V[x]] == 0){
				m_PC += 2;
			}
			break;
		}
	}
	m_PC += 2;
}

void Chip8::utility(const uint16_t& opcode){
	uint8_t x = (opcode >> 8) & 0x0F;
	switch(opcode & 0x00FF){
		case 0x0007:{
			m_V[x] = m_delay;
			break;
		}
		case 0x000A:{
			m_wait_press = x;
			break;
		}
		case 0x0015:{
			m_delay = m_V[x];
			break;
		}
		case 0x0018:{
			m_sound_timer = m_V[x];
			break;
		}
		case 0x001E:{
			m_I += m_V[x];
			break;
		}
		case 0x0029:{
			m_I = m_V[x] * 0x5;
			break;
		}
		case 0x0033:{
			m_memory[m_I + 0x00] = m_V[x] / 0x64;
			m_memory[m_I + 0x01] = (m_V[x] / 0xA) % 0xA;
			m_memory[m_I + 0x02] = (m_V[x] % 0x64) % 0xA;
			break;
		}
		case 0x0055:{
			memcpy(&m_memory[m_I],m_V,x+0x1);
			break;
		}
		case 0x0065:{
			memcpy(m_V,&m_memory[m_I],x+0x1);
			break;
		}
	}
	m_PC += 2;
}

#pragma endregion
