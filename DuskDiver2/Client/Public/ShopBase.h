#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CShopBase final : public CUI
{
private:
	CShopBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShopBase(const CShopBase& rhs);
	virtual ~CShopBase() = default;

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
	class CNumReserveExp* m_pNumReserveExp = nullptr;
	class CNumLevel* m_pNumLevel = nullptr;
	class CNumHp* m_pNumHp = nullptr;
	class CNumMental* m_pNumMental = nullptr;
	class CNumMeta* m_pNumMeta = nullptr;
	class CNumStr* m_pNumStr = nullptr;
	class CNumWeapon* m_pNumWeapon = nullptr;
	class CNumWeapon1* m_pNumWeapon1 = nullptr;
	class CNumWeapon2* m_pNumWeapon2 = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

	_uint m_iSelectItem = 0;
	_uint m_iCountItem = 0;
	_bool m_bSelectPage = false;
	_bool m_bSelectPage1 = false;
	_bool m_bSelectPage2 = false;
	_bool m_bBuyPage = false;


	//페이지 1의 아이템 선택 상황
	_uint m_iPage1 = 0;


	_uint m_iShopSelect = 0;

	_bool m_bSelect = false;

	_bool m_bUpgrade = false;

public:
	static CShopBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END