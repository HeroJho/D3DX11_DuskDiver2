#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CPlayerNum1 final : public CUI
{
private:
	CPlayerNum1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerNum1(const CPlayerNum1& rhs);
	virtual ~CPlayerNum1() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	HRESULT Ready_Components();

	virtual void Handle_Hover() override;
	virtual void Handle_Down() override;
	virtual void Handle_Press() override;
	virtual void Handle_Click() override;


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool m_bDebug = false;

public:
	static CPlayerNum1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END