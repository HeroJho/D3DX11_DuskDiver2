#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CPlayer_Status01_BP final : public CUI
{
private:
	CPlayer_Status01_BP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Status01_BP(const CPlayer_Status01_BP& rhs);
	virtual ~CPlayer_Status01_BP() = default;

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
	_float m_fMaxEp = 0.f;
	_float m_fNowEp = 0.f;
	_float m_fEpGauge = 0.f;
	_float m_fPreEp = 0.f;
	_float m_fPreEpGauge = 0.f;

public:
	static CPlayer_Status01_BP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END