#include "stdafx.h"
#include "..\public\MainApp.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "MapTool.h"
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "AssimpLoad.h"
#include "FileLoad_Manager.h"
#include "UI_Normal.h"
#include "UI_Number.h"
#include "UI_Combo.h"
#include "UI_ComboBar.h"
#include "UI_Money.h"
#include "UI_MoneyBar.h"
#include "UI_Minigame_Num.h"
#include "GameManager.h"
#include "Progress_Manager.h"
#include "UI_Num_Manager.h"
#include "InvenManager.h"
#include "MiniGame_Manager.h"
#include "SoundManager.h"
#include "Effect_Manager.h"
#include "VIBuffer_Trail.h"
#include "Player_Weapon.h"
#include "Effect_VISwordTrail.h"
#include "Camera_Manager.h"
#include "UI_Num_Manager.h"
#include "UI_MenuNum.h"
#include "UI_MenuNum_Shop.h"
#include "UI_InvenNum.h"

//Logo
#include "UI_Logo.h"
#include "UI_BackSimbol.h"
#include "UI_FadeOut.h"

#include "Effect_Trail.h"

#define MODELTAG_SIZE 60

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::NativeConstruct()
{
	// 난수화
	srand((_uint)time(NULL));

	CGraphic_Device::GRAPHICDESC GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(CGraphic_Device::GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = CGraphic_Device::MODE_WIN;
	GraphicDesc.iWinCX = g_iWinCX;
	GraphicDesc.iWinCY = g_iWinCY;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pDeviceContext)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_GameObjects()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(GM->Ready_Prototype_EffectMeshes(m_pDevice, m_pDeviceContext)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Open_Level(LEVEL_LOGO)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CImGui_Manager::GetInstance()->ImGui_SetUp();
	CEffect_Manager::GetInstance()->Initialize();
	CFileLoad_Manager::GetInstance()->FileLoadManager_SetUp();

	//RECT	rc;
	//POINT	pt1, pt2;
	//GetClientRect(g_hWnd, &rc);

	////ShowCursor(false);

	//rc.left = pt1.x;
	//rc.top = pt1.y;
	//rc.right = pt2.x;
	//rc.bottom = pt2.y;

	//ClipCursor(&rc);

	ShowCursor(false);

	return S_OK;
}

_int CMainApp::Tick(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return -1;

	// ImGui_Manager Start
	CImGui_Manager::GetInstance()->ImGui_Start();


	NM->Update(dTimeDelta);

	if (0 > m_pGameInstance->Tick_Engine(dTimeDelta))
		return -1;

	// 게임매니저 업데이트
	GM->Update(dTimeDelta);
	PM->Update(dTimeDelta);
	MINIGAME->Tick(dTimeDelta);

	// ImGui_Manager Tick
	CImGui_Manager::GetInstance()->ImGui_Tick();

	return _int();
}

HRESULT CMainApp::Render(_double dTimeDelta)
{
	if (nullptr == m_pGameInstance)
	{
		MSGBOX("m_pGameInstance is nullptr in CMainApp::Render");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.5f, 0.5f, 0.5f, 1.f))))
	{
		MSGBOX("m_pGameInstance->Clear_BackBuffer_View returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
	{
		MSGBOX("m_pGameInstance->Clear_DepthStencil_View returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}


	if (FAILED(m_pRenderer->Render()))
	{
		MSGBOX("m_pRenderer->Render returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Render_Level()))
	{
		MSGBOX("m_pGameInstance->Render_Level returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}

	// GameObject PostRender / ImGui_Manager Render
	m_pGameInstance->GameObject_PostRender(dTimeDelta);
	CImGui_Manager::GetInstance()->ImGui_Render();

	if (FAILED(m_pGameInstance->Present()))
	{
		MSGBOX("m_pGameInstance->Present returned E_FAIL in CMainApp::Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	HRESULT hr = 0;

	switch (eLevelIndex)
	{
	case LEVEL_LOGO:
		hr = m_pGameInstance->OpenLevel(eLevelIndex, CLevel_Logo::Create(m_pDevice, m_pDeviceContext));
		break;
	default:
		hr = m_pGameInstance->OpenLevel(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eLevelIndex));
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Components()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Textures ================================================================================================================================================================================================
	/* For.Prototype_Component_Texture_UI_Loading */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Loading"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Loading_UI/Loading%d.dds"), 15))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BlackBoard */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BlackBoard"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Loading_UI/BlackBoard.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Now_Loading */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Now_Loading"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Loading_UI/Now_Loading.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Num_Font */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Num_Font"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/NumFont/MainNumFont%d.dds"), 10))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Money_Font */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Money_Font"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/NumFont/MoneyFont%d.png"), 10))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Menu_Font */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Font"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/NumFont/MenuNum_%d.png"), 10))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/Effect/Effect%d.dds"), g_iEffectTexCnt))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Noise */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Noise/noise (%d).png"), 100))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_ComboBar*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_ComboBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/ComboBar.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_MoneyBar*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_MoneyBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/NumFont/MoneyBar.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Logo*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/ExtraLogo.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Logo*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/ExtraLogo.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Title */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Title.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Title_Heart */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title_Heart"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Title_Heart.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Title_Trd */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title_Trd"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Title_Trd.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Title_Crown */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title_Crown"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Title_crown.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Title_Sora */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Title_Sora"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/TitleBgSora.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Button0*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Button0"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/GAMEPLAY.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Button1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Button1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/EXIT.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Point*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Point"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Title_sample.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Logo_Back*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Logo_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/Logo_Back%d.png"), 5))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Black_FadeOut*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Black_FadeOut"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/FadeOut.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_White_FadeOut*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_White_FadeOut"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/UI/ExtraLogo.png")))))
		return E_FAIL;

	/* For.Prototype_Copmonent_Texture_Weapon_RageForm*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Weapon_RageForm"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Textures/RageForm/PlayerWeapon_Diffuse.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RageForm*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_RageForm"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Models/Player_sora/sora_rage_D%d.png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HammerForm*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_HammerForm"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../bin/Resources/Models/Player_sora/sora_blue_D%d.png"), 4))))
		return E_FAIL;

	// Components =============================================================================================================================================================================================
	/* For.Prototype_Component_Transform */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), CTransform::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_Renderer */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	Safe_AddRef(m_pRenderer);
	GM->Set_Renderer(m_pRenderer);

	/* Prototype_Component_Collider_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* Prototype_Component_StateMachine */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), CStateMachine::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* Prototype_Component_Status */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Status"), CStatus::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* Prototype_Component_Status */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_MonsterCooler"), CMonsterCooler::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// VIBuffer ================================================================================================================================================================================================
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Axis */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Axis"), CVIBuffer_Axis::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_VITrail */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), CVIBuffer_Trail::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Shader ================================================================================================================================================================================================
	/* For.Prototype_Component_Shader */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_UI */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_VtxTex_UI.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORMALTEX_DECLARATION::Elements, VTXNORMALTEX_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_VtxNorMulTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorMulTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_VtxNorMulTex.hlsl"), VTXNORMALMULTEX_DECLARATION::Elements, VTXNORMALMULTEX_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_PointInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_PointInstance"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINT_MATRIX_DECLARATION::Elements, VTXPOINT_MATRIX_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_10"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 10))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_30"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 30))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_50"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 50))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_100"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 100))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_VtxPos */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPos"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_VtxPos.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_NonAnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_NonAnimModel.hlsl"), VTXNONANIMMODELTEX_DECLARATION::Elements, VTXNONANIMMODELTEX_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_AnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_AnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Prototype_GameObject_PlayerWeapon */
	CHECK_FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerWeapon"), CPlayer_Weapon::Create(m_pDevice, m_pDeviceContext)));

	/* For.Prototype_Component_Model_Sora */
	_matrix TransformMatrix;
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sora"), CModel::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Models/Player_sora/player_sora(named)_Add_Aerial.dat"), TransformMatrix))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObjects()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_UI_Normal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Normal"),
		CUI_Normal::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Number */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Number"),
		CUI_Number::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Combo */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Combo"),
		CUI_Combo::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ComboBar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ComboBar"),
		CUI_ComboBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Money */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Money"),
		CUI_Money::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MoneyBar */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MoneyBar"),
		CUI_MoneyBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Logo */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Logo"),
		CUI_Logo::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BackSimbol */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackSimbol"),
		CUI_BackSimbol::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LoadingBlack */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingBlack"),
		UI_FadeOut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_VISwordTrail*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VISwordTrail"),
		CEffect_VISwordTrail::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Trail*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trail"),
		CEffect_Trail::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuNumber */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuNumber"),
		CUI_MenuNum::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuNumber_Shop */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuNumber_Shop"),
		CUI_MenuNum_Shop::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Minigame_Num */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Minigame_Num"),
		CUI_Minigame_Num::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_InvenNum */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_InvenNum"),
		CUI_InvenNum::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	CEffect_Manager::GetInstance()->DestroyInstance();
	CFileLoad_Manager::GetInstance()->DestroyInstance();
	CUI_Num_Manager::GetInstance()->DestroyInstance();
	CGameInstance::GetInstance()->Level_Free();
	CInvenManager::GetInstance()->DestroyInstance();
	CProgress_Manager::GetInstance()->DestroyInstance();
	CMiniGame_Manager::GetInstance()->DestroyInstance();
	CSoundManager::GetInstance()->DestroyInstance();
	CCamera_Manager::GetInstance()->DestroyInstance();
	CGameManager::GetInstance()->DestroyInstance();

	// ImGui_Manager Free
	CImGui_Manager::GetInstance()->DestroyInstance();

	//m_pRenderer->Clear_Renderer();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	CGameInstance::Release_Engine();
}