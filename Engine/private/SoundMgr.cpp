#include "fmod.hpp"
#include "fmod_errors.h"
#include "SoundMgr.h"
#include "tchar.h"
#include "PipeLine.h"

READY_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
{
	m_pSystem = nullptr;
}

void CSoundMgr::Initialize()
{
	FMOD_System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 72, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();
	LoadMonsterSoundFile();
	LoadPlayerSoundFile();
}

int CSoundMgr::VolumeUp(CHANNELID eID, _float _vol)
{
	if (m_volume < SOUND_MAX) {
		m_volume += _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}

int CSoundMgr::VolumeDown(CHANNELID eID, _float _vol)
{
	if (m_volume > SOUND_MIN) {
		m_volume -= _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}

int CSoundMgr::BGMVolumeUp(_float _vol)
{
	if (m_BGMvolume < SOUND_MAX) {
		m_BGMvolume += _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[BGM], m_BGMvolume);

	return 0;
}

int CSoundMgr::BGMVolumeDown(_float _vol)
{
	if (m_BGMvolume > SOUND_MIN) {
		m_BGMvolume -= _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[BGM], m_BGMvolume);

	return 0;
}

int CSoundMgr::Pause(CHANNELID eID)
{
	m_bPause = !m_bPause;
	FMOD_Channel_SetPaused(m_pChannelArr[eID], m_bPause);

	return 0;
}

_bool CSoundMgr::IsPlaying(CHANNELID eID)
{
	FMOD_BOOL playing = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &playing))
		return true;

	return false;
}

void CSoundMgr::PlaySound(TCHAR * pSoundKey, CHANNELID eID, _float _vol, _bool b3DSound, _fvector vPosition)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr,FALSE, &m_pChannelArr[eID]);

		if (true == b3DSound)
		{
			CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

			_vector vCamPosition = pPipeLine->Get_CamPosition();
			_float	fDistance = DISTANCE(vPosition - vCamPosition);
			_vol = _vol * (1.f - fDistance / 40.f);
		}

		if (_vol >= SOUND_MAX)
			_vol = 1.f;
		else if (_vol <= SOUND_MIN)
			_vol = 0.f;

		FMOD_Channel_SetVolume(m_pChannelArr[eID], _vol);
	}
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayRoofSound(TCHAR * pSoundKey, CHANNELID eID, _float _vol, _bool b3DSound, _fvector vPosition)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
		FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_NORMAL);

		if (true == b3DSound)
		{
			CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

			_vector vCamPosition = pPipeLine->Get_CamPosition();
			_float	fDistance = DISTANCE(vPosition - vCamPosition);
			_vol = _vol * (1.f - fDistance / 40.f);
		}

		if (_vol >= SOUND_MAX)
			_vol = 1.f;
		else if (_vol <= SOUND_MIN)
			_vol = 0.f;

		FMOD_Channel_SetVolume(m_pChannelArr[eID], _vol);
	}

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(TCHAR * pSoundKey, _float _vol)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[BGM], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[BGM]);
		FMOD_Channel_SetMode(m_pChannelArr[BGM], FMOD_LOOP_NORMAL);
		if (_vol >= SOUND_MAX)
			_vol = 1.f;
		else if (_vol <= SOUND_MIN)
			_vol = 0.f;
		FMOD_Channel_SetVolume(m_pChannelArr[BGM], _vol);
	}

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);

	for (int i = 0; i < UIMAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pUIChannelArr[i]);

	for (int i = 0; i < MONSTERMAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pMonsterChannelArr[i]);

	
}

void CSoundMgr::PauseAll(_bool bPause)
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_SetPaused(m_pChannelArr[i], bPause);

	for (int i = 0; i < UIMAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pUIChannelArr[i]);
}

