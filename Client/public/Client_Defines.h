#pragma once

static const unsigned int g_iWinCX = 1280;
static const unsigned int g_iWinCY = 720;

static const unsigned int g_iEffectTexCnt = 383;

enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO,
	LEVEL_TWILIGHT,
	LEVEL_RAPUNZEL,
	LEVEL_RAPUNZEL_BOSS,
	LEVEL_FROZEN,
	LEVEL_KEYGRAVE,
	LEVEL_FINALBOSS,
	LEVEL_ENDING,
	LEVEL_MAPTOOL, LEVEL_EFFECTTOOL, LEVEL_COLLIDERTOOL, LEVEL_TEST,
	LEVEL_END };

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;
extern float		g_fGameSpeed;

namespace Client {}

using namespace Client;
#include <fstream>
#include <comdef.h> // const whcar_t* to const char*, const char* to wchar_t*
#include <tchar.h>

#define DISTANCE(VECTOR) XMVectorGetX(XMVector4Length(VECTOR))
#define GM	CGameManager::GetInstance()
#define NM	CUI_Num_Manager::GetInstance() // UI_Num_Manager
#define Inven	CInvenManager::GetInstance() // InvenManager
#define SM		CSoundManager::GetInstance() //	SoundMananger
#define PM		CProgress_Manager::GetInstance() // ProgressManager
#define MINIGAME	CMiniGame_Manager::GetInstance() // MiniGame_Manager