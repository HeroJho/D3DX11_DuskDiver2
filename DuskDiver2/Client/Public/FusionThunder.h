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

class CFusionThunder : public CEffect
{
public:
	typedef struct tagFusionThunderInfo
	{
		_float3 vScale;
		_float4 vPos;
		_float  fSpeed;
	}THUNDER1INFO;
private:
	CFusionThunder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFusionThunder(const CFusionThunder& rhs);
	virtual ~CFusionThunder() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void Render_Debug() override;


private:
	THUNDER1INFO* m_Info = nullptr;
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
	static CFusionThunder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END