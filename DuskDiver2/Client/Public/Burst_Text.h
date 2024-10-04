#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBurst_Text final : public CUI
{
enum BURSTMAX {B_LOW, B_HIGH, B_END};

private:
	CBurst_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBurst_Text(const CBurst_Text& rhs);
	virtual ~CBurst_Text() = default;

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
	_bool m_bHigh = true;

	_float m_fTextAcc = 0.f;
	_float m_fTextSpeed = 0.f;

	BURSTMAX	m_eBurstState;

public:
	static CBurst_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END