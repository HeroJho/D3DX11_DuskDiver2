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


class CEffectCraw final : public CEffect
{
public:
	typedef struct tagMonsterCrawInfo
	{
		_float4x4 vWorldMatrix;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float3  fScale;
		_float	fSpeed;
		_uint	iIndex = 0;
		_bool	bMove = false;
		_float fBlurPower = 1.f;
	}CRAWINFO;		 

private:
	CEffectCraw(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CEffectCraw(const CEffectCraw& rhs);
	virtual ~CEffectCraw() = default;

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
	CModel*					m_pModelCom = nullptr;
	CRAWINFO*				m_CrawInfo = nullptr;

private:
	_float4					m_vMyPos = {0.f, 0.f, 0.f, 0.f};
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END