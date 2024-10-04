#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CMenuBackGround final : public CUI
{
private:
	CMenuBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMenuBackGround(const CMenuBackGround& rhs);
	virtual ~CMenuBackGround() = default;

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

	_uint MenuNum = 1;

	_bool m_bSelect = false;


public:
	static CMenuBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END