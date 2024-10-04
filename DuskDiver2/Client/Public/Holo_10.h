#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CHolo_10 final : public CCreture
{
private:
	CHolo_10(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHolo_10(const CHolo_10& rhs);
	virtual ~CHolo_10() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	void Set_StartTime(_float fStartTime) { m_fStartTime = fStartTime; }
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	// _float				g_RimPower = 0.f;

	CGameObject* m_pParent = nullptr;

	_float3 vLocalPos{ 0.f, -0.1f, 0.f };
	_float m_fTimeDelta = 0.f;

	_float m_fStartAlpa = 0.f;
	_float m_fStartTime = 0.f;


protected:
	HRESULT		Ready_Components();

private:

public:
	static CHolo_10* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END