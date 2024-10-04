#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CInventoryUpside final : public CUI
{
private:
	CInventoryUpside(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventoryUpside(const CInventoryUpside& rhs);
	virtual ~CInventoryUpside() = default;

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

	class CMenuNumber1* m_pUINum = nullptr;
	class CMenuNumber2* m_pUINum2 = nullptr;
	class CMenuNumber3* m_pUINum3 = nullptr;


public:
	static CInventoryUpside* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END