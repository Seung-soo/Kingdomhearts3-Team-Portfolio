#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Engine)
class CMonster;
class CGameObject;
class CRenderer;
END

BEGIN(Client)
class CPlayer;

enum MOVE_DIR { DIR_FORWARD, DIR_BACKWARD, DIR_LEFT, DIR_RIGHT, DIR_END };

class CGameManager : public CBase
{
	MAKE_SINGLETON(CGameManager)
private:
	CGameManager();
	virtual ~CGameManager() = default;
public:
	void	Set_Renderer(CRenderer* pRenderer) { m_pRenderer = pRenderer; }
	CRenderer* Get_Renderer() { return m_pRenderer; }

	void	Set_MoveOnToNextLevel(_bool bMoveOnToNextLevel) {
		m_bMoveOnToNextLevel = bMoveOnToNextLevel;
	}
	_bool	Get_MoveOnToNextLevel() { return m_bMoveOnToNextLevel; }

	void	Set_NextLevel(LEVEL eNextLevel) { m_eNextLevel = eNextLevel; }
	LEVEL	Get_NextLevel() { return m_eNextLevel; }

	void	Set_CurrentLevel(LEVEL eCurrentLevel) { m_eCurrentLevel = eCurrentLevel; }
	LEVEL	Get_CurrentLevel() { return m_eCurrentLevel; }

	void	Set_GameOver(_bool bGameOver) { m_bGameOver = bGameOver; }
	_bool	Get_GameOver() { return m_bGameOver; }

	_float Get_Gravity() { return m_fGravity; }
	void Set_Gravity(_float fGravity) { m_fGravity = fGravity; }

	_float Get_GravityMag() { return m_fGravity_Magnification; }
	void Set_GravityMag(_float fGravityMagnification) { 
		m_fGravity_Magnification = fGravityMagnification;
		m_fGravity *= m_fGravity_Magnification;
	}

	//	몬스터 리스트 반환
	list<CMonster*>*	Get_MonstersList();
	/* UI ===================================== */

	_int	Get_ItemCnt() { return m_iItemCnt; }
	void	Set_ItemCnt(_int _Cnt) { m_iItemCnt += _Cnt; }



	// 카메라
	_bool	Get_CameraSceneEnd() { return m_bCameraSceneEnd; }
	void	Set_CamaeraSceneEnd(_bool bCamaeraSceneEnd) { m_bCameraSceneEnd = bCamaeraSceneEnd; }

	_float	Get_PlayStartInterval() { return m_fPlayStartInterval; }
	void	Set_PlayStartInterval(_float fPlayStartInterval) { m_fPlayStartInterval = fPlayStartInterval; }

	_bool	Get_AppearTutorial() { return m_bAppearTutorial; }
	void	Set_AppearTutorial(_bool bAppearTutorial) { m_bAppearTutorial = bAppearTutorial; }

public:
	void	Update(_double TimeDelta);

	//	컨테이너에 몬스터 추가
	HRESULT Add_Monster(CMonster* pMonster);

	//	모든 몬스터 삭제(컨테이너에서 비움 + 들어있는 모든 몬스터를 Dead처리)
	void	Release_Monsters();

	/* UI ===================================== */
	/* Camera_Move */
	_bool	Get_bCameraMove() { return m_bCameraMove; }
	void	Set_bCameraMove(_bool _Cnt) { m_bCameraMove = _Cnt; }

	_bool	Get_bItem() { return m_bItemGet; }
	void	Set_bItem(_bool _Get) { m_bItemGet = _Get; }

	_double	Get_Item_Time() { return m_dItem_Time; }
	void	Set_Item_Time(_double _Time) { m_dItem_Time = _Time; }

	_bool	Get_Item_PosCnt() { return m_bItem_PosCnt; }
	void	Set_Item_PosCnt(_bool _Cnt) { m_bItem_PosCnt = _Cnt; }

	_bool	Get_First_Frame() { return m_bFirst_Frame; }
	void	Set_First_Frame(_bool _First) { m_bFirst_Frame = _First; }
	_int	Get_First_Frame_Num() { return m_iFirst_Frame_Num; }
	void	Set_First_Frame_Num(_int _First) { m_iFirst_Frame_Num = _First; }

	_bool	Get_Second_Frame() { return m_bSecond_Frame; }
	void	Set_Second_Frame(_bool _Second) { m_bSecond_Frame = _Second; }
	_int	Get_Second_Frame_Num() { return m_iSecond_Frame_Num; }
	void	Set_Second_Frame_Num(_int _Second) { m_iSecond_Frame_Num = _Second; }

	_bool	Get_Item_Frame() { return m_bItem_Frame; }
	void	Set_Item_Frame(_bool _Item) { m_bItem_Frame = _Item; }
	_int	Get_Item_Frame_Num() { return m_iItem_Frame_Num; }
	void	Set_Item_Frame_Num(_int _Item) { m_iItem_Frame_Num = _Item; }

	/* UI_FriendIcon */
	_bool	Get_bFriendIcon() { return m_bFriendIcon; }
	void	Set_bFriendIcon(_bool _IconRender) { m_bFriendIcon = _IconRender; }

