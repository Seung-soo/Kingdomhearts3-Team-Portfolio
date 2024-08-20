#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "Mouse.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_vector CTransform::Get_Scale(STATE eState) const
{
	return XMVector3Length(XMLoadFloat3((_float3*)&m_WorldMatrix.m[eState][0]));
}

_vector CTransform::Get_AllScale() const
{
	return XMVectorSet(XMVectorGetX(Get_Scale(STATE_RIGHT)), XMVectorGetX(Get_Scale(STATE_UP)), XMVectorGetX(Get_Scale(STATE_LOOK)), 0.f);
}

_vector CTransform::Get_State(STATE eState) const
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
}

_vector CTransform::Get_NRight() const
{
	return XMVector3Normalize(XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_RIGHT][0]));
}

_vector CTransform::Get_NUp() const
{
	return XMVector3Normalize(XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_UP][0]));
}

_vector CTransform::Get_NLook() const
{
	return XMVector3Normalize(XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_LOOK][0]));
}

_vector CTransform::Get_Right() const
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_RIGHT][0]);
}

_vector CTransform::Get_Up() const
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_UP][0]);
}

_vector CTransform::Get_Look() const
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_LOOK][0]);
}

_vector CTransform::Get_Position() const
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_POSITION][0]);
}

_float4 * CTransform::Get_PositionFloat4_Pointer()
{
	memcpy(&m_vPosition, &m_WorldMatrix.m[3], sizeof(_float4));

	return &m_vPosition;
}

_float4 CTransform::Get_Position_F4()
{
	_vector vPos = Get_Position();
	_float4 vPosF4;
	XMStoreFloat4(&vPosF4, vPos);

	return vPosF4;
}

_matrix CTransform::Get_WorldMatrix() const
{
	return XMLoadFloat4x4(&m_WorldMatrix);
}

_matrix CTransform::Get_WorldMatrixInverse() const
{
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
}

_float4x4 CTransform::Get_WorldFloat4x4() const
{
	return m_WorldMatrix;
}

_float4x4* CTransform::Get_WorldFloat4x4_Pointer()
{
	return &m_WorldMatrix;
}

_matrix CTransform::Get_OldWorldMatrix() const
{
	return XMLoadFloat4x4(&m_OldWorldMatrix);
}

_matrix CTransform::Get_OldWorldMatrixInverse() const
{
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_OldWorldMatrix));
}

_float4x4 CTransform::Get_OldWorldFloat4x4() const
{
	return m_OldWorldMatrix;
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vState);
}

void CTransform::Set_Right(_fvector vState)
{
	XMStoreFloat4((_float4*)&m_WorldMatrix.m[STATE_RIGHT][0], vState);
}

void CTransform::Set_Up(_fvector vState)
{
	XMStoreFloat4((_float4*)&m_WorldMatrix.m[STATE_UP][0], vState);
}

void CTransform::Set_Look(_fvector vState)
{
	XMStoreFloat4((_float4*)&m_WorldMatrix.m[STATE_LOOK][0], vState);
}

void CTransform::Set_Position(_fvector vPos)
{
	XMStoreFloat4((_float4*)&m_WorldMatrix.m[STATE_POSITION][0], vPos);
}

_bool CTransform::Set_PositionCameraNavi(_fvector vPos, CNavigation* pNavigation)
{
	_vector vCurrentPosition = Get_Position();
	if (nullptr == pNavigation)
	{
		XMStoreFloat4((_float4*)&m_WorldMatrix.m[STATE_POSITION][0], vPos);
		return true;
	}

	return pNavigation->Move_OnNavigation_NotRunning(vPos, vCurrentPosition);
}

void CTransform::Set_PositionX(_float vPosX)
{
	m_WorldMatrix._41 = vPosX;
}

void CTransform::Set_PositionY(_float vPosY)
{
	m_WorldMatrix._42 = vPosY;
}

void CTransform::Set_PositionZ(_float vPosZ)
{
	m_WorldMatrix._43 = vPosZ;
}

void CTransform::Set_LookY_Zero(_double TimeDelta)
{
	_vector vLook = XMVector3Normalize(Get_Look());
	
	if (fabsf(vecy(vLook)) - 0.f < 0.1f)
		return;

	if (vecy(vLook) > 0.f)
		Turn_Down(TimeDelta);
	else
		Turn_Up(TimeDelta);
}

void CTransform::Set_LookY_Zero_Once(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vRight, vUp, vLook;

	vLook = XMVectorSetY(Get_Look(), 0.f);
	vLook = XMVector4Normalize(vLook);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));
}

void CTransform::Set_MoveBasedOnCurrentRight(_float fCoefficient)
{
	Set_Position(Get_Position() + Get_Right() * fCoefficient);
}

void CTransform::Set_MoveBasedOnCurrentUp(_float fCoefficient)
{
	Set_Position(Get_Position() + Get_Up() * fCoefficient);
}

void CTransform::Set_MoveBasedOnCurrentLook(_float fCoefficient)
{
	Set_Position(Get_Position() + Get_Look() * fCoefficient);
}

HRESULT CTransform::NativeConstruct_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OldWorldMatrix, XMMatrixIdentity());

	ZeroMemory(&m_JumpDesc, sizeof(JUMP_DESC));
	ZeroMemory(&m_KnockBackDesc, sizeof(KNOCKBACK_DESC));
	ZeroMemory(&m_LocationDesc, sizeof(LOCATION_DESC));

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

HRESULT CTransform::Bind_OnShader(CShader * pShader, const char * pValueName)
{
	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));

	return pShader->Set_RawValue(pValueName, &WorldMatrix, sizeof(_float4x4));
}

HRESULT CTransform::Bind_OnOldShader(CShader * pShader, const char * pValueName)
{
	_float4x4		OldWorldMatrix;

	XMStoreFloat4x4(&OldWorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_OldWorldMatrix)));

	return pShader->Set_RawValue(pValueName, &OldWorldMatrix, sizeof(_float4x4));
}

HRESULT CTransform::Sliding_Vector(_fvector vPosition, _fvector vCurrentPosition, CNavigation* pNavigation, _double TimeDelta)
{
	_float3*	pSlidingVector = nullptr;

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (nullptr != pSlidingVector)
		// 네비에 막혀서 이동을 못하고 슬라이딩 벡터가 nullptr이 아닐 때 슬라이딩 벡터를 탄다.
	{
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = XMVector4Normalize(-(vPosition - vCurrentPosition));
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));

		// 슬라이딩 벡터로 이동했을 때 이동할 수 있는지 한번 더 검사
		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}
	return S_OK;
}

