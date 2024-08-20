#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	MAKE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATETYPE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_OLDVIEW, D3DTS_OLDPROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;
public:
	HRESULT			Set_Transform(TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix);

	_matrix			Get_TransformMatrix(TRANSFORMSTATETYPE eStateType);

	_float4x4		Get_TransformFloat4x4(TRANSFORMSTATETYPE eStateType);

	_float4x4		Get_TransformFloat4x4_TP(TRANSFORMSTATETYPE eStateType);

	_vector			Get_CamPosition() const;

public:
	void			Update();
private:
	_float4x4			m_PipeLineMatrix[D3DTS_END];

	_float4				m_vCamPosition = _float4(0.f, 0.f, 0.f, 1.f);

public:
	virtual void	Free() override;
};

END