#include "pch.h"
#include "InputManager.h"

InputManager* InputManager::m_instance = nullptr;

InputManager::InputManager(){
	memset(m_keyboard,0,sizeof(uint8_t) * 1024);
}

InputManager::~InputManager(){
	
}

InputManager* InputManager::GetInstance(){
	if(InputManager::m_instance == nullptr){
		InputManager::m_instance = new InputManager();
	}
	return InputManager::m_instance;
}

void InputManager::Update(){
	memcpy(m_prevKeyboard,m_keyboard,sizeof(uint8_t) * 1024);
}

bool InputManager::IsKeyDown(int keyCode){
	return m_keyboard[keyCode] && !m_prevKeyboard[keyCode];
}

bool InputManager::IsKeyUp(int keyCode){
	return !m_keyboard[keyCode] && m_prevKeyboard[keyCode];
}

bool InputManager::IsKeyPressed(int keyCode){
	return m_keyboard[keyCode];
}

void InputManager::SetKey(int keyCode,bool value){
	m_keyboard[keyCode] = (value)?1:0;
}
