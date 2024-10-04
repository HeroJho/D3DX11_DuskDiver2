#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CParts : public CGameObject
{
public:
	CParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParts(const CParts& rhs);
	virtual ~CParts() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override {};
	virtual void LateTick(_float fTimeDelta)override {};
	virtual HRESULT Render() override { return S_OK; }
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual void Render_Debug() override {};
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override {};

	virtual void On_Hit() {};
	virtual void Set_PartTransform(_int iTransformIndex) {};

public:
	HRESULT SetUp_State(_fmatrix StateMatrix);
	void Rotation(_float3 vAxis, _float fRadian, _float3 vAxis2, _float fRadian2, _float3 vAxis3, _float fRadian3);
	_float4 Get_Pos();
	void Set_Pos(_float4 Pos);
	_float4 Get_ParentPos();
	void Set_ParentPos(_float4 Pos);
	_float3 Get_ParentDir();
	_float4x4 Get_ParentMatrix();
	
protected:
	HRESULT Ready_Components();

protected:
	_int m_iTransfromIndex = 0;

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pParentTransformCom = nullptr;
public:
	static CParts* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END
