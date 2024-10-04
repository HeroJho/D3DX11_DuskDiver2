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

class CPlayer01HitEffect_2 : public CGameObject
{
public:
	typedef struct tagPlayer01HitEffect_2Info
	{
		//Front
		_float3 vRatio;
		_float3 vScale;
		_float4 vColor;
		_float4 vColor2;
		_float4 vPosition;	

	}HITEFFECT2INFO;
private:
	CPlayer01HitEffect_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer01HitEffect_2(const CPlayer01HitEffect_2& rhs);
	virtual ~CPlayer01HitEffect_2() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void Render_Debug() override;

private:

private:
	HITEFFECT2INFO* m_Info = nullptr;
	_matrix m_RotationMatrix;
	_float3 m_vCamLook;
	_float4 m_vBackPos;
	_float m_fAngle;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Particle*     m_pVIBufferCom = nullptr;
protected:
	HRESULT Ready_Components();

public:
	static CPlayer01HitEffect_2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END