HRESULT CTransform::Go_Straight(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}

	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta));

	return S_OK;
}

HRESULT CTransform::Go_Right(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;
	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}

	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta))

	return S_OK;
}

HRESULT CTransform::Go_Left(_double TimeDelta, CNavigation* pNavigation)
{

	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}

	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta))

	return S_OK;
}

HRESULT CTransform::Go_Backward(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta))

	return S_OK;
}

HRESULT CTransform::Go_FrontLeft(_double TimeDelta, CNavigation * pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vDirection = XMVector4Normalize(vLook - vRight);

	vPosition += vDirection * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta))

	return S_OK;
}

HRESULT CTransform::Go_FrontRight(_double TimeDelta, CNavigation * pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vDirection = XMVector4Normalize(vLook + vRight);

	vPosition += vDirection * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta))

	return S_OK;
}

HRESULT CTransform::Go_BackLeft(_double TimeDelta, CNavigation * pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vDirection = XMVector4Normalize(-vLook - vRight);

	vPosition += vDirection * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta))

	return S_OK;
}

HRESULT CTransform::Go_BackRight(_double TimeDelta, CNavigation * pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vDirection = XMVector4Normalize(-vLook + vRight);

	vPosition += vDirection * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta))

	return S_OK;
}

HRESULT CTransform::Go_Up(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}

	return S_OK;
}

HRESULT CTransform::Go_Down(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;
	_vector		vUp = Get_State(CTransform::STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}

	return S_OK;
}

_bool CTransform::Go_Straight_Check_NaviEnd(_double TimeDelta, CNavigation* pNavigation)
{
	_vector vPosition        = Get_State(CTransform::STATE_POSITION);
	_vector vCurrentPosition = vPosition;
	_vector vLook            = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	// 네비에 막혀서 더 이상 갈 수 없으면 true 리턴
	if (pNavigation->Move_OnNavigation(vPosition, vCurrentPosition) == UNMOVABLE)
		return true;

	// 갈 수 있으면 포지션 변경하고 false 리턴
	Set_State(CTransform::STATE_POSITION, vPosition);
	return false;
}

HRESULT CTransform::Go_Vector(_fvector vDirection, _double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;

	vPosition += XMVector3Normalize(vDirection) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta))

	return S_OK;
}

HRESULT CTransform::Go_VectorXZ(_fvector vDirection, _double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPosition = vPosition;

	_vector vDir = XMVectorSetY(vDirection, 0.f);

	vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return S_OK;
	}
	CHECK_FAILED(Sliding_Vector(vPosition, vCurrentPosition, pNavigation, TimeDelta))

		return S_OK;
}

_bool CTransform::SetHeightZero(_double TimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition     = Get_State(CTransform::STATE_POSITION);
	_float	fTargetHeight = 0.f;

	if (pNavigation != nullptr)
		fTargetHeight = pNavigation->Compute_Y(vPosition);

	if (vecy(Get_Position()) > fTargetHeight)
		Go_Down(TimeDelta);

	if (vecy(Get_Position()) == fTargetHeight)
		return true;

	if (vecy(Get_Position()) <= fTargetHeight)
	{
		Set_Position(XMVectorSetY(Get_Position(), fTargetHeight));
		return true; 
	}

	return false;
}

_bool CTransform::SetHeight(_double TimeDelta, _float fTargetHeight, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	if (pNavigation != nullptr)
	{
		_float fNaviY = pNavigation->Compute_Y(vPosition);
		if (fTargetHeight <= fNaviY)
			fTargetHeight = fNaviY;
	}

	if (vecy(Get_Position()) > fTargetHeight)
		Go_Down(TimeDelta);

	if (vecy(Get_Position()) == fTargetHeight)
		return true;

	if (vecy(Get_Position()) <= fTargetHeight)
	{
		Set_Position(XMVectorSetY(Get_Position(), fTargetHeight));
		return true;
	}

	return false;
}

void CTransform::SetHeightNavi(_double TimeDelta, CNavigation* pNavigation)
{
	_vector vPos = Get_Position();
	_float fY    = pNavigation->Compute_Y(vPos);
	Set_Position(XMVectorSetY(vPos, fY));
}

_bool CTransform::PullObject(CTransform* pTargetTransform, _double TimeDelta, _double PullPower)
{
	_vector vMoveDir = -XMVector4Normalize(Get_Dir(pTargetTransform));
	_double	fSpeed   = TimeDelta * PullPower;
	
	if (Get_Distance(pTargetTransform) <= 0.2f)
		return true;

	pTargetTransform->Go_VectorXZ(vMoveDir, fSpeed);
	return false;
}

_bool CTransform::PushObject(CTransform* pTargetTransform, _double TimeDelta, _double MaxTime, _double PushPower, CNavigation* pNavigation)
{
	m_TimeAcc += TimeDelta;

	_vector vMoveDir = XMVector4Normalize(Get_Dir(pTargetTransform));

	if (Get_Distance(pTargetTransform) >= 5.0)
		PushPower *= 0.9;
	else if (Get_Distance(pTargetTransform) >= 10.0)
		PushPower *= 0.7;
	else if (Get_Distance(pTargetTransform) >= 15.0)
		PushPower *= 0.5;
	else if (Get_Distance(pTargetTransform) >= 20.0)
		PushPower *= 0.4;

	_double	fSpeed = TimeDelta * PushPower;

	if (Get_Distance(pTargetTransform) >= 25.f || m_TimeAcc >= MaxTime)
	{
		m_TimeAcc = 0.0;
		return true;
	}

	pTargetTransform->Go_VectorXZ(vMoveDir, fSpeed, pNavigation);
	return false;
}

_bool CTransform::IsInRange(CTransform* pTargetTransform, _float fRange)
{
	// 포지션의 높이를 pTargetTransform 의 높이로 설정
	_vector vTargetPos = pTargetTransform->Get_Position();
	_vector vPos       = XMVectorSetY(Get_Position(), vecy(vTargetPos));

	// 거리 계산
	_float fDistance   = pTargetTransform->Get_Distance(vPos);

	if (fDistance <= fRange)
		return true;

	return false;
}

