#pragma once

/*	파티클 생성 방법
	1.	먼저 사용할 것 같은 파티클 갯수를 미리 원형 추가 해둔다.
	(여기서 이게 불편하면 수정가능. 
	하지만 버텍스버퍼와 인덱스 버퍼를 사본생성할 때마다 만들어줘야함. 
	프레임을 조금이라도 지키고 싶다면 이 방식 유지.)

	2.	클라에 있는 파티클을 사용할 클래스에서 모든 파티클 입자의 행렬이 담긴 벡터 컨테이너를 업데이트파티클 함수로 던져줌.

	3.	셰이더에 이걸 던지면서 랜더하면 끝.	
*/

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer
{
public:
	typedef struct tagParticleDesc 
	{
		_float		fMinSpeed, fMaxSpeed;
		_float3		vMoveDir;
	}PARTICLEDESC;
public:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(_uint iNumInstance);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(class CShader* pShader, _uint iPassIndex);
public:
	void			UpdateParticle(vector<_float4x4> TransformMatrix, _double TimeDelta);
private:
	ID3D11Buffer*			m_pVBInstance = nullptr;
	D3D11_BUFFER_DESC		m_VBInstDesc;
	D3D11_SUBRESOURCE_DATA	m_VBInstSubResource;
	_uint					m_iNumInstance = 0;

	PARTICLEDESC			m_ParticleDesc;
public:
	static CVIBuffer_Point_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumInstance);
	virtual CComponent*					Clone(void* pArg);
	virtual void						Free() override;
};

END