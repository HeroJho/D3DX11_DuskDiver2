#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CMiniQuest final : public CUI
{
private:
	CMiniQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMiniQuest(const CMiniQuest& rhs);
	virtual ~CMiniQuest() = default;

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

	_float m_fTime = 0.f;
	_float m_fTime2 = 0.f;
	_bool m_bIdle = false;

	_float m_fX = 0.f;
	_float m_fY = 0.f;
public:
	static CMiniQuest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END