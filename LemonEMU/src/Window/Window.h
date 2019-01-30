#ifndef WINDOW_H
#define WINDOW_H

#include "../pch.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "InputManager.h"

class Window{
public:
	Window(const std::string& _title,uint32_t _width,uint32_t _height);
	~Window();
	
	bool Init();
	uint32_t Update();
	bool IsClosed();
	void Show();
	void Clear();
	void Present();
	
	SDL_Window* GetWindowHandle() { return m_handle; }
	SDL_Renderer* GetWindowRenderer() { return m_renderer; }
	
	void SetClearColor(int r, int g, int b, int a);
	
	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }

	HWND GetWin32Handle();
	
	//Windows stuff
	WNDPROC SetWindowProcedure(HWND hWnd,LONG_PTR new_proc);
	void SetUserData(HWND hWnd, int flag, LONG_PTR object);
	HMENU AddWindowMenu(HWND hwnd,LPCWSTR content);
	void AppendWindowMenu(HMENU menu,UINT type, LPCWSTR content,uint64_t id);
	
	
private:
	void HandleEvent(const SDL_Event& e);
	
	
private:
	SDL_Window* m_handle;
	SDL_Renderer* m_renderer;
	uint32_t m_width;
	uint32_t m_height;
	std::string m_title;
	bool m_initialized;
	bool m_closed;
	
	HMENU hMenubar = NULL;
};

#endif
