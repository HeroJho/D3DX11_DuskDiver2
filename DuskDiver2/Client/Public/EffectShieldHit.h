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

class CEffectShieldHit final : public CGameObject
{
public:
	typedef struct tagEffectShieldHitDesc {
		_float4	   vPosition;
		_float4    vLook;
		_float	   fSize; //사이즈는 실드의 0.75배
	}EFFECTSHIELDHITDESC;
private:
	CEffectShieldHit(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CEffectShieldHit(const CEffectShieldHit& rhs);
	virtual ~CEffectShieldHit() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	void Set_InitState(_float4 NewPosition, _float4 NewLook);

	void Update_ShiledState(_float fTimeDelta, _float4 NewPosition);
	void Update_ShieldAlpha(_float fTimeDelta);
	void Update_ShieldPosition(_float4 NewPosition);

private:
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool					m_bRender = false;

	_float					m_fAlpha = 1.f;
	_float					m_fMoveV = 0.f; // 서서히 움직이지게 하는 용도

	EFFECTSHIELDHITDESC		m_EffectShieldHitDesc;


public:
	static CEffectShieldHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END