#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CPlayer_Status04_HP final : public CUI
{
private:
	CPlayer_Status04_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Status04_HP(const CPlayer_Status04_HP& rhs);
	virtual ~CPlayer_Status04_HP() = default;

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
	_bool m_bOnce = false;
	_float m_fMaxHp = 0.f;
	_float m_fNowHp = 0.f;
	_float m_fHpGauge = 0.f;
	_float m_fMinusGauge = 0.f;
	_float m_fPreHp = 0.f;
	_float m_fPreHpGauge = 0.f;
	_float m_fOriginalSize = 0.f;
	_uint m_iOriginalHP = 0;
	_bool m_bPreUp = false;
	_bool m_bPreDown = false;

	_float m_fTimeAcc = 0.f;
	_bool m_bRemeber = false;
	_float m_fOriginfX = 0.f;
	_float m_fOriginfY = 0.f;

	class CPS04_STNumber* m_pUINum = nullptr;

public:
	static CPlayer_Status04_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END