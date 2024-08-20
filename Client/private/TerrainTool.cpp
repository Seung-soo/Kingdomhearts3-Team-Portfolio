#include "stdafx.h"
#include "TerrainTool.h"
#include "Terrain.h"

#include "ImGui_Manager.h"
#include "VIBuffer_Terrain.h"

READY_SINGLETON(CTerrainTool)

#ifdef _DEBUG



HRESULT CTerrainTool::CTerrainTool_SetUp()
{
	// 멤버변수 초기화
	m_pGameInstance = CGameInstance::GetInstance();
	m_pDevice = m_pGameInstance->Get_Device();
	m_pDeviceContext = m_pGameInstance->Get_Device_Context();
	m_pImGui_Manager = CImGui_Manager::GetInstance();
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pImGui_Manager);

	if (nullptr == m_pGameInstance || nullptr == m_pDevice ||
		nullptr == m_pDeviceContext || nullptr == m_pImGui_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

_int CTerrainTool::CTerrainTool_Tick(_double TimeDelta)
{
	if (true == CImGui_Manager::GetInstance()->show_terraintool_window)
	{
		Generate_Terrain();

		if (nullptr == m_pTerrain)
			return RESULT_NOPROBLEM;

		ImGui::SetNextWindowBgAlpha(0.7f);
		ImGui::Begin("TerrainTool Window");

		static _bool edit_mode = false;
		ImGui::Checkbox("Edit Mode", &edit_mode);

		ImGui::SameLine();
		if (true == ImGui::Button("Eliminate"))
		{
			Eleminate_Terrain();
			ImGui::End();
			return RESULT_NOPROBLEM;
		}

		if (false == edit_mode)
		{
			ImGui::End();
			return RESULT_NOPROBLEM;
		}

		// 월드 상태
		_vector ray_p = CPicking::GetInstance()->Get_RayPos();
		_vector ray_d = CPicking::GetInstance()->Get_RayDir();
		ray_d = XMVector3Normalize(ray_d);

		_bool ray_hit_terrain = false;
		_uint ray_length = 200;
		for (_uint i = 0; i < ray_length; ++i)
		{
			// 범위 안에 있는지 검사
			_vector result_pos = m_pTerrain->Compute_PositionOnTerrain(ray_p);
			if (XMVector4Equal(result_pos, XMVectorZero()))
			{
				ray_p += ray_d;
				continue;
			}

			// 충돌했는지 검사
			_float height = vecy(result_pos);
			if (vecy(ray_p) > height)
			{
				ray_p += ray_d;
				continue;
			}

			// 조건을 충족하고 내려왔다 : 충돌했다
			vecy(ray_p) = height;
			XMStoreFloat3(&m_PosBrush.brush_position, ray_p);
			XMStoreFloat3(&m_TexBrush.brush_position, ray_p);

			XMStoreFloat4(&m_vPickedPoint, ray_p);
			ray_hit_terrain = true;

			break;
		}

		ImGui::Separator();
		ImGui::Separator();

		if (true == ray_hit_terrain)
			ImGui::Text("Ray Hits");
		else
			ImGui::Text("Ray Doesn't Hit");

		ImGui::Separator();
		ImGui::Separator();

		if (ImGui::BeginTabBar("Terrain Tool"))
		{
			if (ImGui::BeginTabItem("Height"))
			{
				m_isPosBrush = true;

				if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && 
					m_pGameInstance->Get_KeyPressing(DIK_LSHIFT))
				{
					if (m_pGameInstance->Get_KeyEnter(DIK_1))
						m_PosBrush.brush_style = CVIBuffer_Terrain::BRUSH_STYLE::STYLE_HIGHER;
					else if (m_pGameInstance->Get_KeyEnter(DIK_2))
						m_PosBrush.brush_style = CVIBuffer_Terrain::BRUSH_STYLE::STYLE_LOWER;
					else if (m_pGameInstance->Get_KeyEnter(DIK_3))
						m_PosBrush.brush_style = CVIBuffer_Terrain::BRUSH_STYLE::STYLE_MIDDLE;
					else if (m_pGameInstance->Get_KeyEnter(DIK_Z))
						m_PosBrush.brush_style = (CVIBuffer_Terrain::BRUSH_STYLE)0;
				}

				else if (m_pGameInstance->Get_KeyPressing(DIK_LSHIFT))
				{
					if (m_pGameInstance->Get_KeyEnter(DIK_1))
						m_PosBrush.brush_work = CVIBuffer_Terrain::BRUSH_WORK::WORK_UP;
					else if (m_pGameInstance->Get_KeyEnter(DIK_2))
						m_PosBrush.brush_work = CVIBuffer_Terrain::BRUSH_WORK::WORK_DOWN;
					else if (m_pGameInstance->Get_KeyEnter(DIK_3))
						m_PosBrush.brush_work = CVIBuffer_Terrain::BRUSH_WORK::WORK_FLAT;
					else if (m_pGameInstance->Get_KeyEnter(DIK_4))
						m_PosBrush.brush_work = CVIBuffer_Terrain::BRUSH_WORK::WORK_INIT;
					else if (m_pGameInstance->Get_KeyEnter(DIK_Z))
						m_PosBrush.brush_work = (CVIBuffer_Terrain::BRUSH_WORK)0;
				}

				ImGui::LabelText("Brush Position : ", "%.2f, %.2f, %.2f", m_PosBrush.brush_position.x, m_PosBrush.brush_position.y, m_PosBrush.brush_position.z);
				ImGui::Text("Type [ 0 = EXPECTION, 1 = UP, 2 = DOWN, 3 = LEVEL, 4 = ZERO ]");
				ImGui::SliderInt("Brush Type", (_int*)(&m_PosBrush.brush_work), 0, (_int)(CVIBuffer_Terrain::WORK_END - 1));
				ImGui::Text("Style [ 0 = EXPECTION, 1 = Higher, 2 = Lower, 3 = Level");
				ImGui::SliderInt("Brush Style", (_int*)(&m_PosBrush.brush_style), 0, (_int)(CVIBuffer_Terrain::STYLE_END - 1));

				ImGui::Separator();
				ImGui::DragFloat("Brush Range [ 0 - 50 ]", &m_PosBrush.brush_range, 0.05f, 0.f, 50.f);
				ImGui::DragFloat("Raise Amount [ 0 - 5 ]", &m_PosBrush.brush_value, 0.05f, 0.f, 5.f);

				_float3 out;
				ZeroMemory(&out, sizeof(_float3));

				if (m_pGameInstance->Get_KeyPressing(DIK_SPACE))
				{
					if (CVIBuffer_Terrain::WORK_INIT == m_PosBrush.brush_work)
					{
						static_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Component(TEXT("Com_VIBuffer")))->Edit_Position(m_PosBrush, &out);
						m_vPickedPoint.x = out.x;
						m_vPickedPoint.y = out.y;
						m_vPickedPoint.z = out.z;
						m_vPickedPoint.w = 1.f;
					}
					else if (0.f != m_PosBrush.brush_range &&
						0 != m_PosBrush.brush_work	&&
						CVIBuffer_Terrain::WORK_END != m_PosBrush.brush_work)
					{
						static_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Component(TEXT("Com_VIBuffer")))->Edit_Position(m_PosBrush, &out);
						m_vPickedPoint.x = out.x;
						m_vPickedPoint.y = out.y;
						m_vPickedPoint.z = out.z;
						m_vPickedPoint.w = 1.f;
					}
				}

				ImGui::LabelText("Picked Position", "%.2f, %.2f, %.2f", out.x, out.y, out.z);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Texture"))
			{
				m_isPosBrush = false;

				ImGui::SliderInt("Brush Type", (_int*)(&m_TexBrush.brush_tex), 0, (_int)(CVIBuffer_Terrain::BRUSH_TEX::TEX_END - 1));
				ImGui::LabelText("Brush Position : ", "%.2f, %.2f, %.2f", m_TexBrush.brush_position.x, m_TexBrush.brush_position.y, m_TexBrush.brush_position.z);
				ImGui::DragFloat("Brush Range", &m_TexBrush.brush_range, 0.05f, 0.f, 50.f);
				ImGui::DragFloat("Raise Amount", &m_TexBrush.brush_value, 0.05f, 0.f, 5.f);

				_float3 out;
				ZeroMemory(&out, sizeof(_float3));

				if (m_pGameInstance->Get_KeyPressing(DIK_SPACE))
				{
					if (0.f != m_TexBrush.brush_range &&
						0 != m_TexBrush.brush_tex	&&
						CVIBuffer_Terrain::BRUSH_TEX::TEX_END != m_TexBrush.brush_tex)
					{
						static_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Component(TEXT("Com_VIBuffer")))->Edit_Texture(m_TexBrush, &out);
						m_vPickedPoint.x = out.x;
						m_vPickedPoint.y = out.y;
						m_vPickedPoint.z = out.z;
						m_vPickedPoint.w = 1.f;
					}
				}

				ImGui::LabelText("Picked Position", "%.2f, %.2f, %.2f", out.x, out.y, out.z);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		LControl_Function();

		ImGui::End();
	}

	
	return RESULT_NOPROBLEM;
}

