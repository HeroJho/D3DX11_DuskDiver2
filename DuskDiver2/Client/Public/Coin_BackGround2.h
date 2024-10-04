#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CCoin_BackGround2 final : public CUI
{
private:
	CCoin_BackGround2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCoin_BackGround2(const CCoin_BackGround2& rhs);
	virtual ~CCoin_BackGround2() = default;

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

	_bool m_bSizeUp = true;
	_float m_fSizeUpAcc = 3.f;

	_bool m_bSizeOnce = false;

public:
	static CCoin_BackGround2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END