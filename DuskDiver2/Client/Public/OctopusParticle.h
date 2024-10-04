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
		_uint iModelType; //�� �ٸ��� �ϱ�����
		
		//rigid ���� �ʿ�, ������ ������ �� �Ȱ�����
		_float3 vStartPos; //���� ����
		_float3 vPos;
		_float3 vRotation; //�� ȸ������
		_float3	vExtense; //�ݶ��̴� ũ��
		_float	fWeight;

		_float fSize;
		_float fAngle;
		_float4 vLocalPosition; //��ü�� ���� startpos�� ���������
		_float	vLocalRot; //��ü�� ���� startpos�� ���������

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
	void Add_Torque(); //ȸ��

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