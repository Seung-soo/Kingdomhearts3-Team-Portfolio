#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CProgress_Manager final : public CBase
{
	MAKE_SINGLETON(CProgress_Manager)
public:
	enum ACT { ACT_SUMMONTUTORIAL, ACT_WHITEFADEOUT, ACT_MINIGAME, ACT_END };
public:
	CProgress_Manager();
	virtual ~CProgress_Manager() = default;

public:
	void Set_ReadyWhiteFadeOut(_bool bReadyWhiteFadeOut);
	_bool Get_ReadyWhiteFadeOut() { return m_bReadyWhiteFadeOut; }

	_bool Get_WhiteFirst() { return m_bWhiteFirst; }
public:
	void Update(_double TimeDelta);

	void CameraScene_End();

	void ToUseInterval(_bool bUseInterval, _float fInterval, ACT eAct);
	
public:	// Rapunzel 
	// 라푼젤 시작 맵 소개하면서 로고뜨는 카메라 씬
	void Rapunzel_StartLogoScene();

	// 소환수 쓰는 튜토리얼
	void Summon_Tutorial();

	// 미니게임
	_bool	Get_Minigame() { return m_bMinigameStart; }
	void	Set_Minigame(_bool _Cnt) { m_bMinigameStart = _Cnt; }
	_bool	Get_MinigameWaiting() { return m_bMinigameWaiting; }
	void	Set_MinigameWaiting(_bool _Cnt) { m_bMinigameWaiting = _Cnt; }
	_bool	Get_MinigameEnd() { return m_bMinigameEnd; }
	void	Set_MinigameEnd(_bool _Cnt) { m_bMinigameEnd = _Cnt; }
	_bool	Get_MinigameWhiteFadeOut() { return m_bMinigameWhiteFadeOut; }
	void	Set_MinigameWhiteFadeOut(_bool _Cnt) { m_bMinigameWhiteFadeOut = _Cnt; }
	_bool	Get_CrowTarget() { return m_bCrowTargetOn; }
	void	Set_CrowTarget(_bool _Cnt) { m_bCrowTargetOn = _Cnt; }


public: // 공용
	void	WhiteFadeOut(_double TimeDelta);
	void	QuestNavi_TargetSetting(_fvector vPos);
	void	Create_QuestNavi();
private:
	// 간격
	_bool	m_bUseInterval = false;
	_float	m_fInterval    = 0.f;
	ACT		m_eAct         = ACT_END;

	// 화이트페이드아웃
	_bool			m_bReadyWhiteFadeOut = false;
	_float			m_fWhiteTime         = 5.f;
	CGameObject*	m_pFade          = nullptr;
	_bool			m_bMonsterDead       = false;
	_bool			m_bWhiteFirst = false;

	/* Minigame */
	_bool			m_bMinigameStart = false;
	_bool			m_bCrowTargetOn = false;
	_bool			m_bMinigameFade = false;
	_bool			m_bMinigameEnd = false;
	_bool			m_bMinigameWhiteFadeOut = false;
	_bool			m_bMinigameWaiting = false;

	// QuestNavi
	class CUI_QusetNavi*	m_pQuestNavi = nullptr;
public:
	virtual void	Free() override;
};

END