	_int	Get_iFriendIcon_Num() { return m_iFriendIcon_Num; }
	void	Set_iFriendIcon_Num(_int _Num) { m_iFriendIcon_Num = _Num; }

	/* UI_BackSimbol*/
	_int	Get_SimbolCnt() { return m_iSimbolCnt; }
	void	Set_SimbolCnt(_int _Cnt) { m_iSimbolCnt = _Cnt; }
	void	Math_SimbolCnt(_int _Cnt) { m_iSimbolCnt += _Cnt; }

	_int	Get_GCnt() { return m_iGCnt; }
	void	Set_GCnt(_int _Cnt) { m_iGCnt = _Cnt; }
	void	Math_GCnt(_int _Cnt)
	{ 
		if (3 <= m_iGCnt)
		{
			m_iGCnt = 0;
		}
		else
		{
			m_iGCnt += _Cnt;
		}
	}

	/* UI_Logo */
	_bool	Get_bLoadingStart() { return m_bLoadingStart; }
	void	Set_bLoadingStart(_bool _Start) { m_bLoadingStart = _Start; }

	/* UI_Menu */
	_bool	Get_MenuSet() { return m_bMenuSet; }
	void	Set_MenuSet(_bool _Cnt) { m_bMenuSet = _Cnt; }

	_int	Get_MenuStart() { return m_iMenuStart; }
	void	Set_MenuStart(_int	_Cnt) { m_iMenuStart = _Cnt; }

	_int	Get_MenuPatternCnt() { return m_iMenuPatternCnt; }
	void	Set_MenuPatternCnt(_int _Cnt) { m_iMenuPatternCnt = _Cnt; }

	_int	Get_MenuIconCnt() { return m_iMenuIconCnt; }
	void	Set_MenuIconCnt(_int _Cnt) { m_iMenuIconCnt = _Cnt; }
	void	Math_MenuIconCnt(_int _Cnt) { m_iMenuIconCnt += _Cnt; }

	_bool	Get_InvenOn() { return bInvenOn; }
	void	Set_InvenOn(_bool _Cnt) { bInvenOn = _Cnt; }
	
	/* UI_BOSS */
	_int	Get_BossHPCnt() { return m_iBossHPCnt; }
	void	Set_BossHpCnt(_int _Cnt) { m_iBossHPCnt = _Cnt; }
	void	Math_BossHpCnt(_int _Cnt) { m_iBossHPCnt += _Cnt; }

	_bool	Get_UIRender() { return m_bUIRender; }
	void	Set_UIRender(_bool _Cnt) { m_bUIRender = _Cnt; }

	/* UI_BOSSNAME */
	_bool	Get_BossName() { return m_bBossNameRender; }
	void	Set_BossName(_bool _Cnt) { m_bBossNameRender = _Cnt; }


	/* Donald and Goofy */
	_bool	Get_Slot_On_Donald() { return bSlot_On_Donald; }
	void	Set_Slot_On_Donald(_bool _Cnt) { bSlot_On_Donald = _Cnt; }
	_bool	Get_Slot_On_Goofy() { return bSlot_On_Goofy; }
	void	Set_Slot_On_Goofy(_bool _Cnt) { bSlot_On_Goofy = _Cnt; }
	_bool	Get_Rage_Gauge() { return bRage_Gauge; }
	void	Set_Rage_Gauge(_bool _Cnt) { bRage_Gauge = _Cnt; }

	/* MiniMap */
	_bool	Get_Render_MiniMap() { return bRender_MiniMap; }
	void	Set_Render_MiniMap(_bool _Cnt) { bRender_MiniMap = _Cnt; }

	/* MiniGame */
	_bool	Get_Render_Minigame() { return bRender_Minigame; }
	void	Set_Render_Minigame(_bool _Cnt) { bRender_Minigame = _Cnt; }
	_int	Get_Minigame_Score() { return iMinigame_Score; }
	void	Set_Minigame_Score(_int _Cnt) { iMinigame_Score = _Cnt; }
	void	Math_Minigame_Score(_int _Cnt) 
	{ 
		if (0 >= iMinigame_Score)
			iMinigame_Score = 0;

		iMinigame_Score += _Cnt; 
	}


	/* Effect */
	size_t				Get_EffectMeshTag_Size() { return m_EffectModelTag.size(); }
	vector<_tchar*>*	Get_EffectMeshTag_Vector() { return &m_EffectModelTag; }

	/* Navigation */
	_bool Get_IgnoreNavigation() { return m_bIgnoreNavigation; }
	void Set_IgnoreNavigation(_bool bIgnore) { m_bIgnoreNavigation = bIgnore; }

	/* KeyBlade Lock */
	_bool Get_KeyBladeLock() { return m_bKeybladeLock; }
	void Set_KeyBladeLock(_bool bLock) { m_bKeybladeLock = bLock; }

	void Set_MonsterStun();
	void Set_MonsterSlow();

	_bool* Get_IgnoreNavigationPointer() { return &m_bIgnoreNavigation; }


	// 카메라, 플레이어
	class CCamera_Perspective*	Get_Camera();
	void						Set_Camera(class CCamera_Perspective* pCamera);

