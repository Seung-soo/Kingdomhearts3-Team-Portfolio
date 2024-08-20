#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "GameInstance.h"

BEGIN(Client)

class CTerrainTool final : public CBase
{
	MAKE_SINGLETON(CTerrainTool)

public:
	explicit CTerrainTool() = default;
	virtual ~CTerrainTool() = default;

#ifdef _DEBUG

public:
	HRESULT CTerrainTool_SetUp();
	_int	CTerrainTool_Tick(_double TimeDelta);
	HRESULT CTerrainTool_Render();

private:
	void	Generate_Terrain();
	void	Eleminate_Terrain();
	void	LControl_Function();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;
	class CImGui_Manager*	m_pImGui_Manager = nullptr;

public:
	_float4					m_vPickedPoint;
	_bool					m_isPosBrush = false;

	class CTerrain*			m_pTerrain = nullptr;

	CVIBuffer_Terrain::POS_BRUSH_DESC m_PosBrush;
	CVIBuffer_Terrain::TEX_BRUSH_DESC m_TexBrush;

	const _tchar* m_pComponentTag_Prototype = nullptr;
	const _tchar* m_pGameObjectTag_Prototype = nullptr;
	const char* m_pLayerTag = nullptr;

#endif // _DEBUG

public:
	void virtual Free() override;
};

END