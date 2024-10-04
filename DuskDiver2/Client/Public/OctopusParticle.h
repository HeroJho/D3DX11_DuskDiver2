#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CRigid;
END

BEGIN(Client)

class COctopusParticle : public CGameObject
{
public:
	typedef struct tagMechanicalPartDesc {
		_uint iModelType; //모델 다르게 하기위함
		
		//rigid 정보 필요, 나머지 정보는 다 똑같이함
		_float3 vStartPos; //몸통 포스
		_float3 vPos;
		_float3 vRotation; //모델 회전각도
		_float3	vExtense; //콜라이더 크기
		_float	fWeight;

		_float fSize;
		_float fAngle;
		_float4 vLocalPosition; //본체의 포스 startpos에 더해줘야함
		_float	vLocalRot; //본체의 포스 startpos에 더해줘야함

		_float3 vFlyPower;
	}PARTDESC;

private:
	COctopusParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COctopusParticle(const COctopusParticle& rhs);
	virtual ~COctopusParticle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	void Add_Force(_float3 vForce);
	void Add_Torque(); //회전

	void Tick_Cuma(_float fTimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRigid*					m_pRigidCom = nullptr;

	PARTDESC				m_PartDesc;
private:
	_bool					m_bRender = false;
	_bool					m_bFall = false;
	_float					m_fFallAcc = 0.f;
	_bool					m_bAddForce = false;
	_bool					m_bOneForce = false;


	_float				m_fTimeAcc = 0.f;

private:
	HRESULT Ready_Components();
	HRESULT Ready_ModelCom();

public:
	static COctopusParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END