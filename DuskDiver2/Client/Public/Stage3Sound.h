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

class CStage3Sound : public CGameObject
{
private:
	CStage3Sound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage3Sound(const CStage3Sound& rhs);
	virtual ~CStage3Sound() = default;


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

	_bool m_bSpawn = false;

protected:
	HRESULT Ready_Components();

	_bool m_bOnce = false;
	_bool m_bSound = false;
	_uint test = 0;
	_float m_fTimeAcc = 0.f;

public:
	static CStage3Sound* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END