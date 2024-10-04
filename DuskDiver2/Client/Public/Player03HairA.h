#pragma once

#include "Client_Defines.h"
#include "Parts.h"
#include "DebugDraw.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer03HairA : public CParts
{
private:
	CPlayer03HairA(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer03HairA(const CPlayer03HairA& rhs);
	virtual ~CPlayer03HairA() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	void Set_PlayerLight(_bool bPlayerLight) { m_bPlayerLight = bPlayerLight; }
	void Set_PlayerLightColor(_float3 vColor) { m_vPlayerLightColor = vColor; }
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

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	_float3					m_vPlayerLightColor = { 1.f,1.f,1.f };
	_bool					m_bPlayerLight = false;
private:
	vector<PxRigidDynamic*>	m_pRigid;
	vector<CHierarchyNode*>	m_pBones;

	PxRigidDynamic* m_pHeadRigid_L = nullptr;
	CHierarchyNode* m_pHeadBone_L = nullptr;
	PxRigidDynamic* m_pHeadRigid_R = nullptr;
	CHierarchyNode* m_pHeadBone_R = nullptr;
	_float4x4 m_HeadInitComM_L;
	_float4x4 m_HeadInitComM_R;

	PxJoint* m_pJoint = nullptr;

	_uint test = 0;

	_float3 Test;
	_bool m_bRedner = true;
	_bool m_bColRedner = false;
	_float m_fBlurPower = 0.7f;
	_float4x4 m_BoneWorld;


private:
	HRESULT Ready_Components();
	HRESULT	Ready_JointHair();



public:
	static CPlayer03HairA* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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