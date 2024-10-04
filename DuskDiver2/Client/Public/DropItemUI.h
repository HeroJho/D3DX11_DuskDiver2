#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CDropItemUI final : public CUI
{
private:
	CDropItemUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDropItemUI(const CDropItemUI& rhs);
	virtual ~CDropItemUI() = default;

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
	class CNumWaper* m_pUINum = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

	UIINFODESC m_iIndex;
	_int m_iIndexNum = 0;

	_uint m_iItemNum = 0;
	_float m_fItemNumTime = 0.f;
	_float m_fItemNumTime2 = 0.f;

	_bool m_bVainish = false;
	_bool m_bOn = false;
	_bool m_bVanishOn = false;
	_float m_fVanishTime = 0.f;

	_float m_fTime = 0.f;

	_float m_fTime2 = 0.f;

	_uint m_iOriginNum = 0;

	_float m_fAlphaTime = 0.f;
	_float m_fOriginPosx = 0.f;

	_uint m_iRandom = 0;
public:
	static CDropItemUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END