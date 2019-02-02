#include "pch.h"
#include "LemonEMU.h"

//Globls
WNDPROC g_prevWndProc;

#define SAFE_DELETE(x) if(x != nullptr) { delete x; }

//Windows stuff
LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	LONG_PTR lpUserData = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	LemonEMU* emulator = (LemonEMU*)lpUserData;
	switch(uMsg) {
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
		        case IDM_LOAD:
		        	OPENFILENAME ofn ;
					wchar_t szFile[MAX_PATH] ;
		        	ZeroMemory( &ofn , sizeof( ofn));
					ofn.lStructSize = sizeof ( ofn );
					ofn.hwndOwner = NULL  ;
					ofn.lpstrFile = szFile ;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof( szFile );
					ofn.lpstrFilter = L"Binary File (.bin)\0*.bin\0Rom File (.rom)\0*.rom\0Chip8 ROM (.ch8)\0*.ch8\0All files\0*.*\0";
					ofn.nFilterIndex =1;
					ofn.lpstrFileTitle = NULL ;
					ofn.nMaxFileTitle = 0 ;
					ofn.lpstrInitialDir=NULL ;
					ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ;
					if(GetOpenFileNameW( &ofn )){
						char path[MAX_PATH];
						wcstombs(path, ofn.lpstrFile, 500);
						ROM rom = ROMLoader::LoadROM(path);
						emulator->GetVM()->LoadROM(rom);
						emulator->PlayEmulation();
					}
		        break;
		        case IDM_QUIT:
		            PostMessage(hwnd, WM_CLOSE, 0, 0);
		        break;
		        case IDM_PLAY:
		        	emulator->PlayEmulation();
		        break;
		        case IDM_PAUSE:
		        	emulator->PauseEmulation();
		        break;
		        case IDM_STOP:
		        	emulator->StopEmulation();
		        break;
		    }
		break;	
    }
    return CallWindowProc(g_prevWndProc,hwnd,uMsg,wParam,lParam);
}


LemonEMU::LemonEMU()
	:	m_stop_emulation(true)
{}

LemonEMU::~LemonEMU(){
	delete m_timer;
	SAFE_DELETE(m_vm);
	SAFE_DELETE(m_window);
}

bool LemonEMU::Initialize(){
	m_vm = new Chip8();
	m_window = new Window("LemonEMU",640,324);
	if(!m_window->Init()){
		return false;
	}
	HWND hWnd = m_window->GetWin32Handle();
	file_menu = m_window->AddWindowMenu(hWnd,L"&File");
	emulation_menu = m_window->AddWindowMenu(hWnd,L"&Emulation");
	
	
	//File Menu
	m_window->AppendWindowMenu(file_menu,MF_STRING,L"&Load ROM",IDM_LOAD);
	m_window->AppendWindowMenu(file_menu,MF_SEPARATOR,NULL,0);
	m_window->AppendWindowMenu(file_menu,MF_STRING,L"&Quit",IDM_QUIT);
	
	//Emulation Menu
	m_window->AppendWindowMenu(emulation_menu,MF_STRING,L"&Play Emulation",IDM_PLAY);
	m_window->AppendWindowMenu(emulation_menu,MF_STRING,L"&Pause Emulation",IDM_PAUSE);
	m_window->AppendWindowMenu(emulation_menu,MF_STRING,L"&Stop Emulation",IDM_STOP);
	
	m_window->SetUserData(hWnd,-21,(LONG_PTR)this);
	g_prevWndProc = m_window->SetWindowProcedure(hWnd,(LONG_PTR)&WinProc);
	
	EnableMenuItem(emulation_menu,IDM_PLAY,MF_DISABLED);
	EnableMenuItem(emulation_menu,IDM_PAUSE,MF_DISABLED);
	EnableMenuItem(emulation_menu,IDM_STOP,MF_DISABLED);
	
	SDL_Renderer* renderer = m_window->GetWindowRenderer();
	m_display = SDL_GetWindowSurface(m_window->GetWindowHandle());
	m_window->SetClearColor(255,255,255,255);
	
	m_timer = new Timer();
	m_window->Show();
	return true;
}

