#pragma once

#include "Client_Defines.h"
#include "Parts.h"
#include "DebugDraw.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer01HairC : public CParts
{
private:
	CPlayer01HairC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer01HairC(const CPlayer01HairC& rhs);
	virtual ~CPlayer01HairC() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	void Render_Vtx();

public:
	PxRigidDynamic* Find_Rigid(const char* pName, CHierarchyNode** Out_pHeadBone = nullptr);

	void SetInitPos();
	void StagMass();
	void SkillMass();

	void Set_Sleep();
	void Set_Wake();

	CModel* Get_ModelCom() { return m_pModelCom; }
	_float4x4 Get_ConbineMatrix();
	void Set_PlayerLight(_bool bPlayerLight) { m_bPlayerLight = bPlayerLight; }
	void Set_PlayerLightColor(_float3 vColor) { m_vPlayerLightColor = vColor; }

private:
	_float3					m_vPlayerLightColor = { 1.f,1.f,1.f };
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	_bool					m_bPlayerLight = false;
private:
	vector<PxRigidDynamic*>	m_pRigid;
	vector<CHierarchyNode*>	m_pBones;
	PxRigidDynamic* m_pHeadRigid = nullptr;
	CHierarchyNode* m_pHeadBone = nullptr;
	_float4x4 m_HeadInitComM;
	PxJoint* m_pJoint = nullptr;

	_uint test = 0;

	_float3 Test;

	_float4x4 m_BoneWorld;

private:
	HRESULT Ready_Components();
	HRESULT	Ready_JointHair();



public:
	static CPlayer01HairC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;



#ifdef _DEBUG
private:
	void Render_HairCol();

private:
	vector<BoundingOrientedBox*>	m_pOBBs;
	vector<BoundingOrientedBox*>	m_pOriginal_OBBs;

	PrimitiveBatch<VertexPositionColor>*		m_pBatch = nullptr;
	BasicEffect*								m_pEffect = nullptr;
	ID3D11InputLayout*							m_pInputLayout = nullptr;
#endif


};

END