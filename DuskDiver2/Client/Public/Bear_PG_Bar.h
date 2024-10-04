#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)
class CBear;

class CBear_PG_Bar final : public CUI
{
private:
	CBear_PG_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBear_PG_Bar(const CBear_PG_Bar& rhs);
	virtual ~CBear_PG_Bar() = default;

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
	CBear*			m_pBear = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;
	_bool m_bFirst = false;
	_float m_fMaxPG = 0.f;
	_float m_fNowPG = 0.f;
	_float m_fPatternGauge = 0.f;
	_float m_fPrePG = 0.f;
	_float m_fPrePatternGauge = 0.f;

public:
	static CBear_PG_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END