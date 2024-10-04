#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Rigid.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CRigid;
END

BEGIN(Client)

class CPxObj : public CGameObject
{
public:
	typedef struct tagPxObj
	{
		TCHAR   sModelName[MAX_PATH];
		_float3 vScale;

		CRigid::RIGIDEDESC RigidDesc;

	}PXOBJDESC;

protected:
	CPxObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPxObj(const CPxObj& rhs);
	virtual ~CPxObj() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


public:
	void Set_Static(_bool bStatic);

	PXOBJDESC* Get_Desc() { return &m_Desc; }


protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	// CRigid*					m_pRigidCom = nullptr;
	CRigid*			m_pRigidCom = nullptr;


protected:
	PXOBJDESC		m_Desc;
	_bool m_bOther = false;

protected:
	HRESULT Ready_Components();


public:
	static CPxObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END