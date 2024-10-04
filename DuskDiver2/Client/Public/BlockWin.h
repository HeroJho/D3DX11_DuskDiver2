#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CBlockWin final : public CUI
{
private:
	CBlockWin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlockWin(const CBlockWin& rhs);
	virtual ~CBlockWin() = default;

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
	_float m_fAlpha = 1.f;
	_bool m_bTriger = false;


public:
	static CBlockWin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END