#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CEP_Battle final : public CUI
{
private:
	CEP_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEP_Battle(const CEP_Battle& rhs);
	virtual ~CEP_Battle() = default;

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

	_float m_fFrameAcc = 0.f;
	_bool m_bDebug = false;
	_uint m_iFrame = 0;

public:
	static CEP_Battle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END