#pragma once

#include "Client_Defines.h"
#include "Parts.h"
#include "DebugDraw.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer03HairB : public CParts
{
private:
	CPlayer03HairB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer03HairB(const CPlayer03HairB& rhs);
	virtual ~CPlayer03HairB() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	CModel* Get_ModelCom() { return m_pModelCom; }
	_float4x4 Get_ConbineMatrix();
	void Set_PlayerLight(_bool bPlayerLight) { m_bPlayerLight = bPlayerLight; }
	void Set_PlayerLightColor(_float3 vColor) { m_vPlayerLightColor = vColor; }
	
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	_float3					m_vPlayerLightColor = { 1.f,1.f,1.f };
	_bool					m_bPlayerLight = false;
private:
	HRESULT Ready_Components();
	_float AX = 0.f;
	_float AY = 0.f;
	_float AZ = 0.f;

public:
	static CPlayer03HairB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END