#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CTowerRail : public CCreture
{
private:
	CTowerRail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTowerRail(const CTowerRail& rhs);
	virtual ~CTowerRail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

	virtual void Render_Debug() override;

private:
	void	Check_MonsterDeath();
	void	Spawn_Monster();


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	

protected:
	HRESULT Ready_Components();

private:
	CCreture::CREATEINFO	m_CretureInfo;

	_float Turn = 0.f;
	_uint m_iColor = 0;
	_float m_fBlurPower = 0.f;
	_float m_fY = 0.f;
	_uint m_iNum = 0;
	
	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;
	_float					m_fPosY = 0.f;
	_float					m_fRecoverAcc = 0.f;

	_bool					m_bSpawn = false;
	_bool					m_bHitAble = true;

	_bool					m_bResult = false; // 결과출력을 했는가?

	_bool					m_bBalling = false; // 결과출력을 했는가?
	_bool					m_bDeadSound = false;

public:
	static CTowerRail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END