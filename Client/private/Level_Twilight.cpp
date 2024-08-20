#include "stdafx.h"
#include "..\Public\Level_Twilight.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Level_Loading.h"
#include "ImGui_Manager.h"
#include "GameManager.h"
#include "FileLoad_Manager.h"
#include "UI_Num_Manager.h"
#include "Easing_Function.h"
#include "Camera_Manager.h"
#include "Progress_Manager.h"

// GameObject
#include "Camera_Dynamic.h"
#include "Camera_Perspective.h"
#include "Donald.h"
#include "Goofy.h"
#include "Player.h"
#include "TreasureBox.h"
#include "Monster.h"

// UI
#include "UI_Parents.h"
#include "UI_Num_Manager.h"


CLevel_Twilight::CLevel_Twilight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Twilight::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Lights()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_GameObjects()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_UI()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CHECK_FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_MoneyBar"), TEXT("Prototype_GameObject_UI_MoneyBar")));

	m_pRenderer = static_cast<CRenderer*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Renderer")));

	iCom_Texture_Set = 0; // TT
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_StageLogo"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	GM->Get_Player()->Set_ControlMode(false);

	// 시작 카메라 연출
	GM->Get_Camera()->Set_StartScene(true);

	GM->Set_bCameraMove(true);

	m_pRenderer->Set_HDR(true);
	m_pRenderer->Set_HDR_GrayScale(2.55f);
	m_pRenderer->Set_HDR_White(5.17f);


	//m_pGameInstance->PlayBGM(TEXT("TwilightTownBGM.mp3"), 0.f);

	RECT	rc;
	POINT	pt1, pt2;
	GetClientRect(g_hWnd, &rc);

	pt1.x = rc.left;
	pt1.y = rc.top;
	pt2.x = rc.right;
	pt2.y = rc.bottom;

	ClientToScreen(g_hWnd, &pt1);
	ClientToScreen(g_hWnd, &pt2);

	rc.left = pt1.x;
	rc.top = pt1.y;
	rc.right = pt2.x;
	rc.bottom = pt2.y;

	ClipCursor(&rc);

	return S_OK;
}

