#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CCreture;

class CEffectShield final : public CGameObject
{
public:
	typedef struct tagEffectShieldDesc {
		_float4    vPosition;
		_float4    vColor;
		_float	   fSize;
		_float     fMagicNum; 
		_float4 vColor2;
		_float2 vRatio;
		_float fBlurPower = 1.f;
		CCreture*  pDesc;
	}EFFECTSHIELDDESC;
private:
	CEffectShield(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CEffectShield(const CEffectShield& rhs);
	virtual ~CEffectShield() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void	Shield_OnOff(_bool	bOnOff) { m_bRender = bOnOff;  };

	void	Shield_Off() { m_bOff = true; };

	void	Reset_Ratio();

	void Update_ShiledState(_float fTimeDelta, _float4 NewPosition);


private:
	HRESULT Ready_Components();
	void Update_ShieldAlpha(_float fTimeDelta);
	void Update_ShieldPosition(_float4 NewPosition);

private:
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CCreture*				m_pDesc = nullptr; //객체
	_bool					m_bRender = false;
	_bool					m_bAlphaReverse = false;
	_bool					m_bOff = false;
	_float					m_fAlpha = 1.f; // 서서히 사라지게 하는 용도

	EFFECTSHIELDDESC			m_EffectShieldDesc;


public:
	static CEffectShield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END