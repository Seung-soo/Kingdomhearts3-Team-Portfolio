#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CTransform;

class ENGINE_DLL CVIBuffer_Trail final :public CVIBuffer
{
private:
	struct TrailData
	{
		_float3 position[2];
		_bool	isEnd = false;
		double timecount = 0.0;
		TrailData(const _float3& upposition, const _float3& downposition)
			: timecount(0.0)
		{
			position[0] = upposition;
			position[1] = downposition;
		}
	};

public:
	typedef struct tagTrailDesc {
		_uint		iNumVerticies = 300;
		_float		fDuration = 0.01f;    // 몇초마다 트레일 정점을 넣을건지
		_float		fLifeTime = 0.5f;
		_float		fLerpCnt = 10.f;	 // 보간할 정점에 개수라고 생각하면 편함 (위아래가 한세트니까 총 5개라고 생각하면 뎀)
	}TRAILDESC;

private:
	explicit CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	void Update(_float fTimeDelta, _matrix* pWorldMat);
	void Update_EndMotion(_float fTimeDelta, _matrix* pWorldMat); 

public:
	virtual HRESULT Render(CShader* pShader, _uint iPassIndex) override;

	void SplineTrailPosition(OUT VTXTEX* vtx, const size_t& dataindex, unsigned long& index, _matrix* pWorldMat);
	
	void AddNewTrail(const _float3& upposition, const _float3& downposition, _float fTimeDelta);
	void UpdateTrail(const _float3& upposition, const _float3& downposition, _float fTimeDelta);
	void Reset_Trail();

	_uint Get_TrailDataSize() { return (_uint)m_tTrailData.size(); }
	_bool Is_TrailFinished() { return m_isFinished; }

public:
	static CVIBuffer_Trail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*			Clone(void* pArg);
	virtual void				Free() override;

private:
	std::deque<TrailData>		m_tTrailData;

	_uint						m_iLerpCnt = 10;
	_float						m_fAliveTime = 0.5f;
	_float						m_fDuration = 0.01f;
	_float						m_fTimeAccum = 0.f;

	_bool						m_isFinished = false;

	_float						m_fEndingTimeAccum = 0.f;
	_float						m_fEndingTime = 0.5f;
	_float						m_fEndingAliveTime = 0.1f;
};

END