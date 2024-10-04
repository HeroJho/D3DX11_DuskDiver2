#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CHP final : public CUI
{
private:
	CHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHP(const CHP& rhs);
	virtual ~CHP() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

public:
	void Set_Pos(CPlayer::SelectPlayer eSelectPlayer);

private:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool m_bDebug = false;

public:
	static CHP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END