	CPlayer*					Get_Player();
	void						Set_Player(CPlayer* pPlayer);

	// 트리거가 발동되었는지 판단
	void						Set_OnTrigger(_uint iIndex, _bool bOnTrigger) { m_bOnTrigger[iIndex] = bOnTrigger; }
	_bool						Get_OnTrigger(_uint iIndex) { return m_bOnTrigger[iIndex]; }
public:
	//	이펙트 매쉬 불러오기
	HRESULT Ready_Prototype_EffectMeshes(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	//	몬스터 네비게이션 무시
	void IgnoreMonsterNavigation(_bool bOption);

	//	트리거 로드
	HRESULT Loading_Triggers(LEVEL eLeve, const _tchar* pFilePath);

	//	플레이 속도 조절
	HRESULT PlaySpeedControl(_float fSpeed);

	//	타격감 플레이 속도 조절
	void HitSpeedControl(_float fSpeed, _float fTime, _float fPower);
	void HitSpeedUpdate(_double TimeDelta);

	//	레디얼블러 사용
	HRESULT Use_RadialBlur(_bool bUse, _float fMaxBlurDist, _float fMaxTime, _float fDownTime, _float fRadialBlurMaxDistTime);

	HRESULT HitBlood();

private:
	_bool	m_bMoveOnToNextLevel = false;

	LEVEL	m_eNextLevel = LEVEL_END;
	LEVEL	m_eCurrentLevel = LEVEL_END;

	_bool	m_bGameOver = false;

	_float	m_fGravity = 9.8f;
	_float	m_fGravity_Magnification = 1.f;

private:	// 몬스터 관리
	list<CMonster*>	m_Monsters;
	/* UI ===================================== */

	/* Camera_Move */
	_bool	m_bCameraMove = false;

	// Get_item =============
	_int	m_iItemCnt = 0;
	_bool	m_bItemGet = false;
	_bool	m_bItem_PosCnt = false;

	_double	m_dItem_Time = 0.0;
	
	/* UI_Frame */
	_bool	m_bFirst_Frame = false;
	_int	m_iFirst_Frame_Num = 0;

	_bool	m_bSecond_Frame = false;
	_int	m_iSecond_Frame_Num = 0;

	_bool	m_bItem_Frame = false;
	_int	m_iItem_Frame_Num = 0;

	/* UI_FriendIcon */
	_bool	m_bFriendIcon = false;
	_int	m_iFriendIcon_Num = 0;

	/* UI_BackSimbol*/
	_int	m_iSimbolCnt = 0;
	_int	m_iGCnt = 0;

	/* UI_Logo */
	_bool	m_bLoadingStart = false;

	/* UI_Menu */
	_int	iCom_Texture_Set = 0;

	_bool	m_bMenuSet = true;

	_int	m_iMenuStart = 0;
	_int	m_iMenuPatternCnt = 0;

	_int	m_iMenuIconCnt = 0;

	_bool	bInvenOn = false;

	/* UI_BOSS */
	_int	m_iBossHPCnt = 0;
	_bool	m_bUIRender = true;

	/* UI_BOSSNAME */
	_bool	m_bBossNameRender = false;

	/* Donald and Goofy and Rage */
	_bool	bSlot_On_Donald = false;
	_bool	bSlot_On_Goofy= false;
	_bool	bRage_Gauge = false;

	/* MiniMap */
	_bool	bRender_MiniMap = false;

	/* MiniGame */
	_bool	bRender_Minigame = false;
	_int	iMinigame_Score = 0;

	/* ========================================= */

	_bool	m_bIgnoreNavigation = false;
	_bool	m_bKeybladeLock = false;


	/* TEST */

	_int	iQuickSlotTest = 0;


	// 카메라
	_bool	m_bCameraSceneEnd = false;
	_float	m_fPlayStartInterval = 0.5f;
	_bool	m_bAppearTutorial = true;

	// For RadialBlur
	_float						m_fRadialBlurDist = 0.f;
	_bool						m_bRadialBlur = false;
	_float						m_fRadialBlurTime = 0.f;
	_float						m_fRadialBlurMaxDist = 1.f;
	_float						m_fRadialBlurMaxTime = 0.f;
	_float						m_fRadialBlurDownTime = 0.f;
	_float						m_fRadialBlurMaxDistTime = 0.f;

	// 타격감
	_bool	m_bHit = false;
	_bool	m_bHitSlow = false;
	_float	m_fHitSlowTime = 0.f;
	_float	m_fHitSlowTimeMax = 0.f;
	_float	m_fHitSpeed = 1.f;
	_float	m_fHitPower = 0.f;

	CPlayer* m_pPlayer = nullptr;
	class CCamera_Perspective* m_pCamera = nullptr;

	_bool	m_bOnTrigger[8] = {false,};
	CRenderer*		m_pRenderer = nullptr;
private:
	// 이펙트 매쉬 태그들이 담겨있음
	vector<_tchar*>			m_EffectModelTag;
private:
	void			RadialBlur(_double TimeDelta);
public:
	virtual void	Free() override;
};

END
