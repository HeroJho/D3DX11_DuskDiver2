#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)


class CPlayer01Pillar2 final : public CEffect
{
public:
	typedef struct tagCPlayer01Pillar2Info
	{
		_float4x4 vWorldMatrix;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower = 1.f;
		_float3 vScaleSpeed;
		_float4 vLocalPos = { 0.f,0.f,2.f,1.f };
		_float3 vScale = { 0.1f,0.1f,0.1f };
		_float2 vUvFixSpeed = { 1.f,0.f };
	}PILLAR2INFO;		 

private:
	CPlayer01Pillar2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer01Pillar2(const CPlayer01Pillar2& rhs);
	virtual ~CPlayer01Pillar2() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
private:
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	PILLAR2INFO* m_pInfo = nullptr;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END