int32_t LemonEMU::Run(const std::string& rom_path){
	if(!Initialize()){
		return -1;
	}
	m_timer->Start();
	if (rom_path != "") {
		ROM rom = ROMLoader::LoadROM(rom_path.c_str());
		m_vm->LoadROM(rom);
		PlayEmulation();
	}
	float accumulator = 0;
	float elapsed = 0;
	while(!m_window->IsClosed()){
		if(!m_stop_emulation){
			elapsed = m_timer->GetElapsedTime();
			m_vm->EmulateCycle();
		}
		this->HandleInput();
		if(m_vm->CanDraw()){
			Render();
		}
		m_window->Update();
		if(!m_stop_emulation && elapsed < 1000.0f/FRAME_RATE){
			SDL_Delay((uint32_t)(1000.0f/FRAME_RATE - elapsed));
		}
		
	}
	return 0;
}

void LemonEMU::HandleInput(){
	m_vm->set_key(0x1,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_1));
	m_vm->set_key(0x2,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_2));
	m_vm->set_key(0x3,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_3));
	m_vm->set_key(0xC,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_4));
	
	m_vm->set_key(0x4,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_Q));
	m_vm->set_key(0x5,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_W));
	m_vm->set_key(0x6,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_E));
	m_vm->set_key(0xD,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_R));
	
	m_vm->set_key(0x7,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_A));
	m_vm->set_key(0x8,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_S));
	m_vm->set_key(0x9,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_D));
	m_vm->set_key(0xF,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_F));
	
	m_vm->set_key(0xA,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_Z));
	m_vm->set_key(0x0,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_X));
	m_vm->set_key(0xB,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_C));
	m_vm->set_key(0xF,InputManager::GetInstance()->IsKeyPressed(SDL_SCANCODE_V));
}

void LemonEMU::Render(){
	m_window->Clear();
	int rect_width = m_window->GetWidth()/CHIP8_DISPLAY_WIDTH;
	int rect_height = m_window->GetHeight()/CHIP8_DISPLAY_HEIGHT;
	for(int y=0;y<CHIP8_DISPLAY_HEIGHT;y++){
		for(int x=0;x<CHIP8_DISPLAY_WIDTH;x++){
			SDL_Rect draw_rect = {x * rect_width,y * rect_height, rect_width, rect_height};
			uint8_t color = m_vm->get_pixel(x,y) * 255;
			SDL_FillRect(m_display,&draw_rect,SDL_MapRGB(m_display->format, color, color, color));
		}
	}
	m_window->Present();
	m_vm->SetDrawFlag(false);
}

void LemonEMU::PlayEmulation(){
	m_stop_emulation = false;
	EnableMenuItem(emulation_menu,IDM_PLAY,MF_DISABLED);
	EnableMenuItem(emulation_menu,IDM_PAUSE,MF_ENABLED);
	EnableMenuItem(emulation_menu,IDM_STOP,MF_ENABLED);
}

void LemonEMU::PauseEmulation(){
	m_stop_emulation = true;
	EnableMenuItem(emulation_menu,IDM_PLAY,MF_ENABLED);
	EnableMenuItem(emulation_menu,IDM_PAUSE,MF_DISABLED);
	EnableMenuItem(emulation_menu,IDM_STOP,MF_ENABLED);
}

void LemonEMU::StopEmulation(){
	m_vm->StopEmulation();
	m_window->Clear();
	SDL_RenderPresent(m_window->GetWindowRenderer());
	m_stop_emulation = true;
	EnableMenuItem(emulation_menu,IDM_PLAY,MF_DISABLED);
	EnableMenuItem(emulation_menu,IDM_PAUSE,MF_DISABLED);
	EnableMenuItem(emulation_menu,IDM_STOP,MF_DISABLED);
}



