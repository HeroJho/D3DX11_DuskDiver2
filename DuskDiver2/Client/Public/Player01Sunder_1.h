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

class CPlayer01Sunder_1 : public CEffect
{
public:
	typedef struct tagPlayer01Sunder_1Info
	{
		_float3 vScale;
		_float4 vPos;
	}SUNDER1INFO;
private:
	CPlayer01Sunder_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer01Sunder_1(const CPlayer01Sunder_1& rhs);
	virtual ~CPlayer01Sunder_1() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void Render_Debug() override;


private:
	SUNDER1INFO* m_Info = nullptr;
	_float4 m_vColor;
	_float4 m_vColor2;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Particle*     m_pVIBufferCom = nullptr;
protected:
	HRESULT Ready_Components();

public:
	static CPlayer01Sunder_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END