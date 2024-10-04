#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CRigid;
class CRigid_Sphere;
END

BEGIN(Client)

class CPxBox : public CGameObject
{
private:
	CPxBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPxBox(const CPxBox& rhs);
	virtual ~CPxBox() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	// CRigid*					m_pRigidCom = nullptr;
	CRigid*			m_pRigidCom = nullptr;

protected:
	HRESULT Ready_Components();


public:
	static CPxBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END