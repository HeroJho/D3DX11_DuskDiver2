#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBurst_Effect final : public CUI
{
private:
	CBurst_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBurst_Effect(const CBurst_Effect& rhs);
	virtual ~CBurst_Effect() = default;

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

	_uint m_iFrame = 0;
	_float m_fFrameAcc = 0.f;
	//_float2					m_fxyindex = { 4.f, 4.f };
	//_float					m_fxframe = 0.f;
	//_float					m_fyframe = 0.f;

public:
	static CBurst_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END