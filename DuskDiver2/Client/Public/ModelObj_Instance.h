#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CModel_Instance;
END

BEGIN(Client)

class CModelObj_Instance : public CGameObject
{
public:
	typedef struct ModelObjInstanceDesc
	{
		TCHAR sModelName[MAX_PATH];
		_uint iAnimIndex = 0;
	}MODELINSTANCEDESC;

private:
	CModelObj_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelObj_Instance(const CModelObj_Instance& rhs);
	virtual ~CModelObj_Instance() = default;


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
	CModel_Instance*		m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;


private:
	MODELINSTANCEDESC m_Desc;
	_float m_fDissolveAcc = 0.f;
	_bool m_bDissolve = false;
	_bool m_bRender = false;

protected:
	HRESULT Ready_Components();


public:
	static CModelObj_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END