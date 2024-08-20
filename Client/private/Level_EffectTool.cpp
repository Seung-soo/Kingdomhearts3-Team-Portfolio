#include "stdafx.h"
#include "..\Public\Level_EffectTool.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "Camera_Dynamic.h"
#include "ImGui_Manager.h"
#include "Effect_Texture.h"
#include "MonsterGenerator.h"

#include "GameManager.h"
#include "FileLoad_Manager.h"
#include "Terrain.h"

CLevel_EffectTool::CLevel_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{

}

HRESULT CLevel_EffectTool::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Axis(TEXT("Layer_Axis"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_EFFECTTOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	m_tEffectMeshDesc.iLevelIndex = LEVEL_EFFECTTOOL;
	m_tEffectMeshDesc.fLifeTime = 1.f;

	m_tTexDesc.bDefaultPass = true;

	lstrcpy(m_tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Storm"));

	CGameInstance::GetInstance()->StopAll();

	return S_OK;
}

_int CLevel_EffectTool::Tick(_double TimeDelta)
{
	if(0 > __super::Tick(TimeDelta))
		return -1;

	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Effect_Tool");
	
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		//Texture_Tab ==========================================================================================================================================================
		if (ImGui::BeginTabItem("Effect_Texture"))
		{
			if (true == ImGui::CollapsingHeader("Texture"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				
				if (ImGui::TreeNode("Texture"))
				{
					static int item_current_idx = 0; // Here we store our selection data as an index.
					static char buf1[64] = "";

				//	ImGui::InputText("Texture_Prototype_Tag", buf1, IM_ARRAYSIZE(buf1));

					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buf1, (int)strlen(buf1), m_tTexDesc.TextureTag, 60);
					//m_iLevelIndex = LEVEL_EFFECTTOOL;

					lstrcpy(m_tTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));

					static _int		iPassIndex = 0;
					static _int		iTextureIndex = 0;
					static _int		iSecondTextureIndex = 0;

					ImGui::InputInt("PassIndex", &iPassIndex, 1);
					m_tTexDesc.iPassIndex = iPassIndex;

					auto iTexIndex = g_iEffectTexCnt - 1;

					ImGui::DragInt("TextureIndex", &iTextureIndex, 1, 0, iTexIndex);

					if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT))
					{
						if ((_uint)++iTextureIndex > iTexIndex)
							iTextureIndex = 0;
					}
					if (m_pGameInstance->Get_KeyEnter(DIK_LEFT))
					{
						if (--iTextureIndex < 0)
							iTextureIndex = iTexIndex;
					}

					m_tTexDesc.iTextureIndex = iTextureIndex;

					ImGui::Checkbox("Using Noise", &m_tTexDesc.bNoising);

					ImGui::InputInt("Second TextureIndex", &iSecondTextureIndex, 1);
					m_tTexDesc.iSecondTextureIndex = iSecondTextureIndex;

					ImGui::InputFloat("NoiseRot", &m_tTexDesc.fNoiseRot);

					static ImGuiComboFlags flags = 0;

					const char* items[] = { "LEVEL_STATIC", "LEVEL_LOADING", "LEVEL_LOGO", "LEVEL_TWILIGHT", "LEVEL_RAPUNZEL",
						"LEVEL_MAPTOOL", "LEVEL_EFFECTTOOL", "LEVEL_COLLIDERTOOL", "LEVEL_TEST"};

					static int Current_ItemIdx = 0;
					const char* combo_preview_value = items[Current_ItemIdx];

					if (ImGui::BeginCombo("LEVEL", combo_preview_value, flags))
					{
						for (int n = 0; n < IM_ARRAYSIZE(items); n++)
						{
							const bool is_selected = (Current_ItemIdx == n);
							if (ImGui::Selectable(items[n], is_selected))
							{
								Current_ItemIdx = n;
								m_tTexDesc.iLevelIndex = Current_ItemIdx;
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					/*const char* items2[] = { "Effect0", "Effect1", "Effect2", "Effect3" };
					static int Current_ItemIdx2 = 0;
					const char* combo_preview_value2 = items2[Current_ItemIdx2];

					if (ImGui::BeginListBox("##EffectTag", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
					{
						for (int n = 0; n < IM_ARRAYSIZE(items2); n++)
						{
							const bool is_selected_Model = (Current_ItemIdx2 == n);
							if (ImGui::Selectable(items2[n], is_selected_Model))
							{
								Current_ItemIdx2 = n;
								const char* buf1 = "Prototype_Component_Texture_Effect";
								MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buf1, (int)strlen(buf1), m_tTexDesc.TextureTag, 60);
								m_tTexDesc.iTextureIndex = Current_ItemIdx2;
							}

							if (is_selected_Model)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndListBox();
					}*/
					ImGui::TreePop();
				}
			}

			if (true == ImGui::CollapsingHeader("Play_Desc"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::DragFloat("TotalPlayTime", &m_tTexDesc.fTotalTime, 0.25f, 1.f, 100.f);

				ImGui::Checkbox("Turning", &m_tTexDesc.IsTurning);

				static float fLook[3] = {0.f, 0.f, 1.f};

				ImGui::DragFloat3("Look", fLook, 0.25f, -500.f, 500.f);
				m_tTexDesc.vLookDir= _float3(fLook[0], fLook[1], fLook[2]);

				ImGui::Checkbox("Billboarding", &m_tTexDesc.bBillboarding);

				ImGui::Checkbox("Blur", &m_tTexDesc.bBlur);

				ImGui::Checkbox("Distortion", &m_tTexDesc.bDistortion);

				ImGui::Checkbox("Sprite", &m_tTexDesc.bSprite);
				ImGui::Checkbox("MultiSprite", &m_tTexDesc.bMultiSprite);

				if (true == m_tTexDesc.bSprite || true == m_tTexDesc.bMultiSprite)
				{
					if (ImGui::TreeNode("Texture"))
					{
						static ImGuiComboFlags flags = 0;

						const char* items[] = { "PLAY_LOOP", "PLAY_ONCE" };
						static int Current_ItemIdx = 0;
						const char* combo_preview_value = items[Current_ItemIdx];

						if (ImGui::BeginCombo("Play_Type", combo_preview_value, flags))
						{
							for (int n = 0; n < IM_ARRAYSIZE(items); n++)
							{
								const bool is_selected = (Current_ItemIdx == n);
								if (ImGui::Selectable(items[n], is_selected))
								{
									Current_ItemIdx = n;
									m_tTexDesc.ePlayType = CEffect_Texture::PLAY_TYPE(Current_ItemIdx);
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}

						if (ImGui::TreeNode("SpriteDesc"))
						{
							static _int	fSpriteMaxIndex[2] = {};
							ImGui::DragInt2("SpriteMaxIndex", fSpriteMaxIndex, 1, 0, 500);
							m_tTexDesc.fSpriteMaxXIndex = fSpriteMaxIndex[0];
							m_tTexDesc.fSpriteMaxYIndex = fSpriteMaxIndex[1];

							ImGui::DragFloat("Sprite_Speed", &m_tTexDesc.fSpriteSpeed, 0.25f, 0.f, 500.f);

							ImGui::TreePop();
						}

						ImGui::TreePop();
					}
				}


				static ImGuiComboFlags flags2 = 0;

				const char* items2[] = { "RENDER_PRIORITY", "RENDER_NONBLEND", "RENDER_NONLIGHT", "RENDER_BLEND", "RENDER_UI", "RENDER_STOPUI" };
				static int Current_ItemIdx2 = 3;
				const char* combo_preview_value2 = items2[Current_ItemIdx2];

				if (ImGui::BeginCombo("RenderGroup", combo_preview_value2, flags2))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items2); n++)
					{
						const bool is_selected = (Current_ItemIdx2 == n);

						if (ImGui::Selectable(items2[n], is_selected))
						{
							Current_ItemIdx2 = n;
							m_tTexDesc.eRenderGroup = (CRenderer::RENDERGROUP)Current_ItemIdx2;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				static ImGuiComboFlags flags3 = 0;

				const char* items3[] = { "TIME_LIMIT", "ALPHA_LIMIT", "SCALE_LIMIT", "DISTANCE_LIMIT", "SPRITE_LIMIT" };
				static int Current_ItemIdx3 = 0;
				const char* combo_preview_value3 = items3[Current_ItemIdx3];

				if (ImGui::BeginCombo("Texture_LifeType", combo_preview_value3, flags3))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items3); n++)
					{
						const bool is_selected = (Current_ItemIdx3 == n);

						if (ImGui::Selectable(items3[n], is_selected))
						{
							Current_ItemIdx3 = n;
							m_tTexDesc.eLifeType = (CEffect_Texture::LIFE_TYPE)Current_ItemIdx3;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (true == ImGui::CollapsingHeader("Transform_Setting"), ImGuiTreeNodeFlags_DefaultOpen)
				{
					ImGui::DragFloat("PlaySpeed", &m_tTexDesc.fPlaySpeed, 0.25f, 1.f, 50.f);


					ImGui::Checkbox("Spnning", &m_tTexDesc.bSpinning);

					ImGui::DragFloat("SpinningSpeed", &m_tTexDesc.fSpinSpeed, 0.25f, 0.f, 360.f);

					static _float fSpinAxis[3] = {0.f, 1.f, 0.f};
					ImGui::DragFloat3("SpinAxis", fSpinAxis, 0.25f, 1.f, 500.f);
					m_tTexDesc.vSpinAxis = XMVectorSet(fSpinAxis[0], fSpinAxis[1], fSpinAxis[2], 0.f);
				}

				if (true == ImGui::CollapsingHeader("Position"), ImGuiTreeNodeFlags_DefaultOpen)
				{
					if (ImGui::TreeNode("Pattern"))
					{
						static ImGuiComboFlags flags = 0;

						const char* items_Move[] = { "PATTERN_MOVE_NONE", "PATTERN_MOVE_ONEDIR", "PATTERN_MOVE_RANDDIR" };

						static int Current_ItemIdx2 = 0;
						const char* combo_preview_value2 = items_Move[Current_ItemIdx2];

						if (ImGui::BeginCombo("Moving", combo_preview_value2, flags))
						{
							for (int n = 0; n < IM_ARRAYSIZE(items_Move); n++)
							{
								const bool is_selected = (Current_ItemIdx2 == n);
								if (ImGui::Selectable(items_Move[n], is_selected))
								{
									Current_ItemIdx2 = n;
									m_tTexDesc.eMovingPattern = CEffect_Texture::PATTERN_MOVE(Current_ItemIdx2);
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}

							ImGui::EndCombo();
						}

						static _float	fPos[3] = {};

						ImGui::DragFloat3("Change_Position", fPos, 0.25f, -10.f, 10.f);
						m_tTexDesc.vPosition = _float3(fPos[0], fPos[1], fPos[2]);

						static _float	fDir[3] = {0.f, 0.f, 1.f};

						ImGui::DragFloat3("Direction", fDir, 0.25f, -500.f, 500.f);
						m_tTexDesc.vMovingDir = _float3(fDir[0], fDir[1], fDir[2]);
						
						ImGui::Checkbox("Easing", &m_tTexDesc.bUseEasing);

						if (true == m_tTexDesc.bUseEasing)
						{
							ImGui::SetNextItemOpen(true, ImGuiCond_Once);

							if (ImGui::TreeNode("Easing"))
							{
								static ImGuiComboFlags Easingflags = 0;

								const char* Easingitems[] = {
									"INSINE",		"OUTSINE",		"INOUTSINE",	"INQUAD",	"OUTQUAD",	"INOUTQUAD",
									"INCUBIC",		"OUTCUBIC",		"INOUTCUBIC",	"INQUART",	"OUTQUART",	"INOUTQUART",
									"INQUINT",		"OUTQUINT",		"INOUTQUINT",	"INEXPO",	"OUTEXPO",	"INOUTEXPO",
									"INCIRC",		"OUTCIRC",		"INOUTCIRC",	"INBACK",	"OUTBACK",	"INOUTBACK",
									"INELASTIC",	"OUTELASTIC",	"INOUTELASTIC", "INBOUNCE", "OUTBOUNCE", "INOUTBOUNCE"
								};

								static int Easing_current_idx = 0;

								const char* Easing_combo_preview_value = Easingitems[Easing_current_idx];

								if (ImGui::BeginCombo("Easing", Easing_combo_preview_value, Easingflags))
								{
									for (int n = 0; n < IM_ARRAYSIZE(Easingitems); n++)
									{
										const bool is_selected = (Easing_current_idx == n);
										if (ImGui::Selectable(Easingitems[n], is_selected))
										{
											Easing_current_idx = n;
											m_tTexDesc.eEasing = (CEffect_Texture::EASINGFUNCTION)Easing_current_idx;
										}

										if (is_selected)
											ImGui::SetItemDefaultFocus();
									}
									ImGui::EndCombo();
								}
								ImGui::TreePop();
							}
						}
						
						ImGui::TreePop();

					}
				}

				if (true == ImGui::CollapsingHeader("Scale"), ImGuiTreeNodeFlags_DefaultOpen)
				{
					static float fScale[3] = {1.f, 1.f, 1.f};

					ImGui::DragFloat3("Start_Scale", fScale, 0.25f, -100.f, 100.f);
					m_tTexDesc.vMinSize = _float3(fScale[0], fScale[1], fScale[2]);

					ImGui::Dummy(ImVec2(0.0f, 5.0f));
					ImGui::Separator();
					ImGui::Dummy(ImVec2(0.0f, 5.0f));

					static float fFinalScale[3] = {};

					ImGui::DragFloat3("Final_Scale", fFinalScale, 0.25f, -100.f, 100.f);
					m_tTexDesc.vMaxSize = _float3(fFinalScale[0], fFinalScale[1], fFinalScale[2]);

					ImGui::DragFloat("Scale_Speed", &m_tTexDesc.fScalingSpeed, 0.25f, -10.f, 100.f);

					//ImGui::DragFloat("Scale_Time", &m_tTexDesc.f, 0.25f, 1.f, 5.f);

					static ImGuiComboFlags flags = 0;
					
					const char* items[] = { "PATTERN_SCALE_NONE", "PATTERN_SCALE" , "PATTERN_SHRINK(SAME_AS_SCALE)", "PATTERN_DYNAMIC"};

					static int Current_ItemIdx = 0;
					const char* combo_preview_value = items[Current_ItemIdx];

					if (ImGui::BeginCombo("Scaling", combo_preview_value, flags))
					{
						for (int n = 0; n < IM_ARRAYSIZE(items); n++)
						{
							const bool is_selected = (Current_ItemIdx == n);
							if (ImGui::Selectable(items[n], is_selected))
							{
								Current_ItemIdx = n;
								m_tTexDesc.eScalePattern = CEffect_Texture::PATTERN_SCALE(Current_ItemIdx);
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					ImGui::Checkbox("Easing_Scale", &m_tTexDesc.bUseEasing_Scaling);

					if (m_tTexDesc.bUseEasing_Scaling)
					{
						ImGui::SetNextItemOpen(true, ImGuiCond_Once);

						if (ImGui::TreeNode("Easing_Scale"))
						{
							static ImGuiComboFlags Easingflags = 0;

							const char* Easingitems[] = {
								"INSINE",		"OUTSINE",		"INOUTSINE",	"INQUAD",	"OUTQUAD",	"INOUTQUAD",
								"INCUBIC",		"OUTCUBIC",		"INOUTCUBIC",	"INQUART",	"OUTQUART",	"INOUTQUART",
								"INQUINT",		"OUTQUINT",		"INOUTQUINT",	"INEXPO",	"OUTEXPO",	"INOUTEXPO",
								"INCIRC",		"OUTCIRC",		"INOUTCIRC",	"INBACK",	"OUTBACK",	"INOUTBACK",
								"INELASTIC",	"OUTELASTIC",	"INOUTELASTIC", "INBOUNCE", "OUTBOUNCE", "INOUTBOUNCE"
							};

							static int Easing_current_idx = 0;

							const char* Easing_combo_preview_value = Easingitems[Easing_current_idx];

							if (ImGui::BeginCombo("Easing", Easing_combo_preview_value, Easingflags))
							{
								for (int n = 0; n < IM_ARRAYSIZE(Easingitems); n++)
								{
									const bool is_selected = (Easing_current_idx == n);
									if (ImGui::Selectable(Easingitems[n], is_selected))
									{
										Easing_current_idx = n;
										m_tTexDesc.eEasing_Scale = (CEffect_Texture::EASINGFUNCTION)Easing_current_idx;
									}

									if (is_selected)
										ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}
							ImGui::TreePop();
						}
					}

				}

				if (true == ImGui::CollapsingHeader("Color"), ImGuiTreeNodeFlags_DefaultOpen)
				{
					static _float	BaseColor[4] = {0.f, 0.f, 0.f, 0.f};
					static _float	BlendingColor[4] = {1.f, 1.f, 1.f, 1.f};

					ImGui::DragFloat4("Change_BaseColor", BaseColor, 0.1f, -500.f, 500.f);
					m_tTexDesc.vBaseColor = _float4(BaseColor[0], BaseColor[1], BaseColor[2], BaseColor[3]);
					
					ImGui::DragFloat4("Change_BlendingColor", BlendingColor, 0.1f, -500.f, 500.f);
					m_tTexDesc.vBlendingColor = _float4(BlendingColor[0], BlendingColor[1], BlendingColor[2], BlendingColor[3]);
				}

				if (true == ImGui::CollapsingHeader("Transparency"), ImGuiTreeNodeFlags_DefaultOpen)
				{
					ImGui::DragFloat("Start_AlphaNum", &m_tTexDesc.fStartAlpha, 0.25f, 0.f, 1.f);
					ImGui::DragFloat("Transparency_Speed", &m_tTexDesc.fTransparecySpeed, 0.25f, -100.f, 100.f);
					ImGui::DragFloat("MaxTransparency", &m_tTexDesc.fMaxTransparecy, 0.25f, -100.f, 100.f);
				}
			}

			if (CGameInstance::GetInstance()->Get_KeyEnter(DIK_SPACE))
			{
				CGameInstance*		pInstance = GET_INSTANCE(CGameInstance);

				CHECK_FAILED(pInstance->Add_GameObject(LEVEL_EFFECTTOOL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &m_tTexDesc));			 
			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (true == ImGui::Button("Save EffectDesc", ImVec2(-FLT_MIN, 0.f)))
			{
				CEffect_Texture::EFFECT_TEX_DESC			tDesc;

				memcpy(&tDesc, &m_tTexDesc, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

				m_TexDesc.push_back(tDesc);
			}
			ImGui::EndTabItem();
		}

		//Particle_Tab ==========================================================================================================================================================
		if (ImGui::BeginTabItem("Effect_Particle"))
		{
			ImGui::InputInt("TexIndex", &m_tParticleDesc.iTexIndex);

			if (m_pGameInstance->Get_KeyEnter(DIK_LEFT))
				--m_tParticleDesc.iTexIndex;
			if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT))
				++m_tParticleDesc.iTexIndex;

			if (g_iEffectTexCnt - 1 < m_tParticleDesc.iTexIndex)
				m_tParticleDesc.iTexIndex = g_iEffectTexCnt - 1;

			else if (0 > m_tParticleDesc.iTexIndex)
				m_tParticleDesc.iTexIndex = 0;

			ImGui::DragFloat("MinSpeed", &m_tParticleDesc.fMinSpeed, 0.01f, -500.f, 500.f);
			ImGui::DragFloat("MaxSpeed", &m_tParticleDesc.fMaxSpeed, 0.01f, -500.f, 500.f);
			ImGui::DragFloat("OriginScale", &m_tParticleDesc.fOriginScale, 0.01f, -50.f, 50.f);
			ImGui::DragFloat("ScaleSpeed", &m_tParticleDesc.fScaleSpeed, 0.01f, -500.f, 500.f);
			ImGui::DragFloat("GravityPower", &m_tParticleDesc.fGravityPower, 0.01f, -100.f, 100.f);
			ImGui::DragFloat("LifeTime", &m_tParticleDesc.fLiveTime, 0.01f, -3000.f, 3000.f);

			ImGui::Checkbox("Range_One_Or_XYZ", &m_tParticleDesc.bRangeOneOrXYZ);
			if (false == m_tParticleDesc.bRangeOneOrXYZ)
				ImGui::DragFloat("MakeRange", &m_tParticleDesc.fMakeRange, 0.01f, -100.f, 100.f);
			else
			{
				_float	fRange[3] = {};
				memcpy(fRange, &m_tParticleDesc.vMakeRangeXYZ, sizeof(_float3));
				ImGui::DragFloat3("MakeRangeXYZ", fRange, 0.01f, -100.f, 100.f);
				memcpy(&m_tParticleDesc.vMakeRangeXYZ, fRange, sizeof(_float3));
			}

			ImGui::Checkbox("Explosion : Seperate_Axis_Range", &m_tParticleDesc.bSeperateAxisRange);

			if (true == m_tParticleDesc.bSeperateAxisRange)
			{
				_float	fRange[3] = {};
				memcpy(fRange, &m_tParticleDesc.vAxisRangeXYZ, sizeof(_float3));
				ImGui::DragFloat3("Axis_Range_XYZ", fRange, 0.1f, 0.f, 360.f);
				memcpy(&m_tParticleDesc.vAxisRangeXYZ, fRange, sizeof(_float3));

				static ImGuiComboFlags flags = 0;

				const char* items[] = { "STATE_RIGHT", "STATE_UP", "STATE_LOOK"};
				static int Current_ItemIdx = 2;
				const char* combo_preview_value = items[Current_ItemIdx];

				if (ImGui::BeginCombo("DIR", combo_preview_value, flags))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						const bool is_selected = (Current_ItemIdx == n);
						if (ImGui::Selectable(items[n], is_selected))
						{
							Current_ItemIdx = n;
							m_tParticleDesc.eState = (CEffect_Particle::STATE)Current_ItemIdx;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}

			ImGui::DragFloat("MaxDistance", &m_tParticleDesc.fMaxDistance, 0.01f, -100.f, 100.f);

			if (ImGui::TreeNode("DIR"))
			{
				static ImGuiComboFlags flags = 0;

				const char* items[] = { "DIR_ONE", "DIR_RANDOM" };
				static int Current_ItemIdx = 0;
				const char* combo_preview_value = items[Current_ItemIdx];

				if (ImGui::BeginCombo("DIR", combo_preview_value, flags))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						const bool is_selected = (Current_ItemIdx == n);
						if (ImGui::Selectable(items[n], is_selected))
						{
							Current_ItemIdx = n;
							m_tParticleDesc.eDir = (CEffect_Particle::DIR)Current_ItemIdx;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::TreePop();
			}

			if (CEffect_Particle::DIR_ONE == m_tParticleDesc.eDir)
			{
				static _float	fDir[3] = {};
				ImGui::DragFloat3("Dir", fDir, 0.01f, -500.f, 500.f);
				m_tParticleDesc.vDir = XMVector4Normalize(XMVectorSet(fDir[0], fDir[1], fDir[2], 0.f));
			}

			static _float	fPosition[3] = {};
			ImGui::DragFloat3("Position", fPosition, 0.01f, -500.f, 500.f);
			m_vParticlePosition = _float4(fPosition[0], fPosition[1], fPosition[2], 1.f);
			m_tParticleDesc.pParentPosition = &m_vParticlePosition;

			if (ImGui::TreeNode("ParticleNumber"))
			{
				static ImGuiComboFlags flags = 0;

				const char* items[] = { "10", "30", "50", "100" };
				static int Current_ItemIdx = 0;
				const char* combo_preview_value = items[Current_ItemIdx];
				if (ImGui::BeginCombo("ParticleNumber", combo_preview_value, flags))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						const bool is_selected = (Current_ItemIdx == n);
						if (ImGui::Selectable(items[n], is_selected))
						{
							Current_ItemIdx = n;
							switch (Current_ItemIdx)
							{
							case 0:
								m_tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;
								break;
							case 1:
								m_tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;
								break;
							case 2:
								m_tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
								break;
							case 3:
								m_tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;
								break;
							}
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::TreePop();
			}

			ImGui::Checkbox("UseEasing", &m_tParticleDesc.bUseEasing);


			if (true == m_tParticleDesc.bUseEasing)
			{
				if (ImGui::TreeNode("Easing"))
				{
					static ImGuiComboFlags Easingflags = 0;

					const char* Easingitems[] = {
						"INSINE",		"OUTSINE",		"INOUTSINE",	"INQUAD",	"OUTQUAD",	"INOUTQUAD",
						"INCUBIC",		"OUTCUBIC",		"INOUTCUBIC",	"INQUART",	"OUTQUART",	"INOUTQUART",
						"INQUINT",		"OUTQUINT",		"INOUTQUINT",	"INEXPO",	"OUTEXPO",	"INOUTEXPO",
						"INCIRC",		"OUTCIRC",		"INOUTCIRC",	"INBACK",	"OUTBACK",	"INOUTBACK",
						"INELASTIC",	"OUTELASTIC",	"INOUTELASTIC", "INBOUNCE", "OUTBOUNCE", "INOUTBOUNCE"
					};

					static int Easing_current_idx = 0;

					const char* Easing_combo_preview_value = Easingitems[Easing_current_idx];

					if (ImGui::BeginCombo("Easing", Easing_combo_preview_value, Easingflags))
					{
						for (int n = 0; n < IM_ARRAYSIZE(Easingitems); n++)
						{
							const bool is_selected = (Easing_current_idx == n);
							if (ImGui::Selectable(Easingitems[n], is_selected))
							{
								Easing_current_idx = n;
								m_tParticleDesc.eEasing = (CEffect_Particle::EASINGFUNCTION)Easing_current_idx;
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::TreePop();
				}
			}

			ImGui::Checkbox("Chase", &m_tParticleDesc.bChase);
			ImGui::Checkbox("Loop", &m_tParticleDesc.bLoop);
			ImGui::Checkbox("Trail", &m_tParticleDesc.bTrail);
			ImGui::Checkbox("Explosion", &m_tParticleDesc.bExplosion);

			if (true == m_tParticleDesc.bTrail)
				ImGui::DragFloat("Trail_Interval", &m_tParticleDesc.fTrailInterval, 0.01f, 0.f, 10.f);

			if (true == m_tParticleDesc.bExplosion)
				ImGui::DragFloat("Explosion Start Range", &m_tParticleDesc.fExplosionStartRange, 0.01f, 0.f, 100.f);

			ImGui::DragFloat4("AddColor", m_tParticleDesc.fAddColor, 0.01f, -1.f, 1.f);
			ImGui::DragFloat4("MulColor", m_tParticleDesc.fMulColor, 0.01f, 0.f, 10.f);

			ImGui::Checkbox("Blur", &m_tParticleDesc.bBlur);

			static ImGuiComboFlags flags3 = 0;

			const char* items3[] = { "TIME_LIMIT", "ALPHA_LIMIT", "SCALE_LIMIT", "DISTANCE_LIMIT", "SPRITE_LIMIT" };
			static int Current_ItemIdx3 = 0;
			const char* combo_preview_value3 = items3[Current_ItemIdx3];

			if (ImGui::BeginCombo("Particle_Life_Type", combo_preview_value3, flags3))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items3); n++)
				{
					const bool is_selected = (Current_ItemIdx3 == n);

					if (ImGui::Selectable(items3[n], is_selected))
					{
						Current_ItemIdx3 = n;
						m_tParticleDesc.eLifeType = (CEffect_Particle::LIFE_TYPE)Current_ItemIdx3;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (m_pGameInstance->Get_KeyEnter(DIK_SPACE))
			{
				CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

				m_tParticleDesc.iLevelIndex = LEVEL_EFFECTTOOL;

				if (FAILED(pGameInstance->Add_GameObject(LEVEL_EFFECTTOOL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &m_tParticleDesc)))
				{
					MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Particle_Info");
					return RESULT_ERROR;
				}
			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (true == ImGui::Button("Save EffectDesc", ImVec2(-FLT_MIN, 0.f)))
			{
				CEffect_Particle::PARTICLEDESC			tDesc;

				memcpy(&tDesc, &m_PtclDesc, sizeof(CEffect_Particle::PARTICLEDESC));

				m_PtclDesc.push_back(tDesc);
			}

			ImGui::EndTabItem();
		}

		//Mesh_Tab ==========================================================================================================================================================
		if (ImGui::BeginTabItem("Effect_Mesh"))
		{
			// 모델&텍스쳐
			if (true == ImGui::CollapsingHeader("Model & Texture"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				
			/*	string items[7];

				for (_uint i = 0; i < 336; ++i)
				{
					string		szTextureBaseTag = "Effect_";
					szTextureBaseTag += to_string(i);
					items[i] = szTextureBaseTag;
				}
*/
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);

				string items_Noise[99];

				for (_uint i = 0; i < 99; ++i)
				{
					string		szTextureBaseTag = "Noise_";
					szTextureBaseTag += to_string(i);
					items_Noise[i] = szTextureBaseTag;
				}
				
				static int item_current_idx = 0;
				static int item_current_idx_Model = 0;
				static int item_current_idx_Source = 0;

				if (ImGui::TreeNode("Model"))
				{
					if (ImGui::BeginListBox("##ModelTag", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
					{
						Select_EffectMesh();
						ImGui::EndListBox();
					}

					ImGui::TreePop();
				}

				ImGui::SetNextItemOpen(true, ImGuiCond_Once);

				if (ImGui::TreeNode("Texture"))
				{
					ImGui::SetNextItemOpen(true, ImGuiCond_Once);

					auto iTexIndex = g_iEffectTexCnt - 1;

					ImGui::DragInt("TextureIndex", &m_tEffectMeshDesc.iTextureIndex, 1, 0, iTexIndex);

					if (m_pGameInstance->Get_KeyEnter(DIK_RIGHT))
					{
						if ((_uint)++m_tEffectMeshDesc.iTextureIndex > iTexIndex)
							m_tEffectMeshDesc.iTextureIndex = 0;
					}
					if (m_pGameInstance->Get_KeyEnter(DIK_LEFT))
					{
						if (--m_tEffectMeshDesc.iTextureIndex < 0)
							m_tEffectMeshDesc.iTextureIndex = iTexIndex;
					}

					if (ImGui::TreeNode("Texture_Mix"))
					{
						ImGui::SameLine();
						ImGui::Text("(Noise Texture Index : %d)", m_tEffectMeshDesc.iScondTextureIndex);

						if (ImGui::BeginListBox("##TextureTag_Source", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
						{
							for (int n = 0; n < IM_ARRAYSIZE(items_Noise); n++)
							{
								const bool is_selected = (item_current_idx_Source == n);
								if (ImGui::Selectable(items_Noise[n].c_str(), is_selected))
								{
									item_current_idx_Source = n;
									m_tEffectMeshDesc.iScondTextureIndex = n;
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}

							if (m_pGameInstance->Get_KeyEnter(DIK_UP))
							{
								if (++m_tEffectMeshDesc.iScondTextureIndex > IM_ARRAYSIZE(items_Noise))
									m_tEffectMeshDesc.iScondTextureIndex = 0;
							}
							if (m_pGameInstance->Get_KeyEnter(DIK_DOWN))
							{
								if (--m_tEffectMeshDesc.iScondTextureIndex < 0)
									m_tEffectMeshDesc.iScondTextureIndex = IM_ARRAYSIZE(items_Noise);
							}

							ImGui::EndListBox();
						}

					
						
						static ImGuiComboFlags flags = 0;

						const char* items[] = { "MIX_NONE", "MIX_ADD", "MIX_MULT", "MIX_NOISING" };
						static int Current_ItemIdx = 1;
						const char* combo_preview_value = items[Current_ItemIdx];

						if (ImGui::BeginCombo("Select_MixTextureType", combo_preview_value, flags))
						{
							for (int n = 0; n < IM_ARRAYSIZE(items); n++)
							{
								const bool is_selected = (Current_ItemIdx == n);

								if (ImGui::Selectable(items[n], is_selected))
								{
									Current_ItemIdx = n;
									m_tEffectMeshDesc.ePattern_MixTex = (CEffect_Mesh::PATTERN_MIXTEX)Current_ItemIdx;
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}

						if (m_tEffectMeshDesc.ePattern_MixTex == CEffect_Mesh::MIX_NOISING)
						{
							if (ImGui::TreeNode("Noise_Desc"))
							{
								ImGui::DragFloat("Noise_Power", &m_tEffectMeshDesc.fNoisePower, 0.25, 0.f, 100.f);
								
								static _float	fNoiseSpeed[2] = {};
								ImGui::DragFloat2("Noise_Speed", fNoiseSpeed, 0.25f, -500.f, 500.f);
								m_tEffectMeshDesc.vNoiseMovingSpeed = _float2(fNoiseSpeed[0], fNoiseSpeed[1]);

								ImGui::TreePop();
							}
						}

					ImGui::TreePop();
					}
					ImGui::TreePop();
				}
			}

			// DeafultSetting
			if (true == ImGui::CollapsingHeader("Deafult_Setting"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);

				if (ImGui::TreeNode("PlayDesc"))
				{
					// LifeTime
					ImGui::DragFloat("Life_Time", &m_tEffectMeshDesc.fLifeTime, 0.25, 1, 100);
					
					// Position
					static _float	fPosition[3] = {};
					ImGui::DragFloat3("Position", fPosition, 0.25f, -500.f, 500.f);
					m_vEffectMeshPosition = _float4(fPosition[0], fPosition[1], fPosition[2], 1.f);
					m_tEffectMeshDesc.pParentPos = &m_vEffectMeshPosition;
					m_tEffectMeshDesc.vPos = m_vEffectMeshPosition;

					//LookDir
					static _float	fLook[3] = {0.f, 0.f, -1.f};
					ImGui::DragFloat3("Look_Dir", fLook, 0.25f, -500.f, 500.f);
					m_tEffectMeshDesc.vLookDir = XMVectorSet(fLook[0], fLook[1], fLook[2], 0.f);

					static _float	fRoation[3] = {};
					ImGui::DragFloat3("RotationAngle", fRoation, 0.25f, -500.f, 500.f);
					m_tEffectMeshDesc.fRotationAngle = _float3(fRoation[0], fRoation[1], fRoation[2]);

					// PassIndex
					ImGui::DragInt("Pass_Index", &m_tEffectMeshDesc.iPassIndex, 1, 0, 100);

					ImGui::DragFloat("Transparency(Alpha)", &m_tEffectMeshDesc.fTransparency, 0.01f, 0.f, 1.f);
					ImGui::DragFloat("TransparencySpeed(Alpha)", &m_tEffectMeshDesc.fTransparecySpeed, 0.25f, -100.f, 100.f);
					ImGui::DragFloat("TransparencyMax(Alpha)", &m_tEffectMeshDesc.fMaxTransparecy, 0.25f, -100.f, 100.f);

					ImGui::Checkbox("AlphaReverse", &m_tEffectMeshDesc.bAlphaReverse);

					// Spinning
					ImGui::Checkbox("Spinning", &m_tEffectMeshDesc.bSpinning);
					ImGui::DragFloat("SpinningSpeed", &m_tEffectMeshDesc.fSpinningSpeed, 0.25f, 1.f, 100.f);

					static _float	fAxis[3] = {0.f, 1.f, 0.f};
					ImGui::DragFloat3("SpinningAxis", fAxis, 0.25f, 1.f, 100.f);
					m_tEffectMeshDesc.vSpinAxis = XMVectorSet(fAxis[0], fAxis[1], fAxis[2], 0.f);

					// Billboarding
					ImGui::Checkbox("Billboarding", &m_tEffectMeshDesc.bBillboarding);
					
					static ImGuiComboFlags flags3 = 0;

					const char* items3[] = { "TIME_LIMIT", "ALPHA_LIMIT", "SCALE_LIMIT", "DISTANCE_LIMIT", "SPRITE_LIMIT" };
					static int Current_ItemIdx3 = 0;
					const char* combo_preview_value3 = items3[Current_ItemIdx3];

					if (ImGui::BeginCombo("Mesh_LifeType", combo_preview_value3, flags3))
					{
						for (int n = 0; n < IM_ARRAYSIZE(items3); n++)
						{
							const bool is_selected = (Current_ItemIdx3 == n);

							if (ImGui::Selectable(items3[n], is_selected))
							{
								Current_ItemIdx3 = n;
								m_tEffectMeshDesc.eLifeType = (CEffect_Mesh::LIFE_TYPE)Current_ItemIdx3;
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::TreePop();
				}

				// Scale
				ImGui::SetNextItemOpen(true, ImGuiCond_Once); // 트리노드 자동으로 열리게 해주는 함수

				if (ImGui::TreeNode("Scale"))
				{

					static _float	fScale[3] = {0.05f, 0.05f, 0.05f};
					ImGui::DragFloat3("Start_Scale", fScale, 0.25f, -500.f, 500.f);
					m_tEffectMeshDesc.vStartScale = XMVectorSet(fScale[0], fScale[1], fScale[2], 1.f);

					static _float	fScale2[3] = {1.f, 1.f, 1.f};
					ImGui::DragFloat3("Max_Scale", fScale2, 0.25f, -500.f, 500.f);
					m_tEffectMeshDesc.vMaxScale = XMVectorSet(fScale2[0], fScale2[1], fScale2[2], 1.f);

					ImGui::DragFloat("Scaling_Speed", &m_tEffectMeshDesc.fScaleSpeed, 0.25f, -200.f, 200.f);
					
					ImGui::Checkbox("UseEasing_Scale", &m_tEffectMeshDesc.bUseEasing_Scaling);

					if (ImGui::TreeNode("Easing"))
					{
						static ImGuiComboFlags Easingflags = 0;

						const char* Easingitems[] = {
							"INSINE",		"OUTSINE",		"INOUTSINE",	"INQUAD",	"OUTQUAD",	"INOUTQUAD",
							"INCUBIC",		"OUTCUBIC",		"INOUTCUBIC",	"INQUART",	"OUTQUART",	"INOUTQUART",
							"INQUINT",		"OUTQUINT",		"INOUTQUINT",	"INEXPO",	"OUTEXPO",	"INOUTEXPO",
							"INCIRC",		"OUTCIRC",		"INOUTCIRC",	"INBACK",	"OUTBACK",	"INOUTBACK",
							"INELASTIC",	"OUTELASTIC",	"INOUTELASTIC", "INBOUNCE", "OUTBOUNCE", "INOUTBOUNCE"
						};

						static int Easing_current_idx = 0;

						const char* Easing_combo_preview_value = Easingitems[Easing_current_idx];

						if (ImGui::BeginCombo("Easing_Scale", Easing_combo_preview_value, Easingflags))
						{
							for (int n = 0; n < IM_ARRAYSIZE(Easingitems); n++)
							{
								const bool is_selected = (Easing_current_idx == n);
								if (ImGui::Selectable(Easingitems[n], is_selected))
								{
									Easing_current_idx = n;
									m_tEffectMeshDesc.eEasing_Scale = (CEffect_Mesh::EASINGFUNCTION)Easing_current_idx;
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				//패턴 설정
				static ImGuiComboFlags flags2 = 0;

				const char* items2[] = { "NO_PATTERN", "SCALING" };
				static int Current_ItemIdx2 = 0;
				const char* combo_preview_value2 = items2[Current_ItemIdx2];

				if (ImGui::BeginCombo("PATTERN", combo_preview_value2, flags2))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items2); n++)
					{
						const bool is_selected2 = (Current_ItemIdx2 == n);

						if (ImGui::Selectable(items2[n], is_selected2))
						{
							Current_ItemIdx2 = n;
							m_tEffectMeshDesc.ePattern_Scale = (CEffect_Mesh::PATTERN_SCALE)Current_ItemIdx2;
						}

						if (is_selected2)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				// Dir
				if (ImGui::TreeNode("Moving_Direction"))
				{
					static ImGuiComboFlags flags = 0;

					const char* items[] = { "NO_PATTERN", "MOVE_IN_ONEDIR", "MOVE_IN_RANDOM_DIR" };
					static int Current_ItemIdx = 0;
					const char* combo_preview_value = items[Current_ItemIdx];
					
					if (ImGui::BeginCombo("DIR_TYPE", combo_preview_value, flags))
					{
						for (int n = 0; n < IM_ARRAYSIZE(items); n++)
						{
							const bool is_selected = (Current_ItemIdx == n);

							if (ImGui::Selectable(items[n], is_selected))
							{
								Current_ItemIdx = n;
								m_tEffectMeshDesc.ePattern_Move = (CEffect_Mesh::PATTERN_MOVE)Current_ItemIdx;
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					static _float	vDir[3] = {};
					ImGui::DragFloat3("Direction", vDir, 0.25f, -500.f, 500.f);
					m_tEffectMeshDesc.vDir = XMVectorSet(vDir[0], vDir[1], vDir[2], 1.f);
				
					ImGui::DragFloat("Distance", &m_tEffectMeshDesc.fMovingDistance, 0.25f, 1.f, 100.f);
					ImGui::DragFloat("Moving_Speed", &m_tEffectMeshDesc.fMovingSpeed, 0.25f, -100.f, 100.f);

					

					ImGui::Checkbox("UsingEasing", &m_tEffectMeshDesc.bUseEasing);

					if (ImGui::TreeNode("Easing"))
					{
						static ImGuiComboFlags Easingflags = 0;

						const char* Easingitems[] = {
							"INSINE",		"OUTSINE",		"INOUTSINE",	"INQUAD",	"OUTQUAD",	"INOUTQUAD",
							"INCUBIC",		"OUTCUBIC",		"INOUTCUBIC",	"INQUART",	"OUTQUART",	"INOUTQUART",
							"INQUINT",		"OUTQUINT",		"INOUTQUINT",	"INEXPO",	"OUTEXPO",	"INOUTEXPO",
							"INCIRC",		"OUTCIRC",		"INOUTCIRC",	"INBACK",	"OUTBACK",	"INOUTBACK",
							"INELASTIC",	"OUTELASTIC",	"INOUTELASTIC", "INBOUNCE", "OUTBOUNCE", "INOUTBOUNCE"
						};

						static int Easing_current_idx = 0;

						const char* Easing_combo_preview_value = Easingitems[Easing_current_idx];

						if (ImGui::BeginCombo("Easing", Easing_combo_preview_value, Easingflags))
						{
							for (int n = 0; n < IM_ARRAYSIZE(Easingitems); n++)
							{
								const bool is_selected = (Easing_current_idx == n);
								if (ImGui::Selectable(Easingitems[n], is_selected))
								{
									Easing_current_idx = n;
									m_tEffectMeshDesc.eEasing = (CEffect_Mesh::EASINGFUNCTION)Easing_current_idx;
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}

				static ImGuiComboFlags flags = 0;

				const char* items[] = { "RENDER_PRIORITY", "RENDER_NONBLEND", "RENDER_NONLIGHT", "RENDER_BLEND", "RENDER_UI", "RENDER_STOPUI"};
				static int Current_ItemIdx = 3;
				const char* combo_preview_value = items[Current_ItemIdx];

				if (ImGui::BeginCombo("RenderGroup", combo_preview_value, flags))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						const bool is_selected = (Current_ItemIdx == n);

						if (ImGui::Selectable(items[n], is_selected))
						{
							Current_ItemIdx = n;
							m_tEffectMeshDesc.eRenderGroup = (CRenderer::RENDERGROUP)Current_ItemIdx;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}

			// TypeSetting
			if (true == ImGui::CollapsingHeader("Type_Setting"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				//플레이 타입 설정
				static ImGuiComboFlags flags = 0;

				const char* items[] = { "PLAY_ONCE", "PLAY_LOOP" };
				static int Current_ItemIdx = 0;
				const char* combo_preview_value = items[Current_ItemIdx];

				if (ImGui::BeginCombo("PLAY_TYPE", combo_preview_value, flags))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						const bool is_selected = (Current_ItemIdx == n);

						if (ImGui::Selectable(items[n], is_selected))
						{
							Current_ItemIdx = n;
							m_tEffectMeshDesc.ePlayType = (CEffect_Mesh::PLAY_TYPE)Current_ItemIdx;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				

				//UV 설정
				ImGui::Checkbox("MovingUV", &m_tEffectMeshDesc.bMovingUV);

				static _float	vUVSpeed[2] = {};
				ImGui::DragFloat2("UV_MovingSpeed", vUVSpeed, 0.25f, -500.f, 500.f);
				m_tEffectMeshDesc.vUVMovingSpeed = _float2(vUVSpeed[0], vUVSpeed[1]);


				static _float	vStartUV[2] = {};
				ImGui::DragFloat2("StartUV", vStartUV, 0.1f, -1.f, 1.f);
				m_tEffectMeshDesc.fStartU = vStartUV[0];
				m_tEffectMeshDesc.fStartV = vStartUV[1];

				//UsingShaderEffect
				ImGui::Checkbox("Emissive", &m_tEffectMeshDesc.bUsingEmissive);
				ImGui::Checkbox("Blur", &m_tEffectMeshDesc.bUsingBlur);
				ImGui::Checkbox("Distortion", &m_tEffectMeshDesc.bUsingDistortion);
			}

			// ColorSetting
			if (true == ImGui::CollapsingHeader("Color"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);

				static ImGuiComboFlags flags2 = 0;

				const char* items2[] = { "NONE", "ADD_COLOR", "MULT_COLOR", "COVER_COLOR", "ONEVALUE_BLEND_COLOR" };
				static int Current_ItemIdx2 = 1;
				const char* combo_preview_value2 = items2[Current_ItemIdx2];

				if (ImGui::BeginCombo("Dying_Type", combo_preview_value2, flags2))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items2); n++)
					{
						const bool is_selected2 = (Current_ItemIdx2 == n);

						if (ImGui::Selectable(items2[n], is_selected2))
						{
							Current_ItemIdx2 = n;
							m_tEffectMeshDesc.eDyingPattern = (CEffect_Mesh::PATTERN_DYING)Current_ItemIdx2;
						}

						if (is_selected2)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (m_tEffectMeshDesc.eDyingPattern == CEffect_Mesh::DYING_ONEVALUE)
				{
					ImGui::SetNextItemOpen(true, ImGuiCond_Once);

					if (ImGui::TreeNode("Select_RGB"))
					{
						static ImGuiComboFlags flags = 0;

						const char* items[] = { "R", "G", "B", "A" };
						static int Current_ItemIdx = 0;
						const char* combo_preview_value = items[Current_ItemIdx];

						if (ImGui::BeginCombo("Select_TexureRGB", combo_preview_value, flags))
						{
							for (int n = 0; n < IM_ARRAYSIZE(items); n++)
							{
								const bool is_selected = (Current_ItemIdx == n);

								if (ImGui::Selectable(items[n], is_selected))
								{
									Current_ItemIdx = n;
									m_tEffectMeshDesc.iRGBNum = Current_ItemIdx;
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						ImGui::TreePop();
					}
				}

				static _float	vColor[4] = { 0.f, 0.f, 0.f, 0.f };
				ImGui::DragFloat4("Color", vColor, 0.1f, -100.f, 100.f);
				XMStoreFloat4(&m_tEffectMeshDesc.vColor, XMVectorSet(    vColor[0], vColor[1], vColor[2], vColor[3]));

				static _float	vBlendingColor[4] = { 0.f, 0.f, 0.f, 0.f };
				ImGui::DragFloat4("BlendingColor", vBlendingColor, 0.1f, -100.f, 100.f);
				XMStoreFloat4(&m_tEffectMeshDesc.vBlendColor, XMVectorSet(vBlendingColor[0], vBlendingColor[1], vBlendingColor[2], vBlendingColor[3]));
			}
			m_tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();
			if (CGameInstance::GetInstance()->Get_KeyEnter(DIK_SPACE))
			{
				CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

				m_tEffectMeshDesc.iLevelIndex = LEVEL_EFFECTTOOL;

				if (FAILED(pGameInstance->Add_GameObject(LEVEL_EFFECTTOOL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &m_tEffectMeshDesc)))
				{
					MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Particle_Info");
					return RESULT_ERROR;
				}

			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (true == ImGui::Button("Save EffectDesc", ImVec2(-FLT_MIN, 0.f)))
			{
				CEffect_Mesh::EFFECT_MESHDESC		tDesc;

				memcpy(&tDesc, &m_tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

				m_MeshDesc.push_back(tDesc);
			}

			ImGui::EndTabItem();
		}

	ImGui::EndTabBar();
	}

	ImGui::End();

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_KeyEnter(DIK_C))
	{
		m_bControlBox = !m_bControlBox;
	}

	if (true == m_bControlBox)
	{
		ImGui::SetNextWindowBgAlpha(0.7f);
		ImGui::Begin("Effect_Tool_Data");
		
		if (ImGui::TreeNode("Current_PlayingObject"))
		{
		_uint iObjectNumber = 0;

		map<const _tchar*, CLayer*> Object = m_pGameInstance->Get_ObjectMap(GM->Get_CurrentLevel());

		for (auto& Pair : Object)
		{
			const _tchar* wLayerTag = Pair.first;
			_bstr_t LayerTag(wLayerTag);

			if (0 == lstrcmp(wLayerTag, TEXT("Layer_Effect")))
			{
				CLayer* pLayer = Pair.second;
				list<CGameObject*> GameObjectList = *(pLayer->Get_Object());

				auto iter = GameObjectList.begin();

				for (_uint i = 0; i < GameObjectList.size(); i++)
				{
					char buf[MAX_PATH];
					sprintf_s(buf, "Object_%d", iObjectNumber);

					// 버튼을 누르면 삭제되게
					if (ImGui::SmallButton(buf))
					{
						(*iter)->Set_Dead(true);
					}

					++iObjectNumber;
					++iter;
				}
			}

			else
				continue;


		}
		ImGui::TreePop();
	}

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		// Effect저장된거 보기
		
		if (ImGui::TreeNode("SavedDesc"))
		{
			_uint iTexNumber = 0;

			auto iter_Tex = m_TexDesc.begin();

			for (_uint i = 0; i < m_TexDesc.size(); i++)
			{

				char buf[MAX_PATH];
				sprintf_s(buf, "Tex_%d", iTexNumber);

				if (ImGui::SmallButton(buf))
				{
					iter_Tex = m_TexDesc.erase(iter_Tex);
				}

				else
					++iter_Tex;

				++iTexNumber;
			}


			ImGui::TableNextColumn();

			_uint iPtclNumber = 0;

			auto iter_Ptcl = m_PtclDesc.begin();

			for (_uint i = 0; i < m_PtclDesc.size(); i++)
			{

				char buf[MAX_PATH];
				sprintf_s(buf, "Ptcl_%d", iPtclNumber);

				if (ImGui::SmallButton(buf))
				{
					iter_Ptcl = m_PtclDesc.erase(iter_Ptcl);
				}

				else
					++iter_Ptcl;

				++iPtclNumber;
			}

			ImGui::TableNextColumn();

			_uint iMeshNum = 0;

			auto iter_Mesh = m_MeshDesc.begin();

			for (_uint i = 0; i < m_MeshDesc.size(); i++)
			{

				char buf[MAX_PATH];
				sprintf_s(buf, "Mesh_%d", iMeshNum);

				if (ImGui::SmallButton(buf))
				{
					iter_Mesh = m_MeshDesc.erase(iter_Mesh);
				}

				else
					++iter_Mesh;

				++iMeshNum;
			}
			ImGui::TreePop();
		}

		ImGui::TableNextColumn();

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		static _bool	isPlay = false;

		if(true == ImGui::Button("Play Effect", ImVec2(-FLT_MIN, 0.f)))
		{
			for (auto& tDesc : m_TexDesc)
			{
				CGameInstance*		pInstance = GET_INSTANCE(CGameInstance);

				CHECK_FAILED(pInstance->Add_GameObject(LEVEL_EFFECTTOOL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tDesc));
			}

			for (auto& tDesc : m_PtclDesc)
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_EFFECTTOOL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tDesc)))
				{
					MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Particle_Info");
					return RESULT_ERROR;
				}
			}

			for (auto& tDesc : m_MeshDesc)
			{
				if (FAILED(pGameInstance->Add_GameObject(LEVEL_EFFECTTOOL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tDesc)))
				{
					MSGBOX("pGameInstance->Add_GameObject returned E_FAIL in CImGui_Manager::Particle_Info");
					return RESULT_ERROR;
				}
			}
		}

		ImGui::End();
	}

	return 0;
}

_int CLevel_EffectTool::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	return 0;
}

HRESULT CLevel_EffectTool::Render()
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

				XMFLOAT4 vDiffuse = pLight->vDiffuse;
				XMFLOAT4 vAmbient = pLight->vAmbient;
				XMFLOAT4 vSpecualr = pLight->vSpecular;
				XMFLOAT4 vDirection = pLight->vDirection;

				_float diffuse[4] = { vDiffuse.x, vDiffuse.y, vDiffuse.z, vDiffuse.w };
				_float Ambient[4] = { vAmbient.x, vAmbient.y, vAmbient.z, vAmbient.w };
				_float Specualr[4] = { vSpecualr.x, vSpecualr.y, vSpecualr.z, vSpecualr.w };
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

				XMFLOAT4 vDiffuse = pLight->vDiffuse;
				XMFLOAT4 vAmbient = pLight->vAmbient;
				XMFLOAT4 vSpecualr = pLight->vSpecular;
				XMFLOAT4 vDirection = pLight->vDirection;

				_float diffuse = vDiffuse.x;
				_float Ambient = vAmbient.x;
				_float Specualr = vSpecualr.x;
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
			if (ImGui::BeginTabItem("Load"))
			{
				CImGui_Manager::GetInstance()->ColoredButtonBegin(200, 50, 0);
				if (ImGui::Button("Reset", ImVec2(-FLT_MIN, 30.f)))
				{
					// 맵 리스트를 가져옴
					list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_EFFECTTOOL, TEXT("Layer_Map"));

					// 맵 레이어가 없거나 맵 리스트에 아무것도 없으면 패스
					if (MapList != nullptr && MapList->size() != 0)
					{
						// 전부 제거
						auto iter = (*MapList).begin();
						for (_uint i = 0; i < (*MapList).size(); i++)
						{
							(*iter)->Set_Dead(true);
							++iter;
						}
					}

					list<CGameObject*>* TerrainList = m_pGameInstance->Get_ObjectList(LEVEL_EFFECTTOOL, TEXT("Layer_Terrain"));
					if (TerrainList != nullptr && TerrainList->size() != 0)
					{
						// 전부 제거
						auto iter = (*TerrainList).begin();
						for (_uint i = 0; i < (*TerrainList).size(); i++)
						{
							(*iter)->Set_Dead(true);
							++iter;
						}
					}
				}
				CImGui_Manager::GetInstance()->ColoredButtonEnd();

				if (ImGui::Button("Twilight Town", ImVec2(-FLT_MIN, 30.f)))
					CFileLoad_Manager::GetInstance()->Load_MapSaveFile(LEVEL_EFFECTTOOL, TEXT("../SaveFiles/Map/SaveMap_TT.dat"));
				if (ImGui::Button("Rapunzel", ImVec2(-FLT_MIN, 30.f)))
					CFileLoad_Manager::GetInstance()->Load_MapSaveFile(LEVEL_EFFECTTOOL, TEXT("../SaveFiles/Map/SaveMap_RA.dat"));
				if (ImGui::Button("Rapunzel Boss", ImVec2(-FLT_MIN, 30.f)))
				{
					CFileLoad_Manager::GetInstance()->Load_MapSaveFile(LEVEL_EFFECTTOOL, TEXT("../SaveFiles/Map/SaveMap_RA_Boss.dat"));

					// 1. 터레인 생성
					CGameObject* pTerrain = m_pGameInstance->Add_GameObject_And_Return(LEVEL_EFFECTTOOL, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"));
					if (pTerrain == nullptr)
						return E_FAIL;

					// 2. 정점 정보를 불러온다
					static_cast<CTerrain*>(pTerrain)->Load_TerrainData(TEXT("../SaveFiles/Terrain/RABoss.dat"));
				}
				if (ImGui::Button("Frozen", ImVec2(-FLT_MIN, 30.f)))
					CFileLoad_Manager::GetInstance()->Load_MapSaveFile(LEVEL_EFFECTTOOL, TEXT("../SaveFiles/Map/SaveMap_FZ.dat"));
				if (ImGui::Button("KeyGrave", ImVec2(-FLT_MIN, 30.f)))
					CFileLoad_Manager::GetInstance()->Load_MapSaveFile(LEVEL_EFFECTTOOL, TEXT("../SaveFiles/Map/SaveMap_KG.dat"));
				if (ImGui::Button("FinalBoss", ImVec2(-FLT_MIN, 30.f)))
					CFileLoad_Manager::GetInstance()->Load_MapSaveFile(LEVEL_EFFECTTOOL, TEXT("../SaveFiles/Map/SaveMap_BT.dat"));

				// 맵 이동 -> 렉걸리니까 이동 끝나면 꺼주세요
				static _bool bMoveMap = false;
				ImGui::Checkbox("Move Map", &bMoveMap);
				if (bMoveMap == true)
				{
					list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_EFFECTTOOL, TEXT("Layer_Map"));

					// 맵 레이어가 없거나 맵 리스트에 아무것도 없으면 패스
					if (MapList != nullptr && MapList->size() != 0)
					{
						for (auto& pMap : *MapList)
						{
							CTransform* pMapTransform = pMap->Get_Transform();

							if (pMapTransform == nullptr)
								return S_OK;

							if (m_pGameInstance->Get_KeyPressing(DIK_RIGHT))
								pMapTransform->Go_Vector(__vector(1.f, 0.f, 0.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_LEFT))
								pMapTransform->Go_Vector(__vector(-1.f, 0.f, 0.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_UP))
								pMapTransform->Go_Vector(__vector(0.f, 0.f, 1.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_DOWN))
								pMapTransform->Go_Vector(__vector(0.f, 0.f, -1.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_O))
								pMapTransform->Go_Vector(__vector(0.f, -1.f, 0.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_P))
								pMapTransform->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), 0.1);
						}
					}

					list<CGameObject*>* TerrainList = m_pGameInstance->Get_ObjectList(LEVEL_EFFECTTOOL, TEXT("Layer_Terrain"));
					if (TerrainList != nullptr && TerrainList->size() != 0)
					{
						for (auto& pTerrain : *TerrainList)
						{
							CTransform* pTerrainTransform = pTerrain->Get_Transform();

							if (pTerrainTransform == nullptr)
								return S_OK;

							if (m_pGameInstance->Get_KeyPressing(DIK_RIGHT))
								pTerrainTransform->Go_Vector(__vector(1.f, 0.f, 0.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_LEFT))
								pTerrainTransform->Go_Vector(__vector(-1.f, 0.f, 0.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_UP))
								pTerrainTransform->Go_Vector(__vector(0.f, 0.f, 1.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_DOWN))
								pTerrainTransform->Go_Vector(__vector(0.f, 0.f, -1.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_O))
								pTerrainTransform->Go_Vector(__vector(0.f, -1.f, 0.f, 0.f), 0.1);
							if (m_pGameInstance->Get_KeyPressing(DIK_P))
								pTerrainTransform->Go_Vector(__vector(0.f, 1.f, 0.f, 0.f), 0.1);
						}
					}
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));

	CameraDesc.vEye = _float3(0.f, 10.f, -7.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_EFFECTTOOL, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_EFFECTTOOL, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Axis(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_EFFECTTOOL, pLayerTag, TEXT("Prototype_GameObject_Axis"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_EFFECTTOOL, pLayerTag, TEXT("Prototype_GameObject_TestMonster"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Lights()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_EffectTool::Select_EffectMesh()
{
	for (size_t i = 0; i < GM->Get_EffectMeshTag_Size(); i++)
	{
		char	szModelTag[60];
		WideCharToMultiByte(CP_UTF8, 0, (*GM->Get_EffectMeshTag_Vector())[i], 60, szModelTag, IM_ARRAYSIZE(szModelTag), NULL, NULL);
		// 선택한 모델을 픽함
		if (ImGui::Selectable(szModelTag, m_iEffectMeshIndex == i))
		{
			m_iEffectMeshIndex = (_uint)i;

			lstrcpy(m_tEffectMeshDesc.ModelTag, (*GM->Get_EffectMeshTag_Vector())[i]);
		}
	}
}

CLevel_EffectTool * CLevel_EffectTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_EffectTool*	pInstance = new CLevel_EffectTool(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_EffectTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_EffectTool::Free()
{
	__super::Free();
}
