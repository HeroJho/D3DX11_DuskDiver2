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


class CWall final : public CEffect
{
public:
	typedef struct tagWallInfo
	{
		_float3 vSize;
		_float4 vWorldPos;
		_float2 vUVFixSpeed;
		_float4 vColor;
		_float4 vColor2;
		_float3 vFixPosSpeed;
		_float3 vFixScaleSpeed;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower = 0.f;
	}WALLINFO;		 

private:
	CWall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CWall(const CWall& rhs);
	virtual ~CWall() = default;

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
	WALLINFO* m_WallInfo = nullptr;
	_float m_fLifeTimeAcc = 0.f;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END