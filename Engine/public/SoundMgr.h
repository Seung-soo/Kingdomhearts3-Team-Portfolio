#pragma once
#include "Base.h"
#include "../../Reference/Inc/fmod/fmod.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class CSoundMgr final : public CBase
{
	MAKE_SINGLETON(CSoundMgr)
public:
	enum CHANNELID {
		// BGM
		BGM, 
		// Logo
		EFFECT, EFFECT1, 
		
		// Player - Action(6)
		PLAYER_ACTION1, PLAYER_ACTION2, PLAYER_ACTION3, PLAYER_ACTION4, PLAYER_ACTION5, PLAYER_ACTION6, 
		// Player - Attack(6)
		PLAYER_ATTACK1, PLAYER_ATTACK2, PLAYER_ATTACK3, PLAYER_ATTACK4, PLAYER_ATTACK5, PLAYER_ATTACK6,
		// Player - Attack Last & Finish(4)
		PLAYER_ATTACK_LF1, PLAYER_ATTACK_LF2, PLAYER_ATTACK_LF3, PLAYER_ATTACK_LF4,
		// Player - Damaged(6)
		PLAYER_DAMAGED1, PLAYER_DAMAGED2, PLAYER_DAMAGED3, PLAYER_DAMAGED4, PLAYER_DAMAGED5, PLAYER_DAMAGED6,
		// Player - Effect(6)
		PLAYER_EFFECT1, PLAYER_EFFECT2, PLAYER_EFFECT3, PLAYER_EFFECT4, PLAYER_EFFECT5, PLAYER_EFFECT6,

		SYSTEM_EFFECT, SYSTEM_EFFECT2, SYSTEM_EFFECT3,
		COMMANDA, COMMANDB, COMMANDC, COMMANDD, COMMANDE,
		COMMANDF, COMMANDG, COMMANDH, COMMANDI, COMMANDJ,
		COMMANDK, COMMANDL, COMMANDN, COMMANDM, COMMANDO,
		BOSS,
		UI_MONEYGET1, UI_MONEYGET2, UI_MONEYGET3, UI_MONEYGET4, UI_MONEYGET5,
		UI_MONEYGET6, UI_MONEYGET7, UI_MONEYGET8, UI_MONEYGET9, UI_MONEYGET10,
		DONALD_A, DONALD_B, DONALD_C, DONALD_D, DONALD_E,
		GOOFY_A, GOOFY_B, GOOFY_C, GOOFY_D, GOOFY_E,
		MAXCHANNEL
	};

	enum CHNNELMONSTER
	{
		MONSTER_1,	MONSTER_2,	MONSTER_3,	MONSTER_4,	MONSTER_5,
		MONSTER_6,	MONSTER_7,	MONSTER_8,	MONSTER_9,	MONSTER_10,
		MONSTER_11, MONSTER_12, MONSTER_13, MONSTER_14, MONSTER_15,
		MONSTER_16, MONSTER_17, MONSTER_18, MONSTER_19, MONSTER_20,
		MONSTER_21, MONSTER_22, MONSTER_23, MONSTER_24, MONSTER_25,
		MONSTER_26, MONSTER_27, MONSTER_28, MONSTER_29, MONSTER_30,
		MONSTERMAXCHANNEL
	};

	enum CHANNELUI {
		UI_COMMAND1, UI_COMMAND2, UI_COMMAND3, UI_COMMAND4, UI_COMMAND5,
		UI_ITEEGET1, UI_ITEEGET2, UI_ITEEGET3, UI_ITEEGET4, UI_ITEEGET5,
		UIMAXCHANNEL
	};

private:
	CSoundMgr();
	virtual ~CSoundMgr() = default;

public:
	void Initialize();

public:
	int  VolumeUp(CHANNELID eID, _float _vol);
	int  VolumeDown(CHANNELID eID, _float _vol);
	int  BGMVolumeUp(_float _vol);
	int  BGMVolumeDown(_float _vol);
	int  Pause(CHANNELID eID);
	_bool IsPlaying(CHANNELID eID);
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, _float _vol, _bool b3DSound, _fvector vPosition);
	void PlayRoofSound(TCHAR* pSoundKey, CHANNELID eID, _float _vol, _bool b3DSound, _fvector vPosition);
	void PlayBGM(TCHAR* pSoundKey, _float _vol);
	void StopSound(CHANNELID eID);
	void StopAll();
	void PauseAll(_bool bPause);

public: // UI 사운드 전용
	void PlaySound_UI(TCHAR* pSoundKey, CHANNELUI eID, _float _vol);
	void StopSound_UI(CHANNELUI eID);

public: // 몬스터 사운드 전용
	_uint	PlayMonsterRoofSound(TCHAR* pSoundKey, _float _vol, _bool b3DSound, _fvector vPosition);
	_uint	PlayMonsterSound(TCHAR* pSoundKey, _float _vol, _bool b3DSound, _fvector vPosition);
	void	StopMonsterSound(_uint iID);

private:
	float		m_volume    = SOUND_DEFAULT;
	float		m_BGMvolume = SOUND_DEFAULT;
	FMOD_BOOL	m_bool;

private:
	void LoadSoundFile();
	void LoadMonsterSoundFile();
	void LoadPlayerSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL*	m_pChannelArr[MAXCHANNEL];
	FMOD_CHANNEL*	m_pUIChannelArr[UIMAXCHANNEL];
	FMOD_CHANNEL*	m_pMonsterChannelArr[MONSTERMAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM*	m_pSystem;
	_bool			m_bPause = false;

private: // 몬스터 사운드 전용 자동 채널 찾기 변수
	_uint m_iCurMonsterSoundId = MONSTER_1;

public:
	virtual void Free() override;
};

END