#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CHolo_7 final : public CGameObject
{
private:
	CHolo_7(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHolo_7(const CHolo_7& rhs);
	virtual ~CHolo_7() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	void Set_StartTime(_float fStartTime) { m_fStartTime = fStartTime; }
private:
	_float m_fTimeDelta = 0.f;
	CGameObject* m_pParent = nullptr;

	_float3 vLocalPos{ 0.f, -0.1f, 0.f };
	_float m_fStartAlpa = 0.f;
	_float m_fStartTime = 0.f;


private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

protected:
	HRESULT		Ready_Components();

public:
	static CHolo_7* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END