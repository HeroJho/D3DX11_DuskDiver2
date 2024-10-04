#pragma once

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CViewAlpha : public CGameObject
{
public:
	typedef struct tagViewAlphaInfo
	{
		_float4 vColor1;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower;
	}VIEWALPHAINFO;
private:
	CViewAlpha(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CViewAlpha(const CViewAlpha& rhs);
	virtual ~CViewAlpha() = default;


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
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*     m_pVIBufferCom = nullptr;
	VIEWALPHAINFO* m_pViewAlphaInfo = nullptr;
protected:
	HRESULT Ready_Components();

public:
	static CViewAlpha* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END