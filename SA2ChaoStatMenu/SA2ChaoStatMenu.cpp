#include "stdafx.h"
#include "SA2ModLoader.h"
#include "ChaoData.h"
#include "Trampoline.cpp"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

char ChaoMenuString[] = { 07, 'C', 'H', 'A', 'O', ' ', 'D', 'A', 'T', 'A', 0 };
char TestString[] = {'T', 'E', 'S', 'T', 0 };
DataPointer(int, PauseMenuID, 0x017472BC);
FunctionPointer(void, DisplayPauseMenu, (), 0x00440AD0);
NChaoData *displayChao;

IDirect3D9 *d3d;
void **DirectXFunctions;
void *endSceneTableEntry;
void *resetTableEntry;

void initD3D()
{
	d3d = Direct3DCreate9(32);

	HWND hWnd = find_main_window(GetCurrentProcessId());

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = 1;
	d3dpp.BackBufferHeight = 1;
	d3dpp.hDeviceWindow = hWnd;

	IDirect3DDevice9 *d3ddev;

	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, nullptr, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);

	DirectXFunctions = (void**)(*(void**)d3d);

	endSceneTableEntry = DirectXFunctions[D3D9Funcs_EndScene];
	resetTableEntry = DirectXFunctions[D3D9Funcs_Reset];
};

void ChaoMenuInputHandler()
{
	
}

static const int returnToFunction = 0x0043C0B1;
static const int GoOn = 0x0043B11F;
__declspec(naked) void ChaoMenuInput_wrapper()
{
	__asm
	{
		call ChaoMenuInputHandler
		jmp returnToFunction
	}
}
__declspec(naked) void RedirectToChaoMenuInput()
{
	__asm
	{
		je continueL
		cmp eax, 3
		jne NotThree
		jmp ChaoMenuInput_wrapper
	NotThree:
		jmp returnToFunction
	continueL:
		jmp GoOn;

	}
}

void SelectChaoMenu()
{
	if(CurrentLevel == LevelIDs_ChaoWorld)
		PauseMenuID = 3;
}
__declspec(naked) void SelectChaoMenuWrapper()
{
	__asm
	{
		call SelectChaoMenu
		jmp returnToFunction
	}
}

static const void *const DrawUITextPtr = (void*)0x0058D880;
static inline void DrawUIText(unsigned char *TextPtr, float XPos, float YPos, float TextBoxLength, float TextSize, char TextAlignment)
{
	__asm
	{
		push [TextAlignment]
		push [TextSize]
		push [TextBoxLength]
		push [YPos]
		push [XPos]
		mov ecx, [TextPtr]
		call DrawUITextPtr
	}
}

void DisplayChaoMenu()
{
	if (displayChao != nullptr)
	{

	}
	else
	{
		int t = *(int*)&endSceneTableEntry;
		std::string var = std::to_string(t);
		char char_array[20];
		strcpy_s(char_array, var.c_str());

		DrawUIText((unsigned char*)&(char_array), 250, 250, 999, 24, 0);
	}
}
void ActivateChaoMenu()
{
	if (IsNotPauseHide >= 0 && PauseMenuID < 3)
	{
		DisplayPauseMenu();
	}
	if (PauseMenuID == 3)
	{
		if((**MainCharObj2).HeldObject != NULL)
			if ((**MainCharObj2).HeldObject->MainSub == Chao_Main)
			{
				ChaoData1 ChaoDataptr = *(**MainCharObj2).HeldObject->Data1.Chao;
				displayChao = (NChaoData*)ChaoDataptr.ChaoDataBase_ptr;
			}
			else goto NoChao;
		else
		{
		NoChao:
			displayChao = nullptr;
		}
		DisplayChaoMenu();
	}
}

static const int Return1 = 0x0043D4A9;
__declspec(naked) void DisplayChaoMenu_Wrapper()
{
	__asm
	{
		call ActivateChaoMenu
		jmp Return1;
	}
}

void RenderDelegate()
{
	
}

extern "C"
{
	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		initD3D();
		//Trampoline((int)endSceneTableEntry, ((int)endSceneTableEntry) + 6, &RenderDelegate);

		WriteData((int*)0x0174A830, (int)&ChaoMenuString);
		WriteJump((int*)0x0043B119, RedirectToChaoMenuInput);
		WriteJump((int*)0x0043B1EE, SelectChaoMenuWrapper);
		WriteData((char*)0x0043B037, (char)0x8C);
		WriteJump((int*)0x0043D49B, DisplayChaoMenu_Wrapper);
	}
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

