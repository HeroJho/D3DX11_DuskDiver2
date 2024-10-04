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

class CMon_HP_Bar : public CGameObject
{
private:
	CMon_HP_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMon_HP_Bar(const CMon_HP_Bar& rhs);
	virtual ~CMon_HP_Bar() = default;


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
	HRESULT Billboard();

private:
	_float4 m_vMonsterPos;
	_bool m_bRender = false;
	_bool m_bBoss = false;
	_float m_fSizeX, m_fSizeY = 0.f;
	_float m_fMaxHp = 0.f;
	_float m_fNowHp = 0.f;
	_float m_fPreHp = 0.f;
	_float m_fHpGauge = 0.f;
	_float m_fPreHpGauge = 0.f;
	_float	m_fY, m_fZ = 0.f;
public:
	static CMon_HP_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END