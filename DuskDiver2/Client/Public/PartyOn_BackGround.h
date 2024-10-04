#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CPartyOn_BackGround final : public CUI
{
private:
	CPartyOn_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartyOn_BackGround(const CPartyOn_BackGround& rhs);
	virtual ~CPartyOn_BackGround() = default;

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
	_bool m_bRender = false;
	_bool m_bOnce = false;
	_bool m_bOnce2 = false;
	_bool m_bCheck = true;
	_bool m_bSoundOn = false;

public:
	static CPartyOn_BackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END