HRESULT CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_float		fRightScale = vecx(Get_Scale(STATE_RIGHT));
	_float		fUpScale = vecx(Get_Scale(STATE_UP));
	_float		fLookScale = vecx(Get_Scale(STATE_LOOK));

	_vector		vRight = XMVector4Normalize(Get_State(CTransform::STATE_RIGHT));
	_vector		vUp = XMVector4Normalize(Get_State(CTransform::STATE_UP));
	_vector		vLook = XMVector4Normalize(Get_State(CTransform::STATE_LOOK));
	
	// vRight = XMVectorSetW(vRight, 0.f);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight * fRightScale);
	Set_State(CTransform::STATE_UP, vUp * fUpScale);
	Set_State(CTransform::STATE_LOOK, vLook * fLookScale);

	return S_OK;
}

HRESULT CTransform::Turn_Right(_fvector vAxis, _double TimeDelta)
{
	_vector vRight = Get_State(CTransform::STATE_RIGHT);
	_vector vUp = Get_State(CTransform::STATE_UP);
	_vector vLook = Get_State(CTransform::STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Turn_Left(_fvector vAxis, _double TimeDelta)
{
	_vector vRight = Get_State(CTransform::STATE_RIGHT);
	_vector vUp = Get_State(CTransform::STATE_UP);
	_vector vLook = Get_State(CTransform::STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, -m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Turn_Up(_double TimeDelta)
{
	_vector vRight = Get_State(CTransform::STATE_RIGHT);
	_vector vUp    = Get_State(CTransform::STATE_UP);
	_vector vLook  = Get_State(CTransform::STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(Get_Right(), -m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp    = XMVector4Transform(vUp, RotationMatrix);
	vLook  = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Turn_Down(_double TimeDelta)
{
	_vector vRight = Get_State(CTransform::STATE_RIGHT);
	_vector vUp    = Get_State(CTransform::STATE_UP);
	_vector vLook  = Get_State(CTransform::STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(Get_Right(), m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp    = XMVector4Transform(vUp, RotationMatrix);
	vLook  = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::CameraTurnY(_fvector vAxis, _double TimeDelta)
{
	_vector		vLook = XMVector3Normalize(Get_State(CTransform::STATE_LOOK));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vLook = XMVector3TransformNormal(XMVector4Normalize(vLook), RotationMatrix);
	_vector vRight = XMVector3Cross(__vector(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float fRadian = XMVectorGetX(XMVector3Dot(vUp, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (0.1f < fRadian)
	{
		Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
		Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
		Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
	}

	return S_OK;
}


HRESULT CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT)));
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP)));
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK)));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::NowRotation(_fvector vAxis, _float fRadian)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::AllRotation(_fvector vRotationNum)
{
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT)));
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP)));
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK)));

	_matrix		RotationMatrix = XMMatrixRotationX(XMVectorGetX(vRotationNum));
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	RotationMatrix = XMMatrixRotationY(XMVectorGetY(vRotationNum));
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	RotationMatrix = XMMatrixRotationZ(XMVectorGetZ(vRotationNum));
	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Revolution(_fvector vAxis, _fvector vTarget, _double TimeDelta)
{
	_vector		vRight     = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp        = Get_State(CTransform::STATE_UP);
	_vector		vLook      = Get_State(CTransform::STATE_LOOK);
	_vector		vPos       = Get_State(CTransform::STATE_POSITION);
	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vPos  -= vTarget;
	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp    = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook  = XMVector3TransformNormal(vLook, RotationMatrix);
	vPos   = XMVector3TransformNormal(vPos, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	vPos += vTarget;
	Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

_vector CTransform::Revolution_And_ReturnLook(_fvector vAxis, _fvector vTarget, _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPos = Get_State(CTransform::STATE_POSITION);
	_vector		vCurrentPos = vPos;

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	vPos -= vTarget;
	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);
	vPos = XMVector3TransformNormal(vPos, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	vPos += vTarget;
	Set_State(CTransform::STATE_POSITION, vPos);

	return XMVector3Normalize(vPos - vCurrentPos);
}


void CTransform::TeleportActivate(CNavigation* pNavigation)
{
	Set_Position(m_vTeleportPos);
	pNavigation->Find_Cell(m_vTeleportPos);
}

_bool CTransform::TeleportAroundTarget(CTransform* pTargetTransform, _fvector vDir, _float fDistance, CNavigation* pNavigation)
{
	_vector vTargetPos = pTargetTransform->Get_Position();

	m_vTeleportPos = vTargetPos + XMVector3Normalize(vDir) * fDistance;
	if (pNavigation->Move_OnNavigation(m_vTeleportPos, vTargetPos))
		return true;

	return false;
}

_bool CTransform::Teleport(_fvector vDir, _float fDistance, CNavigation* pNavigation)
{
	_vector vOriginPos = Get_Position();

	m_vTeleportPos = vOriginPos + vDir * fDistance;
	if (pNavigation->Move_OnNavigation(m_vTeleportPos, vOriginPos))
		return true;

	return false;
}

_bool CTransform::IsUnderTarget(CTransform* pTargetTransform, _float fDistance)
{
	_float fTargetY = vecy(pTargetTransform->Get_Position());
	_float fY       = vecy(Get_Position());

	fY += fDistance;

	if (fY < fTargetY)
		return true;

	return false;
}

HRESULT CTransform::Scaled(_fvector vScale)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);


	vRight = XMVector3Normalize(vRight) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(vUp) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::OneScaled(STATE eState, _float fScale)
{
	_vector vTarget;
	vTarget = Get_State(eState);
	vTarget = XMVector3Normalize(vTarget) * fScale;

	Set_State(eState, vTarget);

	return S_OK;
}

HRESULT CTransform::Scaling(_double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vRight += XMVector3Normalize(vRight) * m_TransformDesc.fScalingPerSec * (_float)TimeDelta;
	vUp += XMVector3Normalize(vUp) * m_TransformDesc.fScalingPerSec * (_float)TimeDelta;
	vLook += XMVector3Normalize(vLook) * m_TransformDesc.fScalingPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::OneScaling(STATE eState, _double TimeDelta)
{
	_vector		vTargetScale = Get_State(eState);

	vTargetScale += XMVector3Normalize(vTargetScale) * m_TransformDesc.fScalingPerSec * (_float)TimeDelta;

	Set_State(eState, vTargetScale);

	return S_OK;
}

_bool CTransform::ScaledLerp(_fvector fStartScale, _fvector fDestScale, _float fLerpRatio, _double TimeDelta)
{
	if (fLerpRatio >= 1.0)
		fLerpRatio = 1.0;

	_vector	vRight = Get_State(CTransform::STATE_RIGHT);
	_vector	vUp    = Get_State(CTransform::STATE_UP);
	_vector	vLook  = Get_State(CTransform::STATE_LOOK);

	_vector vLerpScale = XMVectorLerp(fStartScale, fDestScale, fLerpRatio);

	vRight = XMVector4Normalize(vRight) * XMVectorGetX(vLerpScale);
	vUp    = XMVector4Normalize(vUp)	* XMVectorGetY(vLerpScale);
	vLook  = XMVector4Normalize(vLook)	* XMVectorGetZ(vLerpScale);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	if (fLerpRatio >= 1.0)
		return true;

	return false;
}

HRESULT CTransform::Chase(_fvector vTargetPos, _double TimeDelta, CNavigation* pNavigation)
{
	_vector	vDir = XMVector4Normalize(vTargetPos - Get_State(STATE_POSITION));

	Go_Vector(vDir, TimeDelta, pNavigation);

	return S_OK;
}

HRESULT CTransform::Chase(CTransform* pTargetTransform, _double TimeDelta, CNavigation* pNavigation)
{
	_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	_vector	vDir = XMVector4Normalize(vTargetPos - Get_State(STATE_POSITION));
	Go_Vector(vDir, TimeDelta, pNavigation);

	return S_OK;
}

HRESULT CTransform::LookAt(_fvector vTargetPos)
{
	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vRight, vUp, vLook;

	vLook = vTargetPos - vPosition;
	vLook = XMVector4Normalize(vLook);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));

	return S_OK;
}

