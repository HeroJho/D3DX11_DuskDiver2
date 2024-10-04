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

class CDGTrigger : public CGameObject
{
private:
	CDGTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDGTrigger(const CDGTrigger& rhs);
	virtual ~CDGTrigger() = default;


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


private:
	_bool m_bTrigger = false;
	_float m_fTimeAcc = 0.f;
	_bool m_bBlind = false;

private:
	HRESULT Ready_Components();


public:
	static CDGTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END