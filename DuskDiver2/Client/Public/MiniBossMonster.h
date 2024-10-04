#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;

END

BEGIN(Client)
class CCreture; 

class CMiniBossMonster : public CUI
{
private:
	CMiniBossMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMiniBossMonster(const CMiniBossMonster& rhs);
	virtual ~CMiniBossMonster() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CCreture*				m_pMonster = nullptr;

protected:
	HRESULT Ready_Components();

private:
	_float4 m_vMonsterPos;
	_bool m_bDebug = false;
	_bool m_bRender = false;

	_float m_fSizeX, m_fSizeY = 0.f;
	_float m_fMaxHp = 0.f;
	_float m_fNowHp = 0.f;
	_float m_fHpGauge = 0.f;


	_float m_fTime = 0.f;
	_float m_fTime2 = 0.f;
	_bool m_bIdle = false;

	_float m_fX = 0.f;
	_float m_fY = 0.f;

public:
	static CMiniBossMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END