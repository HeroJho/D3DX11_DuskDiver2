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


class CBeastBreath2 final : public CGameObject
{
public:

	typedef struct tagCBeastBreath2Info
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
	}BREATHINFO;		

	typedef struct tagFixInfo
	{
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower = 1.f;
		_float3 vScale = { 0.1f,0.1f,0.1f };
		_float2 vUvFixSpeed = { 1.f,0.f };
	}FIXINFO;

private:
	CBeastBreath2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CBeastBreath2(const CBeastBreath2& rhs);
	virtual ~CBeastBreath2() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Breath2Info(BREATHINFO Info);
	void Update_Breath2Matrix(_float4x4 vMatrix);
	void Fix_Info(FIXINFO Info);

private:
	HRESULT Ready_Components();

private:
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_float2 m_vUVFixSpeed = { 0.f,0.f };


private:
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	BREATHINFO* m_pInfo = nullptr;
public:
	static CGameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END