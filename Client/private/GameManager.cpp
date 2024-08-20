#include "stdafx.h"
#include "..\public\GameManager.h"
#include "Monster.h"
#include "Navigation.h"
#include "GameInstance.h"
#include "InvenManager.h"
#include "Player.h"
#include "Camera_Perspective.h"
#include "Trigger.h"
#include "Easing_Function.h"
#include "UI_Parents.h"
#include "Progress_Manager.h"
#include "SoundManager.h"
#include "PlayerHit.h"
#include "Combat_Assistant.h"

READY_SINGLETON(CGameManager)

CGameManager::CGameManager()
{
}

list<CMonster*>* CGameManager::Get_MonstersList()
{
	return &m_Monsters;
}

void CGameManager::Update(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 몬스터 한방에 죽이는 코드
	if (pGameInstance->Get_LevelIndex() != LEVEL_TEST && pGameInstance->Get_LevelIndex() != LEVEL_MAPTOOL)
	{
		if (pGameInstance->Get_KeyPressing(DIK_LSHIFT) && pGameInstance->Get_KeyEnter(DIK_G))
		{
			Release_Monsters();
		}

		//	몬스터 리스트를 순회하며 죽었는지 판단하고 죽었으면 컨테이너에서 뺀다.
		for (auto iter = m_Monsters.begin(); iter != m_Monsters.end();)
		{
			if (true == (*iter)->Get_Dead())
			{
				if (false == (*iter)->Get_IsBoss())
				{
					_int iRand = rand() % 2;
					if (0 == iRand)
					{
						if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Item_Object"), (*iter)->Get_Transform())))
						{
							BREAKPOINT;
							return;
						}
					}

					_int iRandomNumber = rand() % 3 + 4;

					for (_int i = 0; i < iRandomNumber; i++)
					{
						if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Money_Object"), (*iter)->Get_Transform())))
						{
							BREAKPOINT;
							return;
						}
					}
				}

				iter = m_Monsters.erase(iter);

			}
			else
				iter++;
		}
	}

	// 레디얼 블러
	if (true == m_bRadialBlur)
		RadialBlur(TimeDelta);

	// 타격감 스피드
	if (true == m_bHit)
		HitSpeedUpdate(TimeDelta);

	/* Item_Get */
	m_dItem_Time += TimeDelta;  /* 아이템 획득 창 생성 1.4초의 텀 */

	if (1.4 < m_dItem_Time && m_iItemCnt > 0)
	{
		m_bItemGet = true;
		if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_ItemGet"))))
		{
			BREAKPOINT;
			return;
		}
		m_iItemCnt -= 1;

		m_dItem_Time = 0.0;
	}

	/* StateScreen */
	if (m_eCurrentLevel >= LEVEL_TWILIGHT && m_eCurrentLevel <= LEVEL_FINALBOSS)
	{
		if (pGameInstance->Get_KeyEnter(DIK_I) && 0 == m_iMenuStart)
		{
			LEVEL curr_level = m_eCurrentLevel;
			_bool playing_stage = false;

			switch (curr_level)
			{
			case LEVEL_TWILIGHT:
			case LEVEL_RAPUNZEL:
			case LEVEL_RAPUNZEL_BOSS:
			case LEVEL_FROZEN:
			case LEVEL_KEYGRAVE:
			case LEVEL_FINALBOSS:
				playing_stage = true;
				break;
			}

			PlaySpeedControl(0.f);

			SM->Sound_Command(L"UI_MenuOn.wav", 1.f);

			if (true == playing_stage)
			{
				/* UI_MENU */
				m_iMenuStart = 1;
				bInvenOn = true;

				iCom_Texture_Set = CUI_Parents::UI_MENU::MENU_BACKGROUND;
				if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Menu"), &iCom_Texture_Set)))
					return;

				iCom_Texture_Set = CUI_Parents::UI_MENU::MENU_FRAME;
				if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Menu"), &iCom_Texture_Set)))
					return;

				iCom_Texture_Set = CUI_Parents::UI_MENU::MENU_FRAME_MONEY;
				if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Menu"), &iCom_Texture_Set)))
					return;
			}
		}
	}


	// 카메라씬이 끝나고 다음 행동을 위한 간격
	if (true == m_bCameraSceneEnd)
	{
		m_fPlayStartInterval -= (_float)TimeDelta;
		if (0.f > m_fPlayStartInterval)
		{
			m_bCameraSceneEnd = false;
		}
	}

	/* MiniMap_OnOff*/
	if (pGameInstance->Get_KeyEnter(DIK_M))
	{
		if (true == bRender_MiniMap)
			bRender_MiniMap = false;
		else
			bRender_MiniMap = true;
	}

	if (true == m_bBossNameRender)
	{
		iCom_Texture_Set = 0;
		if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_BossName"), &iCom_Texture_Set)))
		{
			BREAKPOINT;
			return;
		}
		iCom_Texture_Set = 1;
		if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_BossName"), &iCom_Texture_Set)))
		{
			BREAKPOINT;
			return;
		}

		iCom_Texture_Set = 2;
		if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_BossName"), &iCom_Texture_Set)))
		{
			BREAKPOINT;
			return;
		}
		iCom_Texture_Set = 3;
		if (FAILED(pGameInstance->Add_GameObject(m_eCurrentLevel, TEXT("Layer_UI"), TEXT("Prototype_GameObject_BossName"), &iCom_Texture_Set)))
		{
			BREAKPOINT;
			return;
		}

		SM->Sound_Command(L"UI_BossSound.wav", 1.f);
		
		m_bBossNameRender = false;
	}

}

