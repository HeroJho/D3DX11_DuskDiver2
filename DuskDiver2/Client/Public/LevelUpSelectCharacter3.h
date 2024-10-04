#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CLevelUpSelectCharacter3 final : public CUI
{
private:
	CLevelUpSelectCharacter3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLevelUpSelectCharacter3(const CLevelUpSelectCharacter3& rhs);
	virtual ~CLevelUpSelectCharacter3() = default;

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
	_bool m_bAppear = false;
	_float m_fCut = 0.f;
	_uint m_iCurrentNum = 0;


	_bool m_bDD = false;
public:
	static CLevelUpSelectCharacter3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END