HRESULT CTerrainTool::CTerrainTool_Render()
{
	return S_OK;
}

void CTerrainTool::Generate_Terrain()
{
	if (nullptr != m_pTerrain)
		return;

	static _int number_vertex_x = 65;
	static _int number_vertex_z = 65;

	static _int number_tile_x = 16;
	static _int number_tile_z = 16;

	ImGui::Begin("Terrain Generator");

	ImGui::PushItemWidth(200.f);

	ImGui::InputInt("Number of X-axis Vertices", &number_vertex_x);
	ImGui::InputInt("Number of Z-axis Vertices", &number_vertex_z);

	ImGui::InputInt("Number of X-axis Tiles", &number_tile_x);
	ImGui::InputInt("Number of Z-axis Tiles", &number_tile_z);

	static char sz_prototype_component_tag[64] = "Prototype_Component_VIBuffer_Terrain";
	ImGui::InputText("Prototype Component Tag", sz_prototype_component_tag, 64);

	static char sz_prototype_gameobject_tag[64] = "Prototype_Gameobject_Terrain";
	ImGui::InputText("Prototype Gameobject Tag", sz_prototype_gameobject_tag, 64);

	static char sz_layer_tag[64] = "Layer_Background";
	ImGui::InputText("Layer Tag", sz_layer_tag, 64);

	ImVec2 button_size;
	button_size.x = 368.5f;
	button_size.y = 0.f;

	if (ImGui::Button("Generate", button_size))
	{
		_int proto_component_tag_length = (_int)strlen(sz_prototype_component_tag);
		_int proto_gameobject_tag_legnth = (_int)strlen(sz_prototype_gameobject_tag);
		_int layer_tag_length = (_int)strlen(sz_layer_tag);

		if (0 == proto_component_tag_length ||
			0 == proto_gameobject_tag_legnth ||
			0 == layer_tag_length)
		{
			ImGui::End();
			return;
		}

		if (0 == number_vertex_x ||
			0 == number_vertex_z)
		{
			ImGui::End();
			return;
		}

		CVIBuffer_Terrain::TERRAIN_BUFFER_DESC terrain_desc;
		ZeroMemory(&terrain_desc, sizeof(CVIBuffer_Terrain::TERRAIN_BUFFER_DESC));

		terrain_desc.numX = (_uint)number_vertex_x;
		terrain_desc.numZ = (_uint)number_vertex_z;

		terrain_desc.tileX = (_uint)number_tile_x;
		terrain_desc.tileZ = (_uint)number_tile_z;

		// 터레인 버퍼 생성
		CVIBuffer_Terrain* terrain_buffer = CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, terrain_desc);

		if (nullptr != terrain_buffer)
		{
			// 프로토타입 컴포넌트
			static _tchar prototype_component_tag_w[64] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, sz_prototype_component_tag, (_int)strlen(sz_prototype_component_tag), prototype_component_tag_w, 64);

			LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();
			_bool success_component = true;

			if (FAILED(m_pGameInstance->Add_Prototype(current_level, prototype_component_tag_w, terrain_buffer)))
			{
				MSGBOX("Failed to Generate Terrain Buffer in Map Edit Scene");
				Safe_Release(terrain_buffer);

				success_component = false;
			}

			m_pComponentTag_Prototype = prototype_component_tag_w;

			if (!success_component)
			{
				ImGui::End();
				return;
			}

			// 프로토타입 게임오브젝트
			static _tchar prototype_gameobject_tag_w[64] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, sz_prototype_gameobject_tag, (_int)strlen(sz_prototype_gameobject_tag), prototype_gameobject_tag_w, 64);

			CTerrain* terrain = CTerrain::Create(m_pDevice, m_pDeviceContext);
			_bool success_gameobject = true;

			if (nullptr != terrain)
			{
				if (m_pGameInstance->Add_Prototype(prototype_gameobject_tag_w, terrain))
				{
					MSGBOX("Failed to Create CTerrain in Map Edit Scene");
					Safe_Release(terrain);

					success_gameobject = false;
				}
			}

			m_pGameObjectTag_Prototype = prototype_gameobject_tag_w;

			if (!success_gameobject)
			{
				ImGui::End();
				return;
			}

			// 게임오브젝트 클론
			static _tchar layer_tag_w[64] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, sz_layer_tag, (_int)strlen(sz_layer_tag), layer_tag_w, 64);

			m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Add_GameObject_And_Return(current_level, layer_tag_w, prototype_gameobject_tag_w));
			if (nullptr == m_pTerrain)
			{
				MSGBOX("Failed to Add Object");
				BREAKPOINT;
			}

			m_pLayerTag = sz_layer_tag;
		}
	}

	/*button_size.x = 180.f;

	if (ImGui::Button("Save", button_size))
	{
		int iDebug = 0;
	}

	ImGui::SameLine();
	if (ImGui::Button("Load", button_size))
	{
		int iDebug = 0;
	}*/

	ImGui::PopItemWidth();
	ImGui::End();
}

