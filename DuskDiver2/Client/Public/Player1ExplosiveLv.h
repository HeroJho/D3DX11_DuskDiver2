#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CPlayer1ExplosiveLv final : public CUI
{
private:
	CPlayer1ExplosiveLv(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer1ExplosiveLv(const CPlayer1ExplosiveLv& rhs);
	virtual ~CPlayer1ExplosiveLv() = default;

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
	_uint m_iStat = 0;

	_bool m_bSelect = false;
public:
	static CPlayer1ExplosiveLv* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END