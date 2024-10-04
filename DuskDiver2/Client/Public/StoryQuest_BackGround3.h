#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CStoryQuest_BackGround3 final : public CUI
{
private:
	CStoryQuest_BackGround3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStoryQuest_BackGround3(const CStoryQuest_BackGround3& rhs);
	virtual ~CStoryQuest_BackGround3() = default;

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
	_bool m_bOn = false;
	_bool m_bOff = false;
	_float m_fOnAcc = 0.f;

public:
	static CStoryQuest_BackGround3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END