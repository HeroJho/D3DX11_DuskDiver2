#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CCreture;

class COpenOX6 : public CGameObject
{
private:
	COpenOX6(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COpenOX6(const COpenOX6& rhs);
	virtual ~COpenOX6() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CCreture*				m_pMonster = nullptr;

protected:
	HRESULT Ready_Components();

private:
	_float4 m_vMonsterPos;
	_bool m_bRender = false;

	_float m_fSizeX, m_fSizeY = 0.f;

	_float m_fAlpha = 0.f;

	_bool m_bO = false;
	_float m_fO = 0.f;

	_float m_fAlphaTime = 0.f;
	_bool m_bNear = false;
	_bool m_bOnce = false;
	_float4 m_fPlayerPos = { 0.f, 0.f, 0.f, 0.f };

	_bool m_bTriggerCheck = false;
public:
	static COpenOX6* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END