HRESULT CTransform::LookAt(CTransform* pTargetTransform)
{
	_vector	vPosition  = Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = pTargetTransform->Get_Position();

	_vector vRight, vUp, vLook;

	vLook = vTargetPos - vPosition;
	vLook = XMVector4Normalize(vLook);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));

	return S_OK;
}

HRESULT CTransform::LookAtXZ(_fvector vTargetPos)
{
	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vRight, vUp, vLook;

	vLook = vTargetPos - vPosition;
	vLook = XMVectorSetY(vLook, 0.f);
	vLook = XMVector4Normalize(vLook);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));

	return S_OK;
}

HRESULT CTransform::LookAtXZ(CTransform* pTargetTransform)
{
	_vector	vTargetPosition = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	LookAtXZ(vTargetPosition);

	return S_OK;
}

HRESULT CTransform::LookAtY(_fvector vTargetPos)
{
	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vRight, vUp, vLook;

	vUp = vTargetPos - vPosition;
	vUp = XMVector4Normalize(vUp);

	vLook = XMVector3Cross(XMVectorSet(1.f, 0.f, 0.f, 0.f), vUp);
	vLook = XMVector4Normalize(vLook);

	vRight = XMVector3Cross(vUp, vLook);
	vRight = XMVector4Normalize(vRight);

	//vLook = vTargetPos - vPosition;
	//vLook = XMVector4Normalize(vLook);

	//vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	//vRight = XMVector4Normalize(vRight);

	//vUp = XMVector3Cross(vLook, vRight);
	//vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));


	return S_OK;
}

HRESULT CTransform::LookTurn(_fvector vAxis, _fvector vTarget, _double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPos = Get_State(CTransform::STATE_POSITION);

	_vector vDir = vTarget - vPos;
	vDir = XMVector4Normalize(XMVectorSetY(vDir, 0.f));

	_float fAngle = XMConvertToDegrees(acos(XMVectorGetX(XMVector4Dot(vDir, vLook))));

	_float d = -(XMVectorGetX(vRight) * XMVectorGetX(vPos) + XMVectorGetY(vRight) * XMVectorGetY(vPos) + XMVectorGetZ(vRight) * XMVectorGetZ(vPos));
	_float LR = XMVectorGetX(vRight) * XMVectorGetX(vTarget) + XMVectorGetY(vRight) * XMVectorGetY(vTarget) + XMVectorGetZ(vRight) * XMVectorGetZ(vTarget) + d;
	if (0 > LR)
		fAngle = -fAngle;

	if (5.f > fAngle && -5.f < fAngle || XMVector3Equal(vAxis, XMVectorSet(0.f,0.f,0.f,0.f)))
	{
		LookAtXZ(vTarget);
	}
	else
	{
		_matrix	RotationMatrix;
		RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fAngle) * (_float)TimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);

		Set_State(CTransform::STATE_RIGHT, vRight);
		Set_State(CTransform::STATE_UP, vUp);
		Set_State(CTransform::STATE_LOOK, vLook);
	}


	return S_OK;
}

_float CTransform::Get_Distance(CTransform* pTargetTransform)
{
	_vector vTargetPosition = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = Get_State(CTransform::STATE_POSITION);

	return XMVectorGetX(XMVector3Length(vTargetPosition - vPosition));
}

_float CTransform::Get_Distance(_fvector vTargetPosition)
{
	_vector vPosition = Get_State(CTransform::STATE_POSITION);

	return XMVectorGetX(XMVector3Length(vTargetPosition - vPosition));
}

_vector CTransform::Get_Dir(CTransform* pTargetTransform)
{
	_vector vTargetPosition = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = Get_State(CTransform::STATE_POSITION);

	return XMVector3Normalize(vTargetPosition - vPosition);
}

_vector CTransform::Get_Dir(_fvector vTargetPosition)
{
	_vector vPosition = Get_State(CTransform::STATE_POSITION);

	return XMVector3Normalize(vTargetPosition - vPosition);
}

_bool CTransform::Turn_To_Direction(_fvector vDir, _double TimeDelta, _float fDiff)
{
	// 자신의 Look 가져와서 매개변수로 들어온 vDir(자신과 타겟 간의 방향벡터) 과 외적(y 값은 0.f 로 변경)
	// 외적의 결과로 Up 벡터가 하나 나옴 -> y값이 + 면 오른쪽으로, - 면 왼쪽으로 돌기
	_vector vLook      = XMVector3Normalize(XMVectorSetY(Get_State(CTransform::STATE_LOOK), 0.f));
	_vector vDirection = XMVectorSetY(vDir, 0.f);
	vDirection         = XMVector3Normalize(vDirection);
	_vector vCross     = XMVector3Cross(vLook, vDirection);

	// vLook 과 vDirection 비교해서 차이가 적으면 return
	if (XMVectorGetX(XMVector3Length(vDirection - vLook)) < fDiff)
		return TRUE;

	if (XMVectorGetY(vCross) > 0.f) // 오른쪽으로 돌기
		Turn_Right(Get_State(CTransform::STATE_UP), TimeDelta);
	else // 왼쪽으로 돌기
		Turn_Left(Get_State(CTransform::STATE_UP), TimeDelta);

	return FALSE;
}