void CSoundMgr::PlaySound_UI(TCHAR* pSoundKey, CHANNELUI eID, _float _vol)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pUIChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pUIChannelArr[eID]);
		if (_vol >= SOUND_MAX)
			_vol = 1.f;
		else if (_vol <= SOUND_MIN)
			_vol = 0.f;
		FMOD_Channel_SetVolume(m_pUIChannelArr[eID], _vol);
	}
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound_UI(CHANNELUI eID)
{
	FMOD_Channel_Stop(m_pUIChannelArr[eID]);
}

_uint CSoundMgr::PlayMonsterRoofSound(TCHAR* pSoundKey, _float _vol, _bool b3DSound, _fvector vPosition)
{
	_uint iReturnValue = -1;

	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return iReturnValue;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pMonsterChannelArr[m_iCurMonsterSoundId], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pMonsterChannelArr[m_iCurMonsterSoundId]);
		FMOD_Channel_SetMode(m_pMonsterChannelArr[m_iCurMonsterSoundId], FMOD_LOOP_NORMAL);

		if (true == b3DSound)
		{
			CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

			_vector vCamPosition = pPipeLine->Get_CamPosition();
			_float	fDistance = DISTANCE(vPosition - vCamPosition);
			_vol = _vol * (1.f - fDistance / 40.f);
		}

		if (_vol >= SOUND_MAX)
			_vol = 1.f;
		else if (_vol <= SOUND_MIN)
			_vol = 0.f;

		FMOD_Channel_SetVolume(m_pMonsterChannelArr[m_iCurMonsterSoundId], _vol);

		iReturnValue = m_iCurMonsterSoundId;

		++m_iCurMonsterSoundId;
		if (m_iCurMonsterSoundId == MONSTERMAXCHANNEL)
			m_iCurMonsterSoundId = MONSTER_1;
	}

	FMOD_System_Update(m_pSystem);

	return iReturnValue;
}

_uint CSoundMgr::PlayMonsterSound(TCHAR* pSoundKey, _float _vol, _bool b3DSound, _fvector vPosition)
{
	_uint iReturnValue = -1;

	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return iReturnValue;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pMonsterChannelArr[m_iCurMonsterSoundId], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pMonsterChannelArr[m_iCurMonsterSoundId]);

		if (true == b3DSound)
		{
			CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

			_vector vCamPosition = pPipeLine->Get_CamPosition();
			_float	fDistance = DISTANCE(vPosition - vCamPosition);
			_vol = _vol * (1.f - fDistance / 40.f);
		}

		if (_vol >= SOUND_MAX)
			_vol = 1.f;
		else if (_vol <= SOUND_MIN)
			_vol = 0.f;
		FMOD_Channel_SetVolume(m_pMonsterChannelArr[m_iCurMonsterSoundId], _vol);

		iReturnValue = m_iCurMonsterSoundId;

		++m_iCurMonsterSoundId;
		if (m_iCurMonsterSoundId == MONSTERMAXCHANNEL)
			m_iCurMonsterSoundId = MONSTER_1;
	}
	FMOD_System_Update(m_pSystem);

	return iReturnValue;
}

void CSoundMgr::StopMonsterSound(_uint iID)
{
	FMOD_Channel_Stop(m_pMonsterChannelArr[(CHNNELMONSTER)iID]);
}

void CSoundMgr::LoadSoundFile()
{
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../../Client/bin/Resources/Sound/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Client/bin/Resources/Sound/";

	char szFullPath[128] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			int iLength = (_int)strlen(szFilename) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _tfindnext64(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

void CSoundMgr::LoadMonsterSoundFile()
{
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../../Client/bin/Resources/Sound/Monster/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Client/bin/Resources/Sound/Monster/";
	char szFullPath[128] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			int iLength = (_int)strlen(szFilename) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _tfindnext64(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

void CSoundMgr::LoadPlayerSoundFile()
{
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../../Client/bin/Resources/Sound/Player/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Client/bin/Resources/Sound/Player/";
	char szFullPath[128] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			int iLength = (_int)strlen(szFilename) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _tfindnext64(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

void CSoundMgr::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
