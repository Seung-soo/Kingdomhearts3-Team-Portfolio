#include "stdafx.h"
#include "Level_Rapunzel.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Level_Loading.h"
#include "ImGui_Manager.h"
#include "Navigation.h"
#include "Progress_Manager.h"

// GameObject
#include "Camera_Dynamic.h"
#include "Camera_Perspective.h"
#include "Terrain.h"
#include "Player.h"
#include "TreasureBox.h"

// UI
#include "UI_Parents.h"

CLevel_Rapunzel::CLevel_Rapunzel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Rapunzel::NativeConstruct()
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

	// 시작 카메라씬
	PM->Rapunzel_StartLogoScene();

	m_pRenderer = static_cast<CRenderer*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Renderer")));

	m_pRenderer->Set_HDR(false);

	m_pRenderer->Set_LUTTexIndexSecond(95);

	m_pRenderer->Set_DOF(true);
	m_pRenderer->Set_DOF_MinDistance(10.f);
	m_pRenderer->Set_DOF_MaxDistance(170.f);

	//m_pGameInstance->PlayBGM(TEXT("RapunzelBGM.mp3"), 0.6f);

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

_int CLevel_Rapunzel::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (true == GM->Get_MoveOnToNextLevel())
	{
		GM->Set_CurrentLevel(LEVEL_LOADING);
		m_pGameInstance->StopAll();
		if (FAILED(m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, GM->Get_NextLevel()))))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
		GM->Set_MoveOnToNextLevel(false);
	}

	return RESULT_NOPROBLEM;
}

_int CLevel_Rapunzel::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_pGameInstance->Get_KeyEnter(DIK_NUMPAD7))
	{
		if (false == m_pRenderer->Get_DOF())
			m_pRenderer->Set_DOF(true);
		else
			m_pRenderer->Set_DOF(false);
	}

	return RESULT_NOPROBLEM;
}

