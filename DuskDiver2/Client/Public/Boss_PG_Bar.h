#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)
class CCreture;

class CBoss_PG_Bar final : public CUI
{
private:
	CBoss_PG_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_PG_Bar(const CBoss_PG_Bar& rhs);
	virtual ~CBoss_PG_Bar() = default;

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
	CCreture*				m_pMonster = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;
	_bool m_bFirst = false;
	_float m_fMaxPG = 0.f;
	_float m_fNowPG = 0.f;
	_float m_fPatternGauge = 0.f;
	_float m_fPrePG = 0.f;
	_float m_fPrePatternGauge = 0.f;

public:
	static CBoss_PG_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END