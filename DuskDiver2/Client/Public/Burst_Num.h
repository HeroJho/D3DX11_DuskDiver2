#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBurst_Num final : public CUI
{
enum BURSTTYPE {B_120, B_140, B_160, B_180, B_END};

private:
	CBurst_Num(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBurst_Num(const CBurst_Num& rhs);
	virtual ~CBurst_Num() = default;

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
	_bool m_bBpMax = false;
	BURSTTYPE	m_eBurstState;

public:
	static CBurst_Num* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END