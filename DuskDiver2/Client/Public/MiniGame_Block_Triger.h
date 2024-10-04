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

class CMiniGame_Block_Triger : public CGameObject
{
private:
	CMiniGame_Block_Triger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMiniGame_Block_Triger(const CMiniGame_Block_Triger& rhs);
	virtual ~CMiniGame_Block_Triger() = default;


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

private:
	HRESULT Ready_Components();


public:
	static CMiniGame_Block_Triger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END