void CTerrainTool::Eleminate_Terrain()
{
	if (nullptr == m_pTerrain || nullptr == m_pComponentTag_Prototype)
		return;

	// 현재 레벨
	LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();
	
	_tchar layer_tag_w[64] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, m_pLayerTag, (_int)strlen(m_pLayerTag), layer_tag_w, 64);

	m_pGameInstance->Rmv_GameObject(current_level, layer_tag_w, m_pTerrain);
	m_pTerrain = nullptr;

	// 컴포넌트 프로토타입 삭제
	m_pGameInstance->Rmv_Prototype(current_level, m_pComponentTag_Prototype);
	m_pComponentTag_Prototype = nullptr;

	// 게임오브젝트 프로토타입 삭제
	m_pGameInstance->Rmv_Prototype(m_pGameObjectTag_Prototype);
	m_pGameObjectTag_Prototype = nullptr;

	// 변수 초기화
	m_isPosBrush = false;
	ZeroMemory(&m_vPickedPoint, sizeof(_float4));
	ZeroMemory(&m_PosBrush, sizeof(CVIBuffer_Terrain::POS_BRUSH_DESC));
	ZeroMemory(&m_TexBrush, sizeof(CVIBuffer_Terrain::TEX_BRUSH_DESC));
}

void CTerrainTool::LControl_Function()
{
	if (!m_pGameInstance->Get_KeyPressing(DIK_LCONTROL))
		return;

	if (m_pGameInstance->Get_KeyEnter(DIK_S))
		static_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Component(TEXT("Com_VIBuffer")))->Save_Terrain();

	if (m_pGameInstance->Get_KeyEnter(DIK_L))
		static_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Component(TEXT("Com_VIBuffer")))->Load_Terrain();

	if (m_pGameInstance->Get_KeyEnter(DIK_N))
		static_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Component(TEXT("Com_VIBuffer")))->Calculate_Normal(nullptr);
}

#endif // _DEBUG

void CTerrainTool::Free()
{
#ifdef _DEBUG

	// 멤버변수 Free
	if (nullptr != m_pDevice)
		Safe_Release(m_pDevice);
	if (nullptr != m_pDeviceContext)
		Safe_Release(m_pDeviceContext);
	if (nullptr != m_pGameInstance)
		Safe_Release(m_pGameInstance);
	if (nullptr != m_pImGui_Manager)
		Safe_Release(m_pImGui_Manager);

#endif // _DEBUG
}

