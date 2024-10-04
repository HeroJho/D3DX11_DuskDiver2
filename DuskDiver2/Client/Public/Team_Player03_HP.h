#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CTeam_Player03_HP final : public CUI
{
private:
	CTeam_Player03_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTeam_Player03_HP(const CTeam_Player03_HP& rhs);
	virtual ~CTeam_Player03_HP() = default;

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
	_float m_fHPGauge = 0.f;

	class CPS01_STNumber* m_pUINum = nullptr;

public:
	static CTeam_Player03_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END