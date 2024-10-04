#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CStatusBase final : public CUI
{
private:
	CStatusBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatusBase(const CStatusBase& rhs);
	virtual ~CStatusBase() = default;

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
	class CNumStamina* m_pNumStamina = nullptr;
	class CNumExplosive* m_pNumExplosive = nullptr;
	class CNumLuck* m_pNumLuck = nullptr;

	_bool m_bDebug = false;

	_bool m_bRender = false;
	_bool m_bSelect = false;

	_uint m_iSelect = 0;

	_uint m_iStatusNum = 1;

	_uint m_iPlayerNum = 1;

	_uint m_iPlayer1Str = 1;
	_uint m_iPlayer1Stamina = 1;
	_uint m_iPlayer1Hp = 1;
	_uint m_iPlayer1Mental = 1;
	_uint m_iPlayer1Meta = 1;
	_uint m_iPlayer1Explosive = 1;
	_uint m_iPlayer1Luck = 1;

	_uint m_iPlayer2Str = 1;
	_uint m_iPlayer2Stamina = 1;
	_uint m_iPlayer2Hp = 1;
	_uint m_iPlayer2Mental = 1;
	_uint m_iPlayer2Meta = 1;
	_uint m_iPlayer2Explosive = 1;
	_uint m_iPlayer2Luck = 1;

	_uint m_iPlayer3Str = 1;
	_uint m_iPlayer3Stamina = 1;
	_uint m_iPlayer3Hp = 1;
	_uint m_iPlayer3Mental = 1;
	_uint m_iPlayer3Meta = 1;
	_uint m_iPlayer3Explosive = 1;
	_uint m_iPlayer3Luck = 1;

	_uint m_iUp = 0;


public:
	static CStatusBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END