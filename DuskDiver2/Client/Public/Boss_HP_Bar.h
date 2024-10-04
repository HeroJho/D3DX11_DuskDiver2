#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)
class CCreture;

class CBoss_HP_Bar final : public CUI
{
private:
	CBoss_HP_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_HP_Bar(const CBoss_HP_Bar& rhs);
	virtual ~CBoss_HP_Bar() = default;

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
	class CBoss_HP_Number*  m_pUINum = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;
	_float m_fMaxHp = 0.f;
	_float m_fNowHp = 0.f;
	_float m_fPreHp = 0.f;
	_float m_fHpGauge = 0.f;
	_float m_fPreHpGauge = 0.f;

public:
	static CBoss_HP_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END