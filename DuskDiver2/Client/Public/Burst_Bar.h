#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBurst_Bar final : public CUI
{
private:
	CBurst_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBurst_Bar(const CBurst_Bar& rhs);
	virtual ~CBurst_Bar() = default;

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

	_bool		m_bDebug = false;
	_float		m_fBurstBp = 0.f;
	_float		m_fLimitAngle = 0.f;

public:
	static CBurst_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END