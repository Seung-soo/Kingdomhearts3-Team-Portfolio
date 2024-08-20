#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct tagTerrainBufferStruct
	{
		_uint numX,		numZ;
		_uint tileX,	tileZ;

	} TERRAIN_BUFFER_DESC;

	/*enum BRUSH_TYPE		{	TYPE_POSITION		= 1, TYPE_SPLATTING,	TYPE_END									};*/
	enum BRUSH_WORK		{	WORK_UP				= 1, WORK_DOWN,			WORK_FLAT,		WORK_INIT,		WORK_END	};
	enum BRUSH_STYLE	{	STYLE_HIGHER		= 1, STYLE_LOWER,		STYLE_MIDDLE,	STYLE_END					};
	enum BRUSH_TEX		{	TEX_FIRST			= 1, TEX_SECOND,		TEX_THIRD,		TEX_FORTH,		TEX_END		};

	typedef struct tagPositionBrushStruct
	{
		BRUSH_WORK		brush_work;
		BRUSH_STYLE		brush_style;

		_float3			brush_position;
		_float			brush_range;
		_float			brush_value;

	} POS_BRUSH_DESC;

	typedef struct tagSplattingBrushStruct
	{
		BRUSH_TEX		brush_tex;

		_float3			brush_position;
		_float			brush_range;
		_float			brush_value;

	} TEX_BRUSH_DESC;

public:
	explicit CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	HRESULT			NativeConstruct_Prototype(_uint iNumX, _uint iNumZ);
	HRESULT			NativeConstruct_Prototype(const _tchar* pFilePath);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	_uint			NumX() { return m_iNumVerticesX; }
	_uint			NumZ() { return m_iNumVerticesZ; }

public:
	_float			Compute_Y(_fvector vPosition);
	_float			Compute_Y(const _float3& vPoint);

	void			Edit_Position(POS_BRUSH_DESC _posBrush, _float3* out = nullptr);
	void			Edit_Texture(TEX_BRUSH_DESC _texBrush, _float3* out = nullptr);
	void			Calculate_Normal(VTXNORMULTEX* pVertices);

	HRESULT			Load_Terrain(const _tchar* pFilePath);

public:
	_bool			Terrain_Pick(const _matrix& WorldMatrixInverse, _fvector * pOut);

private:
	_float			m_fTileNumX = 30.f;
	_float			m_fTileNumZ = 30.f;
	_float4*		m_pVerticesSplatting = nullptr;

public:
	HRESULT			Save_Y(const char* szTerrainSaveFileName);
	void			Save_Terrain();
	void			Load_Terrain();

public:
	static CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumX = 129, _uint iNumZ = 129);
	static CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TERRAIN_BUFFER_DESC _Desc);
	static CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFilePath);

	virtual CComponent*			Clone(void* pArg);
	virtual void				Free() override;
};

END