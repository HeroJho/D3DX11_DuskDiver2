#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CRenderer;
class CVIBuffer_Particle;
END

BEGIN(Client)

class CPlayer03SniperEffect_1 : public CEffect
{
public:
	typedef struct tagPlayer03Sniper_1Info
	{
		_float3 vScale;
		_float4 vPos;
	}SNIPERINFO;
private:
	CPlayer03SniperEffect_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer03SniperEffect_1(const CPlayer03SniperEffect_1& rhs);
	virtual ~CPlayer03SniperEffect_1() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void Render_Debug() override;


private:
	SNIPERINFO* m_Info = nullptr;
	_float4 m_vColor = { 0.8f,0.8f,0.8f, 1.f };
	_float4 m_vColor2 = { 0.8f,0.8f,0.8f, 1.f };
	_float2 m_vRatio = { 0.8f,0.8f };

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Particle*     m_pVIBufferCom = nullptr;
protected:
	HRESULT Ready_Components();

public:
	static CPlayer03SniperEffect_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END