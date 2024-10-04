#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CHolo_1 final : public CGameObject
{
public:
	typedef struct HoloDesc
	{
		_float3 vPos;
		_float3 vRot;
		_float3 vScale;

		CGameObject* pParentObj = nullptr;

	}HOLODESC;

private:
	CHolo_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHolo_1(const CHolo_1& rhs);
	virtual ~CHolo_1() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


private:
	_float m_fTimeDelta = 0.f;
	_float m_fStartAlpa = 0.f;
	_float m_fStartTime = 0.f;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

protected:
	HRESULT		Ready_Components();

public:
	static CHolo_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END