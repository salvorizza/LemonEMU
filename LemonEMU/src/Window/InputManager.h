#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "SDL2/SDL.h"

class InputManager{
public:
	//Singleton
	static InputManager* m_instance;
	~InputManager();
	
	static InputManager* GetInstance();
	
	void Update();
	
	bool IsKeyDown(int keyCode);
	bool IsKeyUp(int keyCode);
	bool IsKeyPressed(int keyCode);
	
	
	void SetKey(int keyCode, bool value);
	
private:
	uint8_t m_keyboard[1024];
	uint8_t m_prevKeyboard[1024];
	
private:
	InputManager();
};

#endif
