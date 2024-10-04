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

class CEffectAtom final : public CGameObject
{
public:
	typedef struct tagEffectAtomDesc {
		_uint    iAtomIndex;
	}EFFECTATOMDESC;
private:
	CEffectAtom(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CEffectAtom(const CEffectAtom& rhs);
	virtual ~CEffectAtom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	void Set_StartState(_float4 vIniLook);
	void Find_MoveLook(_uint Index);

	void Update_Atom(_float fTimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool					m_bRender = false;
	_bool					m_bReverse = false; // 알파를 반전시키냐
	_float4					m_vMoveLook = { 0.f, 0.f, 0.f, 0.f };
	_float4					m_vFriction = { 0.f,0.f,0.f,0.f }; // x-> 수평면의 시작속도, y-> 수직 시작속도, z-> 수평면 마찰계수, w-> 수직 마찰계수
	_float					m_fColorAlphaUp = 0.f; // 흰색으로 만드는 용도
	_float					m_fColorAlphaDown = 0.f;// 까만색으로 만드는 용도
	_float					m_fAlpha = 1.f; // 서서히 사라지게 하는 용도

	EFFECTATOMDESC			m_EffectAtomDesc;


public:
	static CEffectAtom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END