#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CMiniMinimap final : public CUI
{
private:
	CMiniMinimap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMiniMinimap(const CMiniMinimap& rhs);
	virtual ~CMiniMinimap() = default;

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

	_float m_fPosx = 0.f;
	_float m_fPosy = 0.f;

	_int   m_iMapNum = 0;
public:
	static CMiniMinimap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END