_int CLevel_Twilight::Tick(_double TimeDelta)
{
	if(0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (0.f < m_fBGMTime)
	{
		m_fBGMTime -= (_float)TimeDelta;
		if (0.f >= m_fBGMTime)
			m_pGameInstance->VolumeUp(CSoundMgr::BGM, 0.7f);
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == GM->Get_MoveOnToNextLevel())
	{
		GM->Set_CurrentLevel(LEVEL_LOADING);
		m_pGameInstance->StopSound(CSoundMgr::BGM);
		m_pGameInstance->StopAll();
		if (FAILED(m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, GM->Get_NextLevel()))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
		GM->Set_MoveOnToNextLevel(false);
		return RESULT_NOPROBLEM;
	}


	// 튜토리얼 UI 띄우는 타이밍
	if (FAILED(Ready_Tutorial_UI()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 첫 번째 몬스터 전투가 끝나면 두 번째 트리거 활성화
	if (true == GM->Get_OnTrigger(0) && false == m_bCreateSecondTrigger && 0 == GM->Get_MonstersList()->size())
	{
		GM->Loading_Triggers(LEVEL_TWILIGHT, TEXT("../SaveFiles/Objects/TT/TT_Triggers_Second.dat"));
		m_bCreateSecondTrigger = true;
	}

	// 첫 번째 몬스터 전투가 끝나면 두 번째 트리거 활성화
	if (true == GM->Get_OnTrigger(0) && true == GM->Get_OnTrigger(1) && false == GM->Get_OnTrigger(2) && 1 == GM->Get_MonstersList()->size())
	{
		static _float fTime = 0.f;
		static _bool bMonsterDead = false;

		if (0.f >= static_cast<CStatus*>(GM->Get_MonstersList()->back()->Get_Component(TEXT("Com_Status")))->Get_HP() && false == bMonsterDead)
		{
			GM->PlaySpeedControl(0.03f);
			GM->Set_UIRender(false);

			_int iCom_Texture = 2;
			m_pFade = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Fade"), TEXT("Prototype_GameObject_LoadingBlack"), &iCom_Texture);

			bMonsterDead = true;

			pGameInstance->StopSound(CSoundMgr::BGM);
			m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT);
			m_pGameInstance->LetsPlaySound(TEXT("WhiteFadeOut.wav"), CSoundMgr::SYSTEM_EFFECT, 0.7f);
		}
		if (true == bMonsterDead)
		{
			fTime += (_float)TimeDelta;
		}
		if (4.f < fTime)
		{
			if (nullptr != m_pFade)
			{
				m_pFade->Set_Dead(true);
				_int iCom_Texture = 3;
				m_pFade = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Fade"), TEXT("Prototype_GameObject_LoadingBlack"), &iCom_Texture);

				GM->Get_MonstersList()->back()->Set_Dead(true);
				fTime = 0.f;
				bMonsterDead = false;
				GM->Get_Camera()->Get_Transform()->MakeSameDirection(__vector(1.f, 0.f, 0.f, 0.f));
				GM->Get_Camera()->Get_Transform()->Set_Position(__vector(46.44f, 1.72f, -82.2f, 1.f));
				GM->Get_Camera()->Set_Focusing(false);
				GM->PlaySpeedControl(1.f);
			}
		}
	}

	if (true == GM->Get_OnTrigger(0) && true == GM->Get_OnTrigger(1) && false == GM->Get_OnTrigger(2) && 0 == GM->Get_MonstersList()->size())
	{
		if (nullptr != m_pFade && true == m_pFade->Get_Dead())
		{
			m_pFade = nullptr;
			// Portal
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Portal"), TEXT("Prototype_GameObject_Portal"))))
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}
	}


	if (true == GM->Get_OnTrigger(0) && true == GM->Get_OnTrigger(1) && true == GM->Get_OnTrigger(2))
	{
		PM->WhiteFadeOut(TimeDelta);
	}

	return RESULT_NOPROBLEM;
}

_int CLevel_Twilight::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_pGameInstance->Get_KeyEnter(DIK_NUMPAD1))
	{
		if (false == m_pRenderer->Get_RenderDiffuse())
			m_pRenderer->Set_RenderDiffuse(true);
		else
			m_pRenderer->Set_RenderDiffuse(false);
	}

	if (true == m_pGameInstance->Get_KeyEnter(DIK_NUMPAD3))
	{
		if (false == m_pRenderer->Get_SSAO())
			m_pRenderer->Set_SSAO(true);
		else
			m_pRenderer->Set_SSAO(false);
	}

	if (true == m_pGameInstance->Get_KeyEnter(DIK_NUMPAD4))
	{
		if (false == m_pRenderer->Get_HDR())
			m_pRenderer->Set_HDR(true);
		else
			m_pRenderer->Set_HDR(false);
	}



	return RESULT_NOPROBLEM;
}

