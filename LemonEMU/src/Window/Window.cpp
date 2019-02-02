#include "pch.h"
#include "Window.h"

Window::Window(const std::string& _title,uint32_t _width,uint32_t _height)
	:	m_title(_title),
		m_width(_width),
		m_height(_height),
		m_initialized(false),
		m_closed(false)
{}

Window::~Window(){
	if(m_initialized){
		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_handle);
		SDL_Quit();
	}
}

bool Window::Init(){
	if (SDL_Init(SDL_INIT_EVERYTHING ) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    
    SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	int32_t screen_width = DM.w;
	int32_t screen_height = DM.h;
	
	int32_t x = screen_width/2 - m_width/2;
	int32_t y = screen_height/2 - m_height/2;
    
    m_handle = SDL_CreateWindow(m_title.c_str(),x,y,m_width,m_height,SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if(m_handle == NULL){
    	SDL_Log("Unable to create the window: %s", SDL_GetError());
    	return false;
	}
	
	m_renderer = SDL_CreateRenderer(m_handle, -1, 0);
	if(m_renderer == NULL){
    	SDL_Log("Unable to create the renderer: %s", SDL_GetError());
    	return false;
	}
	
    
    
	m_initialized = true;
	return true;
}

HWND Window::GetWin32Handle(){
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(m_handle, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;
	return hwnd;
}

WNDPROC Window::SetWindowProcedure(HWND hWnd,LONG_PTR new_proc){
	return (WNDPROC) SetWindowLongPtr(hWnd, -4, new_proc);
}

void Window::SetUserData(HWND hWnd, int flag, LONG_PTR object){
	SetWindowLongPtr(hWnd,flag,object);
}

uint32_t Window::Update(){
	InputManager::GetInstance()->Update();
	SDL_Event event;
    while (SDL_PollEvent(&event)) {
        HandleEvent(event);
    }
	return 0;
}

void Window::Clear(){
	SDL_RenderClear(m_renderer);
}

void Window::Present(){
	SDL_UpdateWindowSurface(m_handle);
}

void Window::HandleEvent(const SDL_Event& e){
	if (e.type == SDL_WINDOWEVENT) {
        switch (e.window.event) {
        	case SDL_WINDOWEVENT_CLOSE:
            	m_closed = true;
            break;
        }
    }else if (e.type == SDL_KEYDOWN){
    	InputManager::GetInstance()->SetKey(e.key.keysym.scancode,true);
	}else if (e.type == SDL_KEYUP){
		InputManager::GetInstance()->SetKey(e.key.keysym.scancode,false);;
	}
}

bool Window::IsClosed(){
	return m_closed;
}

void Window::Show(){
	SDL_ShowWindow(m_handle);
}

void Window::SetClearColor(int r, int g, int b ,int a){
	SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
}

HMENU Window::AddWindowMenu(HWND hWnd,LPCWSTR content){
	if(hMenubar == NULL){
		hMenubar = CreateMenu();
		int cy_border = GetSystemMetrics(SM_CYFRAME);
		int cy_caption = GetSystemMetrics(SM_CYCAPTION);
		RECT window_rect;
		GetWindowRect(hWnd, &window_rect);
		POINT client_top_left = { 0, 0 };
		ClientToScreen(hWnd, &client_top_left);
		int menu_height = client_top_left.y - window_rect.top - cy_caption - cy_border;
		SDL_SetWindowSize(m_handle,m_width,m_height + 20 - menu_height);
	}
    HMENU new_menu = CreateMenu();
    AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR) new_menu, content);
    SetMenu(hWnd, hMenubar);
    return new_menu;
}

void Window::AppendWindowMenu(HMENU menu,UINT type,LPCWSTR content,uint64_t id){
	AppendMenuW(menu,type,id,content);
	SetMenu(GetWin32Handle(), hMenubar);
}


