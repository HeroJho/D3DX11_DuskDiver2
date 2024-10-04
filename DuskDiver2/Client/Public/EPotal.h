#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)


class CEPotal final : public CCreture
{
private:
	CEPotal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEPotal(const CEPotal& rhs);
	virtual ~CEPotal() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;


public:
	void Set_CutScene() { m_bCutScene = true; }
	_bool Get_CutScene() { return m_bCutScene; }


private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();



private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;


private:
	_bool m_bCutScene = false;
	_float m_fTimeAcc = 0.f;
	_bool m_bTrigger = false;
	_bool m_bPotal = false;
	_bool m_bCanUse = false;
	_float m_fParticleAcc = 0, f;
	_float m_fAlpha = 0.f;
	_bool m_bOffSound = false;
	_bool m_bCutSceneSound = false;

public:
	static CEPotal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END