_bool CTransform::Turn_To_Direction(CTransform* pTargetTransform, _double TimeDelta, _float fDiff)
{
	_vector vDir = Get_Dir(pTargetTransform);

	_vector vLook      = XMVector3Normalize(XMVectorSetY(Get_State(CTransform::STATE_LOOK), 0.f));
	_vector vDirection = XMVectorSetY(vDir, 0.f);
	vDirection         = XMVector3Normalize(vDirection);
	_vector vCross     = XMVector3Cross(vLook, vDirection);

	// vLook 과 vDirection 비교해서 차이가 적으면 return
	if (XMVectorGetX(XMVector3Length(vDirection - vLook)) < fDiff)
		return TRUE;

	if (XMVectorGetY(vCross) > 0.f) // 오른쪽으로 돌기
		Turn_Right(Get_State(CTransform::STATE_UP), TimeDelta);
	else // 왼쪽으로 돌기
		Turn_Left(Get_State(CTransform::STATE_UP), TimeDelta);

	return FALSE;
}

_bool CTransform::Turn_To_Direction_AxisX(CTransform* pTargetTransform, _double TimeDelta, _float fDiff)
{
	_vector vPos       = Get_Position();
	_vector vTargetPos = pTargetTransform->Get_Position();

	_vector vLook = XMVector3Normalize(Get_Look());
	_vector vDir  = XMVectorSetX(Get_Dir(pTargetTransform), vecx(vLook));
	vDir          = XMVectorSetZ(vDir, vecz(vLook));
	vDir          = XMVector3Normalize(vDir);

	if (XMVectorGetX(XMVector3Length(vLook - vDir)) < fDiff)
	{
		MakeSameDirection(vDir);
		return true;
	}

	if (XMVectorGetY(vPos - vTargetPos) > 0.f) // 아래로 돌기 
		Turn_Down(TimeDelta);
	else if (XMVectorGetY(vPos - vTargetPos) < 0.f) // 위로 돌기
		Turn_Up(TimeDelta);

	return false;
}

_bool CTransform::Turn_To_Direction_AxisX(_fvector vTargetPosition, _double TimeDelta, _float fDiff)
{
	_vector vPos = Get_Position();
	_vector vTargetPos = vTargetPosition;

	_vector vLook = XMVector3Normalize(Get_Look());
	_vector vDir = XMVectorSetX(Get_Dir(vTargetPos), vecx(vLook));
	vDir = XMVectorSetZ(vDir, vecz(vLook));
	vDir = XMVector3Normalize(vDir);

	if (XMVectorGetX(XMVector3Length(vLook - vDir)) < fDiff)
	{
		MakeSameDirection(vDir);
		return true;
	}

	if (XMVectorGetY(vPos - vTargetPos) > 0.f) // 아래로 돌기 
		Turn_Down(TimeDelta);
	else if (XMVectorGetY(vPos - vTargetPos) < 0.f) // 위로 돌기
		Turn_Up(TimeDelta);

	return false;
}

_bool CTransform::Go_To_Target(CTransform* pTargetTransform, _float fDistDiff, _double TimeDelta, CNavigation* pNavigation)
{
	_float fDistance = Get_Distance(pTargetTransform);

	// 타겟과의 거리가 fDistDiff 이하라면 true 리턴하고 이동 중지
	if (fDistance <= fDistDiff)
		return true;

	_vector vDir = Get_Dir(pTargetTransform);

	Go_Vector(vDir, TimeDelta, pNavigation);

	return false;
}

_bool CTransform::Go_To_Target(_fvector vTargetPos, _float fDistDiff, _double TimeDelta, CNavigation* pNavigation)
{
	_float fDistance = Get_Distance(vTargetPos);

	// 타겟과의 거리가 fDistDiff 이하라면 true 리턴하고 이동 중지
	if (fDistance <= fDistDiff)
		return true;

	_vector vDir = Get_Dir(vTargetPos);

	Go_Vector(vDir, TimeDelta, pNavigation);

	return false;
}

_bool CTransform::Go_To_TargetXZ(CTransform* pTargetTransform, _float fDistDiff, _double TimeDelta, CNavigation* pNavigation)
{
	_float fDistance = Get_Distance(pTargetTransform);

	// 타겟과의 거리가 fDistDiff 이하라면 true 리턴하고 이동 중지
	if (fDistance <= fDistDiff)
		return true;

	_vector vDir = XMVectorSetY(Get_Dir(pTargetTransform), 0.f);

	Go_Vector(vDir, TimeDelta, pNavigation);

	return false;
}

_bool CTransform::Go_To_TargetXZ(_fvector vTargetPos, _float fDistDiff, _double TimeDelta, CNavigation* pNavigation)
{
	_float fDistance = Get_Distance(vTargetPos);

	// 타겟과의 거리가 fDistDiff 이하라면 true 리턴하고 이동 중지
	if (fDistance <= fDistDiff)
		return true;

	_vector vDir = XMVectorSetY(Get_Dir(vTargetPos), 0.f);

	Go_Vector(vDir, TimeDelta, pNavigation);

	return false;
}

