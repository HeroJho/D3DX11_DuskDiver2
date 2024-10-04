#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CItemUseEffect3 final : public CUI
{
private:
	CItemUseEffect3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemUseEffect3(const CItemUseEffect3& rhs);
	virtual ~CItemUseEffect3() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	HRESULT Ready_Components();
	void Use_Sound();
	void Cancel_Sound();


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool m_bDebug = false;

	_float m_fTime = 1.29f;

	_bool m_bRender = false;

	_bool m_bRed = false;

	_bool m_bSoundOn = false;

public:
	static CItemUseEffect3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END