#pragma once

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CRenderer;
class CVIBuffer_Particle;
END

BEGIN(Client)

class CPlayer04VoidEffect_2 : public CGameObject
{
public:
	typedef struct tagPlayer04VoidEffect_2Info
	{
		_float4 vPosition;
	}P04VOIDEFFECT_2;
private:
	CPlayer04VoidEffect_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer04VoidEffect_2(const CPlayer04VoidEffect_2& rhs);
	virtual ~CPlayer04VoidEffect_2() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void Render_Debug() override;

private:
	_float m_fScale = 1.5f;
	_float m_fDeadAcc = 0.f;
	_float3 m_vRGB = { 1.f,1.f,1.f };


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Particle*     m_pVIBufferCom = nullptr;
protected:
	HRESULT Ready_Components();

public:
	static CPlayer04VoidEffect_2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END