_bool CTransform::Go_To_TargetXZ_SetHeight(CTransform* pTargetTransform, _float fDistDiff, _float fTargetHeight, _double SetHeightSpeedRatio, _double TimeDelta, CNavigation* pNavigation)
{
	_bool bGoToTarget = false;
	_bool bSetHeight  = false;

	// Go_To_TargetXZ
	{
		_float fDistance = Get_Distance(pTargetTransform);

		// 타겟과의 거리가 fDistDiff 이하라면 bGoToTarget 에 true 할당
		if (fDistance <= fDistDiff)
			bGoToTarget = true;
		else
		{
			// 아니라면 타겟으로의 방향 중 Y 값을 0 으로 만들고 XZ 로만 이동
			_vector vDir = XMVectorSetY(Get_Dir(pTargetTransform), 0.f);
			Go_Vector(vDir, TimeDelta, pNavigation);
		}
	}

	// SetHeight
	{
		_vector	vPosition = Get_State(CTransform::STATE_POSITION);

		if (pNavigation != nullptr)
		{
			// 현재 네비게이션 위치에 따라 타겟높이 재설정
			_float fNaviY = pNavigation->Compute_Y(vPosition);
			if (fTargetHeight <= fNaviY)
				fTargetHeight = fNaviY;
		}

		if (vecy(Get_Position()) <= fTargetHeight)
		{
			Set_Position(XMVectorSetY(Get_Position(), fTargetHeight));
			bSetHeight = true;
		}
		else
			Go_Down(TimeDelta * SetHeightSpeedRatio);
	}

	if (bGoToTarget && bSetHeight)
		return true;

	return false;
}

_bool CTransform::Go_To_TargetXZ_SetHeight(_fvector vTargetPos, _float fDistDiff, _float fTargetHeight, _double SetHeightSpeedRatio, _double TimeDelta, CNavigation* pNavigation)
{
	_bool bGoToTarget = false;
	_bool bSetHeight  = false;

	// Go_To_TargetXZ
	{
		_float fDistance = Get_Distance(vTargetPos);

		// 타겟과의 거리가 fDistDiff 이하라면 bGoToTarget 에 true 할당
		if (fDistance <= fDistDiff)
			bGoToTarget = true;
		else
		{
			// 아니라면 타겟으로의 방향 중 Y 값을 0 으로 만들고 XZ 로만 이동
			_vector vDir = XMVectorSetY(Get_Dir(vTargetPos), 0.f);
			Go_Vector(vDir, TimeDelta, pNavigation);
		}
	}

	// SetHeight
	{
		_vector	vPosition = Get_State(CTransform::STATE_POSITION);

		if (pNavigation != nullptr)
		{
			// 현재 네비게이션 위치에 따라 타겟높이 재설정
			_float fNaviY = pNavigation->Compute_Y(vPosition);
			if (fTargetHeight <= fNaviY)
				fTargetHeight = fNaviY;
		}

		if (vecy(Get_Position()) <= fTargetHeight)
		{
			Set_Position(XMVectorSetY(Get_Position(), fTargetHeight));
			bSetHeight = true;
		}
		else
			Go_Down(TimeDelta * SetHeightSpeedRatio);
	}

	if (bGoToTarget && bSetHeight)
		return true;

	return false;
}

CTransform::DAMAGEDDIR CTransform::Get_Damaged_4Dir(CTransform* pTargetTransform)
{
	_vector vLook = XMVector3Normalize(Get_Look());
	_vector vDir  = XMVector3Normalize(Get_Dir(pTargetTransform));

	_float fCosineValue = XMVectorGetX(XMVector3Dot(vLook, vDir));
	_float fCrossYValue = XMVectorGetY(XMVector3Cross(vLook, vDir));
	
	if (fCosineValue >= 0.7f) // fCosineValue 가 0.7f 이상이면 앞
		return CTransform::DAMAGED_FRONT;
	else if (fCosineValue <= -0.7f) // fCosineValue 가 -0.7f 이하면 뒤
		return CTransform::DAMAGED_BACK;
	else if (fCrossYValue < 0.f) // fCosineValue 가 -0.7f ~ 0.7f 사이면서 fCrossYValue 가 음수면 왼쪽
		return CTransform::DAMAGED_LEFT;
	else if (fCrossYValue > 0.f) // fCosineValue 가 -0.7f ~ 0.7f 사이면서 fCrossYValue 가 양수면 오른쪽
		return CTransform::DAMAGED_RIGHT;

	return CTransform::DAMAGED_FRONT;
}

CTransform::DAMAGEDDIR CTransform::Get_Damaged_4Dir(_fvector vTargetPosition)
{
	_vector vLook = XMVector3Normalize(Get_Look());
	_vector vDir = XMVector3Normalize(Get_Dir(vTargetPosition));

	_float fCosineValue = XMVectorGetX(XMVector3Dot(vLook, vDir));
	_float fCrossYValue = XMVectorGetY(XMVector3Cross(vLook, vDir));

	if (fCosineValue >= 0.7f) // fCosineValue 가 0.7f 이상이면 앞
		return CTransform::DAMAGED_FRONT;
	else if (fCosineValue <= -0.7f) // fCosineValue 가 -0.7f 이하면 뒤
		return CTransform::DAMAGED_BACK;
	else if (fCrossYValue < 0.f) // fCosineValue 가 -0.7f ~ 0.7f 사이면서 fCrossYValue 가 음수면 왼쪽
		return CTransform::DAMAGED_LEFT;
	else if (fCrossYValue > 0.f) // fCosineValue 가 -0.7f ~ 0.7f 사이면서 fCrossYValue 가 양수면 오른쪽
		return CTransform::DAMAGED_RIGHT;

	return CTransform::DAMAGED_FRONT;
}

CTransform::DAMAGEDDIR CTransform::Get_Damaged_2Dir(CTransform* pTargetTransform)
{
	_vector vLook = XMVector3Normalize(Get_Look());
	_vector vDir  = XMVector3Normalize(Get_Dir(pTargetTransform));

	_float fCosineValue = XMVectorGetX(XMVector3Dot(vLook, vDir));

	if (fCosineValue >= 0)
		return CTransform::DAMAGED_FRONT;
	
	return CTransform::DAMAGED_BACK;
}

CTransform::DAMAGEDDIR CTransform::Get_Damaged_2Dir(_fvector vTargetPosition)
{
	_vector vLook = XMVector3Normalize(Get_Look());
	_vector vDir = XMVector3Normalize(Get_Dir(vTargetPosition));

	_float fCosineValue = XMVectorGetX(XMVector3Dot(vLook, vDir));

	if (fCosineValue >= 0)
		return CTransform::DAMAGED_FRONT;

	return CTransform::DAMAGED_BACK;
}

_float CTransform::Get_Distance_From_Mouse()
{
	auto CollisionCheck = CMouse::GetInstance()->Check_Ray_VirtualPlane_Collision(XMVectorGetY(Get_Position()));

	if (CollisionCheck.first == false)
		return 0.f;

	return Get_Distance(CollisionCheck.second);
}

