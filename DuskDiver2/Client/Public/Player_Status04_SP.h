#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CPlayer_Status04_SP final : public CUI
{
private:
	CPlayer_Status04_SP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Status04_SP(const CPlayer_Status04_SP& rhs);
	virtual ~CPlayer_Status04_SP() = default;

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
	_float m_fMaxSp = 0.f;
	_float m_fNowSp = 0.f;
	_float m_fSpGauge = 0.f;
	_float m_fPreSp = 0.f;
	_float m_fPreSpGauge = 0.f;

public:
	static CPlayer_Status04_SP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END