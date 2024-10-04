#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CStreetVendor1 final : public CUI
{
private:
	CStreetVendor1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStreetVendor1(const CStreetVendor1& rhs);
	virtual ~CStreetVendor1() = default;

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
	class CNumMental* m_pNumMental = nullptr;
	class CNumMeta* m_pNumMeta = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

	_uint m_iSelectItem = 0;
	_uint m_iCountItem = 0;
	_bool m_bSelectPage = false;
	_bool m_bBuyPage = false;

public:
	static CStreetVendor1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END