void CTransform::Random_Rotation(_double TimeDeltla)
{
	_uint randNum = rand() % 3;

	if (0 == randNum)
		Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), TimeDeltla);
	else if (1 == randNum)
		Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDeltla);
	else
		Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), TimeDeltla);
}

_bool CTransform::Move_To_Target_Lerp(_fvector vStartPos, _fvector vDestPos, _float fLerpPercentage, _double TimeDelta)
{
	// 보간 퍼센테이지가 1보다 커지면 목표지점을 넘어서기 때문에 true 리턴해서 보간 끝났음을 알림
	if (fLerpPercentage > 1.f)
		return true;

	// fLerpPercentage 만큼 보간된 위치를 계산
	_vector vLerpPos = XMVectorLerp(vStartPos, vDestPos, fLerpPercentage);

	// 위치 적용
	Set_State(CTransform::STATE_POSITION, vLerpPos);

	// 보간이 끝나지 않았으면 false 리턴
	return false;
}

_bool CTransform::Look_Target_Lerp(_fvector vDestPos, _float fLerpPercentage, _double TimeDelta, _bool bRemoveY)
{
	// 보간 퍼센테이지가 1보다 커지면 목표지점을 넘어서기 때문에 true 리턴해서 보간 끝났음을 알림
	if (fLerpPercentage > 1.f)
		return true;

	// 현재 Transform 값들을 Normalize 한 값 
	_vector vPosition  = Get_State(CTransform::STATE_POSITION);
	_vector vPrevRight = XMVector4Normalize(Get_State(CTransform::STATE_RIGHT));
	_vector vPrevUp    = XMVector4Normalize(Get_State(CTransform::STATE_UP));
	_vector vPrevLook  = XMVector4Normalize(Get_State(CTransform::STATE_LOOK));

	// 현재 Transform 의 Scale 값 저장
	_float	vRightScale = XMVectorGetX(Get_Scale(CTransform::STATE_RIGHT));
	_float	vUpScale    = XMVectorGetX(Get_Scale(CTransform::STATE_UP));
	_float	vLookScale  = XMVectorGetX(Get_Scale(CTransform::STATE_LOOK));

	// 선형 보간 계산할 변수들
	_vector vNewRight, vNewUp, vNewLook;

	if (TRUE == bRemoveY)
		vNewLook = XMVectorSetY(vDestPos - vPosition, 0.f);
	else
		vNewLook = vDestPos - vPosition;

	vNewLook = XMVector4Normalize(vNewLook);

	vNewRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vNewLook);
	vNewRight = XMVector4Normalize(vNewRight);

	vNewUp    = XMVector3Cross(vNewLook, vNewRight);
	vNewUp    = XMVector4Normalize(vNewUp);

	_vector vFinalRight, vFinalUp, vFinalLook;

	// fLerpPercentage 만큼 보간된 축을 계산
	vFinalRight = XMVector4Normalize(XMVectorLerp(vPrevRight, vNewRight, fLerpPercentage)) * vRightScale;
	vFinalUp    = XMVector4Normalize(XMVectorLerp(vPrevUp, vNewUp, fLerpPercentage)) * vUpScale;
	vFinalLook  = XMVector4Normalize(XMVectorLerp(vPrevLook, vNewLook, fLerpPercentage)) * vLookScale;

	// 축 적용
	Set_State(CTransform::STATE_RIGHT, vFinalRight);
	Set_State(CTransform::STATE_UP, vFinalUp);
	Set_State(CTransform::STATE_LOOK, vFinalLook);

	return false;
}

HRESULT CTransform::MakeSameDirection(CTransform * pTransform)
{
	_vector vRight, vUp, vLook;

	vLook = XMVector4Normalize(pTransform->Get_State(CTransform::STATE_LOOK));

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));

	return S_OK;
}

