#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CEquipmentSelectColor final : public CUI
{
private:
	CEquipmentSelectColor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEquipmentSelectColor(const CEquipmentSelectColor& rhs);
	virtual ~CEquipmentSelectColor() = default;

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
	_bool m_bRender = true;

	_uint m_iCurNum = 1;

	_float m_fCountRender = 0.f;
	_float m_fBlend = 0.f;

	_float m_fYPos = 0.f;

	_float m_fGreen = 0.5f;
	_float m_fSelectTime = 0.f;
	_bool m_bRed = false;
public:
	static CEquipmentSelectColor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END