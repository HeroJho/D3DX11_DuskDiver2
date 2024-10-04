#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CBeastMap2 : public CEffect
{

private:
	CBeastMap2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastMap2(const CBeastMap2& rhs);
	virtual ~CBeastMap2() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	void Change() { m_bChange = true; }

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;


private:
	HRESULT Ready_Components();

private:

	_float4 m_vColor1 = { 0.2f,0.f,0.f,1.f };
	_float4 m_vColor2 = { 0.f,0.f,0.f,1.f };
	_float2 m_vRatio = { 1.f,1.f };
	_float	m_fBlurPower = 0.f;
	_float2 m_vUVFixSpeed = { -0.1f,-0.1f };
	_float2 m_vUVFixAcc = { 0.f,0.f };

	_float m_fDissolveAcc = 0.5f;
	_bool m_bDissolve = false;
	_int m_iPassIndex = 10;
	_bool m_bChange = false;

public:
	static CBeastMap2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEffect* Clone(void* pArg);
	virtual void Free() override;
};

END