#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CStage2Event2 : public CGameObject
{
private:
	CStage2Event2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage2Event2(const CStage2Event2& rhs);
	virtual ~CStage2Event2() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool m_bSpawn = false;

	_bool m_bFirstCrack = false;

	_bool m_bOn = false;

protected:
	HRESULT Ready_Components();

	_float m_fTimeAcc = 0.f;
	_float m_fTimeDelay = 0.f;
	_float m_fTimeMax = 5.f;
	_int m_iBringMaxCount = 5;

	_uint test = 0;
	_float m_fRatio = 0.f;
	_float4 m_vInitLightness;
	_float4 m_vOtherSpec;
	_float m_fSpecRatio = 0.01f;

	_bool m_bTurnBack = false;

public:
	static CStage2Event2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END