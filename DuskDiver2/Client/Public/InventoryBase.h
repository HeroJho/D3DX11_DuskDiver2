#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CInventoryBase final : public CUI
{
private:
	CInventoryBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventoryBase(const CInventoryBase& rhs);
	virtual ~CInventoryBase() = default;

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
	class CNumExp* m_pUINum = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

	_bool m_bSelect = false;
	_uint m_iNum = 0;

	_uint m_iInveType = 0;

	_uint m_iInvenSelNum = 0;

	_uint m_iBattleNum = 0;

	_float m_fBattlTime = 0;
	_float m_fEatTime = 0;

	_bool m_bBattle = false;

	_bool m_bEatPage = false;
	_float m_fHungryTime = false;

	// 배틀아이템 내릴때 안보이게 하는거
	_bool m_bBattleSelect= false;
public:
	static CInventoryBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END