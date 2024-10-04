#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)
class CBear;

class CBear_HP_Bar final : public CUI
{
private:
	CBear_HP_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBear_HP_Bar(const CBear_HP_Bar& rhs);
	virtual ~CBear_HP_Bar() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;



public:
	void Set_HP(_float fNowHp) { m_fNowHp = fNowHp; }
	_float Get_HP() { return m_fNowHp; }

	void Deal_Hp(_float fAt);
	void Deal_PunchHp(_float fAt);
	void Deal_HakiHp(_float fAt);

	void Recorve_HpAcc();
	void Reset_HpAcc();


private:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	class CBoss_HP_Number*  m_pUINum = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;
	_float m_fMaxHp = 0.f;
	_float m_fNowHp = 0.f;
	_float m_fPreHp = 0.f;
	_float m_fHpGauge = 0.f;
	_float m_fPreHpGauge = 0.f;

	_float m_fHPAcc = 0.f;

	_float m_fHakiHp = 0.f;
	_bool	m_bOneDeath = false;
public:
	static CBear_HP_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END