HRESULT CGameManager::Add_Monster(CMonster * pMonster)
{
	if (nullptr == pMonster)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_Monsters.push_back(pMonster);

	return S_OK;
}

void CGameManager::Release_Monsters()
{
	for (auto& pMonster : m_Monsters)
	{
		pMonster->Set_Dead(true);
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(m_eCurrentLevel, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Item_Object"), pMonster->Get_Transform())))
		{
			BREAKPOINT;
			return;
		}
	}
	m_Monsters.clear();

	m_pPlayer->Get_CombatAssistant()->Clear();
}

void CGameManager::Set_MonsterStun()
{
	for (auto& pMonster : m_Monsters)
	{
		CStatus* pStatus = dynamic_cast<CStatus*>(pMonster->Get_Component(TEXT("Com_Status")));
		if (pStatus == nullptr)
			continue;

		pStatus->Set_Stun(true, 3.0);
	}
}

void CGameManager::Set_MonsterSlow()
{
	for (auto& pMonster : m_Monsters)
	{
		CStatus* pStatus = dynamic_cast<CStatus*>(pMonster->Get_Component(TEXT("Com_Status")));
		if (pStatus == nullptr)
			continue;

		pStatus->Set_Slow(0.1);
	}
}

CCamera_Perspective * CGameManager::Get_Camera()
{
	return m_pCamera;
}

void CGameManager::Set_Camera(CCamera_Perspective * pCamera)
{
	m_pCamera = pCamera;
}

CPlayer * CGameManager::Get_Player()
{
	return m_pPlayer;
}

void CGameManager::Set_Player(CPlayer * pPlayer)
{
	m_pPlayer = pPlayer;
}

HRESULT CGameManager::Ready_Prototype_EffectMeshes(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	// 파일 목록 불러오기
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Resources/EffectMesh/*.*", &fd);
	if (handle == -1 || handle == 0)
		return E_FAIL;

	int iResult = 0;
	int i = 0;
	char szFilename[MAX_PATH];

	while (iResult != -1)
	{
		char szFullPath[MAX_PATH] = "../Bin/Resources/EffectMesh/";

		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, "..") || !strcmp(szFilename, "desktop.ini"))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		string str = string(szFilename);
		string findStr = string(".dat");
		str.erase(str.find(findStr), findStr.length());

		strcpy_s(szFilename, str.c_str());
		sprintf_s(szFilename, szFilename);

		strcat_s(szFullPath, "/");
		strcat_s(szFullPath, szFilename);
		strcat_s(szFullPath, ".dat");

		//	모델 프로토타입태그 생성
		char	szPrototypeName[60] = "Prototype_Component_Model_";
		strcat_s(szPrototypeName, szFilename);

		_tchar* wszPrototypeName = new _tchar[60];

		MultiByteToWideChar(CP_ACP, 0, szPrototypeName, 60, wszPrototypeName, 60);

		m_EffectModelTag.push_back(wszPrototypeName);


		//	모델 경로
		_tchar	wszFullPath[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szFullPath, MAX_PATH, wszFullPath, MAX_PATH);

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, wszPrototypeName, CModel::Create(pDevice, pDeviceContext, wszFullPath, XMMatrixIdentity()))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	return S_OK;
}

void CGameManager::IgnoreMonsterNavigation(_bool bOption)
{
	for (auto& pMonster : m_Monsters)
	{
		CNavigation* pNavi = dynamic_cast<CNavigation*>(pMonster->Get_Component(TEXT("Com_Navigation")));
		if (pNavi == nullptr)
			continue;

		pNavi->Set_Ignore(bOption);
	}
}

HRESULT CGameManager::Loading_Triggers(LEVEL eLeve, const _tchar * pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 생성하고 월드행렬이랑 타입 세팅
	DWORD               dwBytes = 0;
	_float4x4           WorldMatrix;
	CTrigger::TRIGGER   eTrigger;

	while (true)
	{
		ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwBytes, nullptr);
		if (dwBytes == 0)
			break;

		ReadFile(hFile, &eTrigger, sizeof(CTrigger::TRIGGER), &dwBytes, nullptr);
		if (dwBytes == 0)
			break;

		CGameObject* pGameObject = CGameInstance::GetInstance()->Add_GameObject_And_Return(eLeve, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_Trigger"));
		pGameObject->Get_Transform()->Set_WorldMatrix(WorldMatrix);
		dynamic_cast<CTrigger*>(pGameObject)->Set_TriggerType(eTrigger);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CGameManager::PlaySpeedControl(_float fSpeed)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iCurrentLevel = (_uint)m_eCurrentLevel;
	if (iCurrentLevel >= LEVEL_END)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	pGameInstance->Set_TimeSpeed(iCurrentLevel, TEXT("Layer_Player"), fSpeed);
	pGameInstance->Set_TimeSpeed(iCurrentLevel, TEXT("Layer_PlayerWeapon"), fSpeed);
	pGameInstance->Set_TimeSpeed(iCurrentLevel, TEXT("Layer_Donald"), fSpeed);
	pGameInstance->Set_TimeSpeed(iCurrentLevel, TEXT("Layer_Goofy"), fSpeed);
	pGameInstance->Set_TimeSpeed(iCurrentLevel, TEXT("Layer_Effect"), fSpeed);
	pGameInstance->Set_TimeSpeed(iCurrentLevel, TEXT("Layer_MonsterSkill"), fSpeed);
	
	for (auto pMonster : m_Monsters)
	{
		pGameInstance->Set_TimeSpeed(iCurrentLevel, pMonster->Get_LayerTag(), fSpeed);
	}

	return S_OK;
}

void CGameManager::HitSpeedControl(_float fSpeed, _float fTime, _float fPower)
{
	if (true == m_bHit)
		return;

	m_bHit = true;
	m_fHitSpeed = fSpeed;
	m_fHitSlowTimeMax = fTime;
	m_fHitPower = fPower;

	m_pCamera->Set_Distance(m_pCamera->Get_DefaultDistance() - fPower);
}

void CGameManager::HitSpeedUpdate(_double TimeDelta)
{
	m_fHitSlowTime += (_float)TimeDelta;

	if (m_fHitSlowTimeMax * 0.01f > m_fHitSlowTime)
	{

	}
	else if (m_fHitSlowTimeMax * 0.01f <= m_fHitSlowTime && m_fHitSlowTimeMax * 0.8f >= m_fHitSlowTime)
	{
		if (false == m_bHitSlow)
		{
			PlaySpeedControl(m_fHitSpeed);
			m_bHitSlow = true;
		}
	}
	else if (m_fHitSlowTimeMax * 0.8f < m_fHitSlowTime)
	{
		if (m_fHitSlowTimeMax < m_fHitSlowTime)
			m_fHitSlowTime = m_fHitSlowTimeMax;

		m_pCamera->Set_Distance((m_pCamera->Get_DefaultDistance() - m_fHitPower) + EaseInExpo(m_fHitPower, m_fHitSlowTime - m_fHitSlowTimeMax * 0.8f, m_fHitSlowTimeMax * 0.2f));
	}

	if (m_fHitSlowTimeMax <= m_fHitSlowTime)
	{
		m_bHit = false;
		m_bHitSlow = false;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (LEVEL_TWILIGHT == m_eCurrentLevel && (nullptr != pGameInstance->Get_ObjectList(LEVEL_TWILIGHT, TEXT("Layer_Fade")) && 0 < pGameInstance->Get_Size(LEVEL_TWILIGHT, TEXT("Layer_Fade"))))
		{
		}
		else
		{
			if (false == PM->Get_WhiteFirst())
				PlaySpeedControl(1.f);
		}
		m_fHitSlowTime = 0.f;
		m_fHitSlowTimeMax = 0.f;
	}
}

HRESULT CGameManager::Use_RadialBlur(_bool bUse, _float fMaxBlurDist
	, _float fMaxTime, _float fDownTime, _float fRadialBlurMaxDistTime)
{
	m_bRadialBlur = true;
	m_fRadialBlurDist = 0.f;
	m_fRadialBlurMaxDist = fMaxBlurDist;
	m_fRadialBlurTime = 0.f;
	m_fRadialBlurMaxTime = fMaxTime;
	m_fRadialBlurDownTime = fDownTime;
	m_fRadialBlurMaxDistTime = fRadialBlurMaxDistTime;

	m_pRenderer->Set_RadialBlur(bUse);
	return S_OK;
}

HRESULT CGameManager::HitBlood()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	auto ObjectList = pGameInstance->Get_ObjectList(GM->Get_CurrentLevel(), TEXT("Layer_PlayerHit"));
	if (nullptr == ObjectList || 0 == ObjectList->size())
	{
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_PlayerHit"), TEXT("Prototype_GameObject_PlayerHit"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	else
	{
		static_cast<CPlayerHit*>(pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_PlayerHit")))->Set_Alpha();
	}

	return S_OK;
}

void CGameManager::RadialBlur(_double TimeDelta)
{
	m_fRadialBlurTime += (_float)TimeDelta;
	if (m_fRadialBlurMaxTime <= m_fRadialBlurTime)
	{
		m_fRadialBlurTime = m_fRadialBlurMaxTime;
		m_bRadialBlur = false;
		m_pRenderer->Set_RadialBlur(false);
	}
	
	if (m_fRadialBlurMaxDistTime >= m_fRadialBlurTime)
		m_fRadialBlurDist = EaseOutQuart(m_fRadialBlurMaxDist, m_fRadialBlurTime, m_fRadialBlurMaxDistTime);
	else if (m_fRadialBlurDownTime < m_fRadialBlurTime)
		m_fRadialBlurDist = m_fRadialBlurMaxDist - EaseInQuart(m_fRadialBlurMaxDist
			, m_fRadialBlurTime - m_fRadialBlurDownTime, m_fRadialBlurMaxTime - m_fRadialBlurDownTime);

	m_pRenderer->Set_RadialBlurDist(m_fRadialBlurDist);
}

void CGameManager::Free()
{
	for (auto& pModelTag : m_EffectModelTag)
		Safe_Delete_Array(pModelTag);
	m_EffectModelTag.clear();

	m_Monsters.clear();
}