#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CPartyOn_Contents final : public CUI
{
private:
	CPartyOn_Contents(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartyOn_Contents(const CPartyOn_Contents& rhs);
	virtual ~CPartyOn_Contents() = default;

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
	_uint m_iTextNum = 0;
	_bool m_bOnce = false;
	_bool m_bOnce2 = false;
	_bool m_bOn = false;
	_bool m_bCheck = true;
	_float m_fPop = 0.f;

public:
	static CPartyOn_Contents* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END