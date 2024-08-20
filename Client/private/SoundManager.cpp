#include "stdafx.h"
#include "..\public\SoundManager.h"
#include "GameInstance.h"

READY_SINGLETON(CSoundManager)

CSoundManager::CSoundManager()
{
}

void CSoundManager::Sound_Command(TCHAR * pSoundKey, _float _vol)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (0 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDA, _vol);

	if (1 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDB, _vol);


	if (2 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDC, _vol);
	
	if (3 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDD, _vol);
	
	if (4 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDE, _vol);

	if (5 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDF, _vol);

	if (6 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDG, _vol);

	if (7 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDH, _vol);

	if (8 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDI, _vol);

	if (9 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDJ, _vol);

	if (10 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDK, _vol);

	if (11 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDL, _vol);

	if (12 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDN, _vol);

	if (13 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDM, _vol);

	if (14 == m_iSoundCommandCnt)
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::COMMANDO, _vol);
	
	/* =============== */
	m_iSoundCommandCnt += 1;

	if (15 <= m_iSoundCommandCnt)
		m_iSoundCommandCnt = 0;
}

void CSoundManager::Sound_ItemGet(TCHAR * pSoundKey, _float _vol)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (0 == m_iItemGetCnt)
	{
		pGameInstance->StopSound_UI(CSoundMgr::CHANNELUI::UI_ITEEGET1);
		pGameInstance->LetsPlaySound_UI(pSoundKey, CSoundMgr::CHANNELUI::UI_ITEEGET1, _vol);
	}

	if (1 == m_iItemGetCnt)
	{
		pGameInstance->StopSound_UI(CSoundMgr::CHANNELUI::UI_ITEEGET2);
		pGameInstance->LetsPlaySound_UI(pSoundKey, CSoundMgr::CHANNELUI::UI_ITEEGET2, _vol);
	}

	if (2 == m_iItemGetCnt)
	{
		pGameInstance->StopSound_UI(CSoundMgr::CHANNELUI::UI_ITEEGET3);
		pGameInstance->LetsPlaySound_UI(pSoundKey, CSoundMgr::CHANNELUI::UI_ITEEGET3, _vol);
	}

	if (3 == m_iItemGetCnt)
	{
		pGameInstance->StopSound_UI(CSoundMgr::CHANNELUI::UI_ITEEGET4);
		pGameInstance->LetsPlaySound_UI(pSoundKey, CSoundMgr::CHANNELUI::UI_ITEEGET4, _vol);
	}

	if (4 == m_iItemGetCnt)
	{
		pGameInstance->StopSound_UI(CSoundMgr::CHANNELUI::UI_ITEEGET5);
		pGameInstance->LetsPlaySound_UI(pSoundKey, CSoundMgr::CHANNELUI::UI_ITEEGET5, _vol);
	}

	/* =============== */
	m_iItemGetCnt += 1;

	if (5 <= m_iItemGetCnt)
		m_iItemGetCnt = 0;
}

void CSoundManager::Sound_MoneyGet(TCHAR * pSoundKey, _float _vol)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (0 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET1);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET1, _vol);
	}
		
	if (1 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET2);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET2, _vol);
	}

	if (2 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET3);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET3, _vol);
	}

	if (3 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET4);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET4, _vol);
	}

	if (4 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET5);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET5, _vol);
	}

	if (5 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET6);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET6, _vol);
	}

	if (6 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET7);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET7, _vol);
	}

	if (7 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET8);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET8, _vol);
	}

	if (8 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET9);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET9, _vol);
	}

	if (9 == m_iMoneyGetCnt)
	{
		pGameInstance->StopSound(CSoundMgr::CHANNELID::UI_MONEYGET10);
		pGameInstance->LetsPlaySound(pSoundKey, CSoundMgr::CHANNELID::UI_MONEYGET10, _vol);
	}


	/* =============== */
	m_iMoneyGetCnt += 1;

	if (10 <= m_iMoneyGetCnt)
		m_iMoneyGetCnt = 0;
}

void CSoundManager::Free()
{
}