HRESULT CLevel_Twilight::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (CImGui_Manager::GetInstance()->show_levelcontrol_window)
	{
		ImGui::SetNextWindowBgAlpha(0.7f);
		ImGui::Begin("Level Control");

		if (ImGui::BeginTabBar("Level Control", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Separated"))
			{
				LIGHTDESC* pLight = CGameInstance::GetInstance()->Get_LightDesc(0);

				XMFLOAT4 vDiffuse     = pLight->vDiffuse;
				XMFLOAT4 vAmbient     = pLight->vAmbient;
				XMFLOAT4 vSpecualr    = pLight->vSpecular;
				XMFLOAT4 vDirection   = pLight->vDirection;

				_float diffuse[4]   = { vDiffuse.x, vDiffuse.y, vDiffuse.z, vDiffuse.w };
				_float Ambient[4]   = { vAmbient.x, vAmbient.y, vAmbient.z, vAmbient.w };
				_float Specualr[4]  = { vSpecualr.x, vSpecualr.y, vSpecualr.z, vSpecualr.w };
				_float Direction[4] = { vDirection.x, vDirection.y, vDirection.z, vDirection.w };
				

				if (ImGui::DragFloat4("Diffuse", diffuse, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDiffuse, XMVectorSet(diffuse[0], diffuse[1], diffuse[2], diffuse[3]));
				if (ImGui::DragFloat4("Ambient", Ambient, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vAmbient, XMVectorSet(Ambient[0], Ambient[1], Ambient[2], Ambient[3]));
				if (ImGui::DragFloat4("Specular", Specualr, 0.01f, 0.0f, 1.f, "%.2f"))
					XMStoreFloat4(&pLight->vSpecular, XMVectorSet(Specualr[0], Specualr[1], Specualr[2], Specualr[3]));
				if (ImGui::DragFloat4("Direction", Direction, 0.1f, -10.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDirection, XMVectorSet(Direction[0], Direction[1], Direction[2], Direction[3]));


				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("United"))
			{
				LIGHTDESC* pLight = CGameInstance::GetInstance()->Get_LightDesc(0);

				XMFLOAT4 vDiffuse   = pLight->vDiffuse;
				XMFLOAT4 vAmbient   = pLight->vAmbient;
				XMFLOAT4 vSpecualr  = pLight->vSpecular;
				XMFLOAT4 vDirection = pLight->vDirection;

				_float diffuse      = vDiffuse.x;
				_float Ambient      = vAmbient.x;
				_float Specualr     = vSpecualr.x;
				_float Direction[4] = { vDirection.x, vDirection.y, vDirection.z, vDirection.w };

				if (ImGui::DragFloat("Diffuse", &diffuse, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDiffuse, XMVectorSet(diffuse, diffuse, diffuse, diffuse));
				if (ImGui::DragFloat("Ambient", &Ambient, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vAmbient, XMVectorSet(Ambient, Ambient, Ambient, Ambient));
				if (ImGui::DragFloat("Specular", &Specualr, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vSpecular, XMVectorSet(Specualr, Specualr, Specualr, Specualr));
				if (ImGui::DragFloat4("Direction", Direction, 0.1f, -10.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDirection, XMVectorSet(Direction[0], Direction[1], Direction[2], Direction[3]));

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("LUT"))
			{
				_bool bUsingLUT = m_pRenderer->Get_UsingLUT();

				if (ImGui::Checkbox("Using LUT", &bUsingLUT))
					m_pRenderer->Set_UsingLUT(bUsingLUT);

				if (true == bUsingLUT)
					CImGui_Manager::GetInstance()->Set_LUTTexIndex(m_pRenderer);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Shader"))
			{
				_float	 fSSAO_Radius = m_pRenderer->Get_SSAORadius();
				_float	 fSSAO_Bias = m_pRenderer->Get_SSAOBias();

				if (ImGui::DragFloat("SSAO_Radius", &fSSAO_Radius, 0.1f, -10.f, 10.f))
					m_pRenderer->Set_SSAORadius(fSSAO_Radius);
				if (ImGui::DragFloat("SSAO_Bias", &fSSAO_Bias, 0.001f, 0.f, 1.f))
					m_pRenderer->Set_SSAOBias(fSSAO_Bias);

				_float	 fHDR_GrayScale = m_pRenderer->Get_HDR_GrayScale();
				_float	 fHDR_White = m_pRenderer->Get_HDR_White();

				if (ImGui::DragFloat("HDR_GrayScale", &fHDR_GrayScale, 0.01f, 0.f, 10.f))
					m_pRenderer->Set_HDR_GrayScale(fHDR_GrayScale);
				if (ImGui::DragFloat("HDR_White", &fHDR_White, 0.01f, 0.f, 10.f))
					m_pRenderer->Set_HDR_White(fHDR_White);

				_bool	bHDR = m_pRenderer->Get_HDR();
				_bool	bBloom = m_pRenderer->Get_Bloom();

				if (ImGui::Checkbox("OnHDR", &bHDR))
					m_pRenderer->Set_HDR(bHDR);
				
				if (ImGui::Checkbox("OnBloom", &bBloom))
					m_pRenderer->Set_Bloom(bBloom);

				_float	fFogStart = m_pRenderer->Get_FogStart();
				_float	fFogEnd = m_pRenderer->Get_FogEnd();
				_float	fFogColor[4];
				memcpy(fFogColor, &m_pRenderer->Get_FogColor(), sizeof(_float4));
				_float	fMaxFogFactor = m_pRenderer->Get_MaxFogFactor();
				_bool	bFog = m_pRenderer->Get_Fog();


				if (ImGui::DragFloat("FogStart", &fFogStart, 0.1f, 0.f, 300.f, "%.1f"))
					m_pRenderer->Set_FogStart(fFogStart);
				if (ImGui::DragFloat("FogEnd", &fFogEnd, 0.1f, 0.f, 300.f, "%.1f"))
					m_pRenderer->Set_FogEnd(fFogEnd);
				if (ImGui::DragFloat4("FogColor", fFogColor, 0.01f, 0.f, 1.f, "%.2f"))
				{
					_float4	vFogColor;
					memcpy(&vFogColor, fFogColor, sizeof(_float4));
					m_pRenderer->Set_FogColor(vFogColor);
				}
				if (ImGui::DragFloat("MaxFogFactor", &fMaxFogFactor, 0.01f, 0.f, 1.f, "%.2f"))
					m_pRenderer->Set_MaxFogFactor(fMaxFogFactor);
				if (ImGui::Checkbox("OnFog", &bFog))
					m_pRenderer->Set_Fog(bFog);


				_bool	bDOF = m_pRenderer->Get_DOF();
				_float	fDOF_MinDistance = m_pRenderer->Get_DOF_MinDistance();
				_float	fDOF_MaxDistance = m_pRenderer->Get_DOF_MaxDistance();

				if (ImGui::DragFloat("DOF_MinDistance", &fDOF_MinDistance, 0.1f, 0.f, 300.f, "%.1f"))
					m_pRenderer->Set_DOF_MinDistance(fDOF_MinDistance);
				if (ImGui::DragFloat("DOF_MaxDistance", &fDOF_MaxDistance, 0.1f, 0.f, 300.f, "%.1f"))
					m_pRenderer->Set_DOF_MaxDistance(fDOF_MaxDistance);
				if (ImGui::Checkbox("OnDOF", &bDOF))
					m_pRenderer->Set_DOF(bDOF);

				_bool	bOutline = m_pRenderer->Get_Outline();
				if (ImGui::Checkbox("OnOutline", &bOutline))
					m_pRenderer->Set_Outline(bOutline);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("RadialBlur"))
			{
				static _float	fRadialBlurMaxDist = 0.f;
				static _float	fRadialBlurMaxTime = 0.f;
				static _float	fRadialBlurDownTime = 0.f;
				static _float	fRadialBlurMaxDistTime = 0.f;
				ImGui::DragFloat("BlurMaxDist", &fRadialBlurMaxDist, 0.01f, 0.f, 20.f, "%.2f");
				ImGui::DragFloat("BlurMaxTime", &fRadialBlurMaxTime, 0.01f, 0.f, 20.f, "%.2f");
				ImGui::DragFloat("BlurDownTime", &fRadialBlurDownTime, 0.01f, 0.f, 20.f, "%.2f");
				ImGui::DragFloat("BlurMaxDistTime", &fRadialBlurMaxDistTime, 0.01f, 0.f, 20.f, "%.2f");

				if (ImGui::Button("Go_RadialBlur"))
					GM->Use_RadialBlur(true, fRadialBlurMaxDist, fRadialBlurMaxTime, fRadialBlurDownTime, fRadialBlurMaxDistTime);

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_WHEEL))
		{
			auto CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(0.f);
			if (CollisionCheck.first == true)
			{
				_vector vPos = CollisionCheck.second;
				//m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_SkullSoldier"), TEXT("Prototype_GameObject_SkullSoldier"), &vPos);


				CTreasureBox::BOXDESC tBoxDesc;
				tBoxDesc.vLook     = __vector(0.f, 0.f, -1.f, 0.f);
				tBoxDesc.vPosition = vPos;
				tBoxDesc.eBoxTheme = CTreasureBox::THEME_TT;
				tBoxDesc.eEvent    = CTreasureBox::EVENT_QUEST;
				m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_TreasureBox"), TEXT("Prototype_GameObject_TreasureBox"), &tBoxDesc);
			}
		}

		ImGui::End();
	}

	return S_OK;
}

HRESULT CLevel_Twilight::Ready_Lights()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;

	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	//LightDesc.eLightType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vDiffuse = _float4(0.5f, 0.5f, 0.1f, 1.f);
	//LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.1f, 1.f);
	//LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.1f, 1.f);
	//LightDesc.vPosition = _float4(24.f, 0.f, -20.f, 1.f);
	//LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	//LightDesc.fRange = 5.f;

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
	//	return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.8f, 0.3f, 0.3f, 1.f);
	LightDesc.vAmbient = _float4(0.7f, 0.3f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(0.05f, 0.03f, 0.01f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Twilight::Ready_Layer_GameObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Camera
	CCamera::CAMERADESC	CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));

	CameraDesc.vEye   = _float3(0.f, 10.f, -7.f);
	CameraDesc.vAt    = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy   = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear   = 0.2f;
	CameraDesc.fFar    = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec    = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Perspective"), &CameraDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	GM->Set_Camera(static_cast<CCamera_Perspective*>(pGameInstance->Get_Back(LEVEL_TWILIGHT, TEXT("Layer_Camera"))));

	// BackGround
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Player
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	GM->Set_Player(static_cast<CPlayer*>(pGameInstance->Get_Back(LEVEL_TWILIGHT, TEXT("Layer_Player"))));

#ifdef _DEBUG
	// Axis
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Axis"), TEXT("Prototype_GameObject_Axis"))))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}
#endif

	// TreasureBox
	CTreasureBox::BOXDESC tBoxDesc;
	tBoxDesc.vLook     = __vector(0.f, 0.f, -1.f, 0.f);
	tBoxDesc.vPosition = __vector(-8.5f, 0.f, -71.f, 1.f);
	tBoxDesc.eBoxTheme = CTreasureBox::THEME_TT;
	tBoxDesc.eEvent    = CTreasureBox::EVENT_QUEST;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_TreasureBox"), TEXT("Prototype_GameObject_TreasureBox"), &tBoxDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tBoxDesc.vLook = __vector(0.f, 0.f, -1.f, 0.f);
	tBoxDesc.vPosition = __vector(80.5f, 0.f, -43.f, 1.f);
	tBoxDesc.eBoxTheme = CTreasureBox::THEME_TT;
	tBoxDesc.eEvent = CTreasureBox::EVENT_KEYBLADE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_TreasureBox"), TEXT("Prototype_GameObject_TreasureBox"), &tBoxDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	// 도널드 오브젝트풀
	if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_STATIC, TEXT("Layer_Donald"), TEXT("Prototype_GameObject_Donald"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 구피 오브젝트풀
	if (FAILED(pGameInstance->Add_ObjectPool(LEVEL_STATIC, TEXT("Layer_Goofy"), TEXT("Prototype_GameObject_Goofy"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_FireFly"))))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_Twilight::Ready_Layer_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// UI :: Player_Circle* ==================================================================================================================================
	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_MAIN;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FACE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_HP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_MP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FOCUS;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_HP_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_MP_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FOCUS_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_BACK2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = 0;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_RageGauge"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = 1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_RageGauge"), &iCom_Texture_Set)))
		return E_FAIL;

	/* UI :: LockOn */
	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Target"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON2_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Target"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON2_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Target"), &iCom_Texture_Set)))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_BossTarget"))))
	//	return E_FAIL;

	// UI :: Monster_Hp* ==================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_MONSTER::MONSTER_TEXT;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI_Monster"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_MONSTER::MONSTER_HP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI_Monster"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_MONSTER::MONSTER_HP_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI_Monster"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &iCom_Texture_Set)))
		return E_FAIL;

	// UI :: bt ===============================================================================================================================================
	
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME0;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;


	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME_FAN1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME_FAN2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME5;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME6;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME7;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3_3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SELECT;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	 
	// Command Extra ==========================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_ATTACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_SKILL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_ITEM;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_DONALD1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_DONALD2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_GOOFY1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_GOOFY2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::DONALD_01;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"),&iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::DONALD_02;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::GOOFY_01;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::GOOFY_02;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	// Command_QucikSlot ==================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_QUICKSLOT::SLOT_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextQuickSlot"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_QUICKSLOT::SLOT_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextQuickSlot"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_QUICKSLOT::SLOT_3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextQuickSlot"), &iCom_Texture_Set)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_NPC"), TEXT("Prototype_GameObject_TT_NPC0"))))
		return E_FAIL;

	iCom_Texture_Set = 0;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MiniMap"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = 1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MiniMap"), &iCom_Texture_Set)))
		return E_FAIL;

	PM->Create_QuestNavi();
	PM->QuestNavi_TargetSetting(pGameInstance->Get_Back(LEVEL_TWILIGHT, TEXT("Layer_NPC"))->Get_Transform()->Get_Position());

	return S_OK;
}


HRESULT CLevel_Twilight::Ready_Tutorial_UI()
{
	if (false == GM->Get_CameraSceneEnd() && 0.f > GM->Get_PlayStartInterval() && true == GM->Get_AppearTutorial())
	{
		_int	iBackUI = 0;
		_int	iTutorialUI = 1;
		switch (m_iCameraSceneIndex)
		{
		case 0:	
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iBackUI)))
			{
				BREAKPOINT;
				return E_FAIL;
			}

			iTutorialUI = 1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Tutorial"), TEXT("Prototype_GameObject_UI_Tip"), &iTutorialUI)))
			{
				BREAKPOINT;
				return E_FAIL;
			}
			m_iCameraSceneIndex++;

			// 카메라씬 끝나면 튜토리얼 UI 뜨는거 꺼줌
			GM->Set_AppearTutorial(false);
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		}
		GM->Set_PlayStartInterval(0.5f);
	}


	return S_OK;
}

CLevel_Twilight * CLevel_Twilight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Twilight* pInstance = new CLevel_Twilight(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Twilight::Free()
{
	__super::Free();
}