HRESULT CTransform::MakeSameDirection(_fvector vDir)
{
	_vector vRight, vUp, vLook;

	vLook = XMVector4Normalize(vDir);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight = XMVector4Normalize(vRight);

	vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector4Normalize(vUp);

	Set_State(CTransform::STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(CTransform::STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(CTransform::STATE_LOOK, vLook * Get_Scale(STATE_LOOK));

	return S_OK;
}

void CTransform::SpiralMoveXZ(_double TimeDelta, CNavigation* pNavigation)
{
	Go_Straight(TimeDelta, pNavigation); 
	Turn_Left(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta);
}

void CTransform::JumpInput(_float fHeight, _float fPower, _float fSpeed, _bool bJump, _bool bGravityEffect)
{
	m_JumpDesc.bJump = bJump;
	m_JumpDesc.bGravityEffect = bGravityEffect;

	m_JumpDesc.fJumpHeight = fHeight;
	
	if (1.f == fSpeed)
	{
		m_JumpDesc.fJumpSpeed = fSpeed;
		m_JumpDesc.fJumpFactor = 1.f;

		m_JumpDesc.fJumpPower = fPower;
	}
	else
	{
		m_JumpDesc.fJumpSpeed = fSpeed;
		m_JumpDesc.fJumpFactor = m_JumpDesc.fJumpSpeed * 0.5f;

		m_JumpDesc.fJumpPower = fPower * m_JumpDesc.fJumpFactor;
	}

	m_JumpDesc.dJumpTime = 0.0;
}

void CTransform::BasicJump(JUMP_TYPE eJumpType)
{
	m_JumpDesc.eJumpType = eJumpType;

	_vector current_position = Get_State(CTransform::STATE_POSITION);
	switch (eJumpType)
	{
	case Engine::CTransform::JUMP_NORMAL:

		JumpInput(vecy(current_position), m_JumpDesc.fJumpValue);

		break;
	case Engine::CTransform::JUMP_FALL:

		JumpInput(vecy(current_position), 0.f, 1.f, true);

		break;
	case Engine::CTransform::JUMP_CONTINUE:

		_vector position = Get_Position();

		m_JumpDesc.bJump = true;
		m_JumpDesc.bGravityEffect = true;

		m_JumpDesc.fJumpHeight = vecy(position);

		break;
	case Engine::CTransform::JUMP_LAND:

		JumpInput(vecy(current_position), 0.f, 1.f, false);

		break;
	case Engine::CTransform::JUMP_Editional1:

		JumpInput(vecy(current_position), m_JumpDesc.fJumpValue * 1.35f);

		break;
	case Engine::CTransform::JUMP_Editional2:

		JumpInput(vecy(current_position), m_JumpDesc.fJumpValue * 1.15f);

		break;
	}
}

_float CTransform::Calc_DirectionOnJump(_float fGravity)
{
	return m_JumpDesc.fJumpPower * (_float)m_JumpDesc.dJumpTime - fGravity * (_float)m_JumpDesc.dJumpTime * (_float)m_JumpDesc.dJumpTime * 0.5f;
}

void CTransform::MonsterJump(_double TimeDelta, _float fMinY)
{
	m_MonsterJumpDesc.JumpTimeAcc += TimeDelta * m_MonsterJumpDesc.fJumpSpeed;
	_float fY = m_MonsterJumpDesc.fOriginY + ((m_MonsterJumpDesc.fJumpPower * (_float)m_MonsterJumpDesc.JumpTimeAcc) - ((_float)m_MonsterJumpDesc.JumpTimeAcc * (_float)m_MonsterJumpDesc.JumpTimeAcc * 5.2f * 0.5f));

	if (fY < fMinY)
		fY = fMinY;

	_vector vPosition = Get_Position();
	vPosition = XMVectorSetY(vPosition, fY);
	Set_Position(vPosition);
}

void CTransform::KnockBackInput(_float3 vDirection, _float fPower, _double dTimer)
{
	m_KnockBackDesc.vDirection = vDirection;

	m_KnockBackDesc.fInputPower = fPower;
	m_KnockBackDesc.fPower = fPower;

	m_KnockBackDesc.dInputTimer = dTimer;
	m_KnockBackDesc.dTimer = dTimer;
}

void CTransform::KnockBackInput_v(_fvector vDirection, _float fPower, _double dTimer)
{
	_float3 direction_f3 = { 0.f, 0.f, 0.f };
	XMStoreFloat3(&direction_f3, vDirection);

	KnockBackInput(direction_f3, fPower, dTimer);
}

_int CTransform::KnockBackTick(_double TimeDelta, CNavigation* pNavigation)
{
	if (0.0 >= m_KnockBackDesc.dTimer)
		return RESULT_NOPROBLEM;

	_vector knockback_direction = XMVector3Normalize((XMLoadFloat3(&m_KnockBackDesc.vDirection)));
	m_KnockBackDesc.fPower = m_KnockBackDesc.fInputPower - Calculate_KnockBack_Attenuation();

	Go_Vector(knockback_direction, TimeDelta * m_KnockBackDesc.fPower, pNavigation);

	m_KnockBackDesc.dTimer -= TimeDelta;

	if (0.0 >= m_KnockBackDesc.dTimer)
		m_KnockBackDesc.dTimer = 0.0;

	return RESULT_NOPROBLEM;
}

_float CTransform::Calculate_KnockBack_Attenuation()
{
	// EaseInQuint 방식
	_float point = (_float)m_KnockBackDesc.dTimer / (_float)m_KnockBackDesc.dInputTimer;
	return m_KnockBackDesc.fInputPower - (m_KnockBackDesc.fInputPower * point * point * point * point * point);
}

void CTransform::LocationAssistanceInput(_float fMoveAmountY, _double dDuration, _double dTimerSpeed)
{
	m_LocationDesc.fInputHeight = fMoveAmountY;

	m_LocationDesc.dInputTimer = dDuration;
	m_LocationDesc.dTimer = 0.0;
	m_LocationDesc.dTimerSpeed = dTimerSpeed;
}

_int CTransform::LocationAssistanceTick(_double Timedelta, CNavigation * pNavigation)
{
	if (0.0 == m_LocationDesc.dInputTimer)
		return RESULT_NOPROBLEM;

	_vector position = Get_Position();
	_vector moved_position = position;
	vecy(moved_position) += m_LocationDesc.fInputHeight * (_float)Timedelta * (_float)m_LocationDesc.dTimerSpeed / (_float)m_LocationDesc.dInputTimer;
	
	if(true == pNavigation->Move_OnNavigation(moved_position, position, nullptr))
		Set_Position(moved_position);

	m_LocationDesc.dTimer += Timedelta * m_LocationDesc.dTimerSpeed;

	if (m_LocationDesc.dInputTimer < m_LocationDesc.dTimer)
	{
		m_LocationDesc.dInputTimer = 0.0;
		m_LocationDesc.dTimer = 0.0;
		m_LocationDesc.dTimerSpeed = 0.0;
	}

	return RESULT_NOPROBLEM;
}

void CTransform::Sync_Animation(_vector vMoveValue, _double TimeDelta, CNavigation* pNavigation)
{
	_vector vPosition	= Get_State(CTransform::STATE_POSITION);
	_vector vRight		= Get_State(CTransform::STATE_RIGHT);
	_vector vUp			= Get_State(CTransform::STATE_UP);
	_vector vLook		= Get_State(CTransform::STATE_LOOK);

	// 이동 연산 전의 위치
	_vector vCurrentPosition = vPosition;
	_float3* pSlidingVector = nullptr;

	// 이동 연산 후의 위치
	vPosition += XMVector3Normalize(vRight) * -XMVectorGetX(vMoveValue);
	vPosition += XMVector3Normalize(vUp)	* -XMVectorGetZ(vMoveValue);
	vPosition += XMVector3Normalize(vLook)  * -XMVectorGetY(vMoveValue);

	if (nullptr == pNavigation)
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return;
	}

	if (true == pNavigation->Move_OnNavigation(vPosition, vCurrentPosition, &pSlidingVector))
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
		return;
	}

	if (nullptr != pSlidingVector)
	{
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&_float3((*pSlidingVector).z, 0.f, -(*pSlidingVector).x)));
		_vector vDir = XMVector4Normalize(-Get_State(STATE_LOOK));
		_float fDotNormalScale = XMVectorGetX(XMVector3Dot(vDir, vNormal));
		vDir = -vDir + vNormal * fDotNormalScale;
		_float fScale = XMVectorGetX(XMVector4Length(vDir));

		// 슬라이딩 벡터로 이동했을 때 이동할 수 있는지 한번 더 검사
		if (true == pNavigation->Move_OnNavigation(vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta, vCurrentPosition, &pSlidingVector))
		{
			Set_State(CTransform::STATE_POSITION, vCurrentPosition + vDir * fScale *  m_TransformDesc.fSpeedPerSec * (_float)TimeDelta);
		}
	}
}

CTransform * CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}