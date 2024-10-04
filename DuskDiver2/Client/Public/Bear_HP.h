#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)
class CBear;

class CBear_HP final : public CUI
{
private:
	CBear_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBear_HP(const CBear_HP& rhs);
	virtual ~CBear_HP() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	_bool m_bDebug = false;
	_bool m_bRender = false;
	_float m_fDeadAcc = 0.f;

public:
	static CBear_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END