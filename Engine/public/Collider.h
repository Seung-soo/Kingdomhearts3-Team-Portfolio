#pragma once

#include "Component.h"

/*
1. �ݶ��̴��� �⺻ ����� ������ �ִ� BOUNDING_ORIGINAL
2. ColliderDesc�� ����ִ� ���� ������ ���� �� BOUNDING_TRANSFORM
3. ������ �޴� ����� ������ BOUNDING_WORLD_TRANSFORM. �̰� �������� �ݶ��̴� Ʈ������ ����.

*/


BEGIN(Engine)
class CGameObject;
class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
	enum BOUNDING { BOUNDING_ORIGINAL, BOUNDING_TRANSFORM, BOUNDING_WORLD_TRANSFORM, BOUNDING_END };
	enum STATE { STATE_ENTER, STATE_STAY, STATE_EXIT, STATE_END };
public:
	typedef struct tagColliderDesc
	{
		_float3 vScale;
		_float4 vRotation;
		_float4 vTranslation;
	}COLLIDERDESC;
private:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;
public:
	// �ݶ��̴� ���� ���� �Լ�
	void					Set_ColliderDesc(COLLIDERDESC tColliderDesc);
	void					Set_Scale(_float3 vScale);
	void					Set_Rotation(_float4 vRotation);
	void					Set_Translation(_float4 vTranslation);

	void					Set_Color(_float4 vColor);

	// �ݶ��̴� �浹 ���� ���� �Լ�
	_bool					Get_IsCollision() { return m_isCollision; }
	void					Set_IsCollision(_bool bIsCollision) { m_isCollision = bIsCollision; }

	// �ٿ���� �� �Ѵ�.
	BoundingBox*			Get_AABB() { return m_pAABB[BOUNDING_WORLD_TRANSFORM]; }
	BoundingOrientedBox*	Get_OBB() { return m_pOBB[BOUNDING_WORLD_TRANSFORM]; }
	BoundingSphere*			Get_Sphere() { return m_pSphere[BOUNDING_WORLD_TRANSFORM]; }

	TYPE					Get_Type() { return m_eType; }

	COLLIDERDESC			Get_ColliderDesc() { return m_ColliderDesc; }
	_float3					Get_Scale() { return m_ColliderDesc.vScale; }
	_float4					Get_Rotation() { return m_ColliderDesc.vRotation; }
	_float4					Get_Translation() { return m_ColliderDesc.vTranslation; }

	_float4					Get_Color() { return m_vColor; }

	_vector					Get_Center();
	void					Set_Center(_float3 vCenter);
	void					Set_Center(_vector vCenter);

	_float4*				Get_PositionPointer() { return &m_vPosition; }

	// ���ڷ� ���� �浹���¿� �ִ� �ݶ��̴� ���� �����̳��� �ּҸ� ��ȯ
	vector<CCollider*>*		Get_CollidingList(STATE eState) { return &m_Colliding[eState]; }
public:
	virtual HRESULT NativeConstruct_Prototype(TYPE eType);
	virtual HRESULT NativeConstruct(void* pArg) override;

	void			Update_Transform(_fmatrix TransformMatrix);
#ifdef _DEBUG
	virtual HRESULT	Render();
#endif
public:
	// �ݶ��̴� �浹 ����(Enter, Stay, Exit)�� ���� �ݶ��̴� �־���
	HRESULT Add_Colliding(STATE eState, CCollider* pCollider);
	void	Add_OvalapingParent_Colliding(STATE eState, CCollider* pCollider);
	// �浹�� �ݶ��̴� ã�� �Լ�
	STATE	Find_Colliding(CCollider* pCollider);

	CCollider::STATE Erase_Colliding(CCollider* pCollider);
	void Erase_Colliding_All(CCollider* pCollider);
	void Clear_Colliding_All();

private:
	// �������� �׸� �� �ִ� ���̴��� �����Ѵ�.
	BasicEffect*									m_pBasicEffect = nullptr;
	// �⺻���� ������ �׷� ���� �� �ִ� ���� ����, �ε��� ���۸� �����Ѵ�.
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = nullptr;

	ID3D11InputLayout*								m_pInputLayout = nullptr;

private:
	BoundingBox*			m_pAABB[BOUNDING_END] = { nullptr };
	BoundingOrientedBox*	m_pOBB[BOUNDING_END] = { nullptr };
	BoundingSphere*			m_pSphere[BOUNDING_END] = { nullptr };
	TYPE					m_eType = TYPE_END;

	COLLIDERDESC			m_ColliderDesc;

	_float4					m_vPosition;

	_float4					m_vColor;
	_bool					m_bCustomColor = false;

	// �浹����
	_bool					m_isCollision = false;

	// �浹 ���¿� ���� �浹�� �ݶ��̴��� ���ִ� ����
	vector<CCollider*>		m_Colliding[STATE_END];
	vector<CCollider*>		m_OverlappingParent_Colliding[STATE_END];
private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);
	void	Make_TransformMatrix();					// �ٿ�� Ʈ��������Ʈ���� ����� �Լ�


public:
	static	CCollider*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType);
	virtual CComponent*	Clone(void* pArg) override;
	virtual void		Free() override;
};

END