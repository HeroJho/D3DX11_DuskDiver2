#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CMinimapStage1 final : public CUI
{
private:
	CMinimapStage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMinimapStage1(const CMinimapStage1& rhs);
	virtual ~CMinimapStage1() = default;

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
	CTexture*				m_pTextureCom2 = nullptr;

	_bool m_bDebug = false;
	_bool m_bRender = false;

public:
	static CMinimapStage1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END