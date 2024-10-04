#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CQuestDist final : public CUI
{
private:
	CQuestDist(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuestDist(const CQuestDist& rhs);
	virtual ~CQuestDist() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	void Set_DistPos(_float2 vPos, _uint iNumber);
	void Set_DistPosRender(_bool b) { m_bRender = b; }

private:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool					m_bDebug = false;
	_bool					m_bRender = false;

	_uint					m_iNumIndex = 0;
	_float2					m_DistPos = { 0.f,0.f };

public:
	static CQuestDist* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END