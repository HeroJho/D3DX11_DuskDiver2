#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CAzitEvent_4 : public CGameObject
{
private:
	CAzitEvent_4(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAzitEvent_4(const CAzitEvent_4& rhs);
	virtual ~CAzitEvent_4() = default;


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


	_bool					m_bActivate = false;
	_bool					m_bOn = false;

	_float					m_fTimeAcc = 0.f;

private:
	HRESULT Ready_Components();


public:
	static CAzitEvent_4* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END