HRESULT CLevel_Rapunzel::Render()
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

				XMFLOAT4 vDiffuse   = pLight->vDiffuse;
				XMFLOAT4 vAmbient   = pLight->vAmbient;
				XMFLOAT4 vSpecualr  = pLight->vSpecular;
				XMFLOAT4 vDirection = pLight->vDirection;

				_float diffuse[4]   = { vDiffuse.x, vDiffuse.y, vDiffuse.z, vDiffuse.w };
				_float Ambient[4]   = { vAmbient.x, vAmbient.y, vAmbient.z, vAmbient.w };
				_float Specualr[4]  = { vSpecualr.x, vSpecualr.y, vSpecualr.z, vSpecualr.w };
				_float Direction[4] = { vDirection.x, vDirection.y, vDirection.z, vDirection.w };

				if (ImGui::DragFloat4("Diffuse", diffuse, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDiffuse, XMVectorSet(diffuse[0], diffuse[1], diffuse[2], diffuse[3]));
				if (ImGui::DragFloat4("Ambient", Ambient, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vAmbient, XMVectorSet(Ambient[0], Ambient[1], Ambient[2], Ambient[3]));
				if (ImGui::DragFloat4("Specular", Specualr, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
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
				_float	 fSSAO_Radius = m_pRenderer->Get_SSAORadius();
				_float	 fSSAO_Bias = m_pRenderer->Get_SSAOBias();

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
				if (ImGui::DragFloat("SSAO_Radius", &fSSAO_Radius, 0.1f, -10.f, 10.f))
					m_pRenderer->Set_SSAORadius(fSSAO_Radius);
				if (ImGui::DragFloat("SSAO_Bias", &fSSAO_Bias, 0.001f, 0.f, 1.f))
					m_pRenderer->Set_SSAOBias(fSSAO_Bias);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("LUT"))
			{
				_bool bUsingLUT = m_pRenderer->Get_UsingLUT();

				if (ImGui::Checkbox("Using LUT", &bUsingLUT))
					m_pRenderer->Set_UsingLUT(bUsingLUT);

				if (true == bUsingLUT)
				{
					CImGui_Manager::GetInstance()->Set_LUTTexIndex(m_pRenderer);
					if (ImGui::DragInt("LUTIndexSecond", &m_iLUTIndexSecond, 0.05f, 0, 96))
						m_pRenderer->Set_LUTTexIndexSecond(m_iLUTIndexSecond);
				}


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

			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	return S_OK;
}

HRESULT CLevel_Rapunzel::Ready_Lights()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTIONAL;
	/*LightDesc.vDiffuse   = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient   = _float4(1.0f, 0.9f, 0.8f, 1.f);*/

	LightDesc.vDiffuse   = _float4(0.7f, 0.8f, 1.f, 1.f);
	LightDesc.vAmbient   = _float4(0.7f, 0.7f, 0.9f, 1.f);

	LightDesc.vSpecular  = _float4(0.1f, 0.1f, 0.1f, 1.f);
	XMStoreFloat4(&LightDesc.vDirection, XMVector4Normalize(__vector(200.f, -150.f, 150.f, 0.f)));

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Rapunzel::Ready_Layer_GameObjects()
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

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Perspective"), &CameraDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	GM->Set_Camera(static_cast<CCamera_Perspective*>(pGameInstance->Get_Back(LEVEL_RAPUNZEL, TEXT("Layer_Camera"))));

	// BackGround
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

#ifdef _DEBUG
	// Axis
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Axis"), TEXT("Prototype_GameObject_Axis"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
#endif

	// Player
	CGameObject* pPlayer = pGameInstance->Add_GameObject_And_Return(LEVEL_RAPUNZEL, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"));
	if (pPlayer == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}
	else
	{
		_vector vPos = __vector(0.f, 0.f, -8.f, 1.f);
		pPlayer->Get_Transform()->Set_Position(vPos);
		pPlayer->Get_Transform()->MakeSameDirection(__vector(0.f, 0.f, 1.f, 0.f));
		static_cast<CNavigation*>(pPlayer->Get_Component(TEXT("Com_Navigation")))->Find_Cell(vPos);
		GM->Set_Player(static_cast<CPlayer*>(pPlayer));
	}

	CTreasureBox::BOXDESC tBoxDesc;
	tBoxDesc.eBoxTheme = CTreasureBox::THEME_RA;
	tBoxDesc.eEvent = CTreasureBox::EVENT_HAMMER;
	tBoxDesc.vLook = XMVector3Normalize(__vector(0.f, 0.f, -1.f, 0.f));
	tBoxDesc.vPosition = __vector(0.8f, 0.f, 40.7f, 1.f);
	// TreasureBox
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_TreasureBox"), TEXT("Prototype_GameObject_TreasureBox"), &tBoxDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tBoxDesc.eBoxTheme = CTreasureBox::THEME_RA;
	tBoxDesc.eEvent = CTreasureBox::EVENT_GOOFY;
	tBoxDesc.vLook = XMVector3Normalize(__vector(1.f, 0.f, -1.f, 0.f));
	tBoxDesc.vPosition = __vector(-5.2f, 0.f, -4.f, 1.f);
	// TreasureBox
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_TreasureBox"), TEXT("Prototype_GameObject_TreasureBox"), &tBoxDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Water
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Water"), TEXT("Prototype_GameObject_Water"), &_float4(-286.f, -9.f, -200.f, 1.f))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Rapunzel::Ready_Layer_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CHECK_FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_MoneyBar"), TEXT("Prototype_GameObject_UI_MoneyBar")));

	// UI :: Player_Circle* ==================================================================================================================================
	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_MAIN;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FACE;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_HP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_MP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FOCUS;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_HP_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_MP_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_FOCUS_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CIRCLE::UI_CIRCLE_BACK2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_Circle"), &iCom_Texture_Set)))
		return E_FAIL;

	/* UI :: LockOn */
	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Target"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON2_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Target"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_LOCKON::UI_LOCKON2_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Target"), &iCom_Texture_Set)))
		return E_FAIL;

	// UI :: Monster_Hp* ==================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_MONSTER::MONSTER_TEXT;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_MONSTER::MONSTER_HP;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_MONSTER::MONSTER_HP_BACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &iCom_Texture_Set)))
		return E_FAIL;

	// UI :: MiniMap ===============================================================================================================================================

	iCom_Texture_Set = 0;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MiniMap"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = 2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MiniMap"), &iCom_Texture_Set)))
		return E_FAIL;

	// UI :: bt ===============================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME0;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;


	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME_FAN1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME_FAN2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME5;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME6;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_FRAME7;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;
	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT2_4;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SLOT3_3;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_NAME::UI_BT_SELECT;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_bt_Command"), &iCom_Texture_Set)))
		return E_FAIL;

	// Command Extra ==========================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_ATTACK;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_SKILL;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_ITEM;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_DONALD1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_DONALD2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_GOOFY1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_CommandTEXT::UI_TEXT_GOOFY2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextCommand"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::DONALD_01;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::DONALD_02;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::GOOFY_01;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_FRAMESECOND::GOOFY_02;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FriendIcon"), &iCom_Texture_Set)))
		return E_FAIL;

	// Command_QucikSlot ==================================================================================================================================

	iCom_Texture_Set = CUI_Parents::UI_QUICKSLOT::SLOT_1;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextQuickSlot"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_QUICKSLOT::SLOT_2;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextQuickSlot"), &iCom_Texture_Set)))
		return E_FAIL;

	iCom_Texture_Set = CUI_Parents::UI_QUICKSLOT::SLOT_3;;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_TextQuickSlot"), &iCom_Texture_Set)))
		return E_FAIL;

	return S_OK;
}

CLevel_Rapunzel* CLevel_Rapunzel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Rapunzel* pInstance = new CLevel_Rapunzel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Rapunzel::Free()
{
	__super::Free();
}
