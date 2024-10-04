#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CModel;
class CShader;
class CHierarchyNode;
class CRenderer;
class CTransform;
END

class CPlayer01MotionTrail : public CGameObject
{
public:
	enum HEADTYPE {HEADA, HEADB, HEADC, HEADEND};
	enum BODYTYPE {BODYA, BODYB, BODYEND};
public:
	typedef struct tagPlayer01MotionTrailInfo
	{
		_float4x4 WorldMatrix;
		HEADTYPE eHeadType;
		BODYTYPE eBodyType;
		_float4x4 HeadAMatrix;
		_float4x4 HeadBMatrix;
		_float4x4 HeadCMatrix;
		vector<CHierarchyNode*>* Bodys;
		vector<CHierarchyNode*>* Bodys2;
		vector<CHierarchyNode*>* Heads;
		vector<CHierarchyNode*>* Heads2;
		vector<CHierarchyNode*>* Heads3;
	}PLAYER01MOTIONTRAILINFO;
private:
	CPlayer01MotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer01MotionTrail(const CPlayer01MotionTrail& rhs);
	virtual ~CPlayer01MotionTrail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

public:
	void Set_OnTrail(PLAYER01MOTIONTRAILINFO Info);
	void Set_UpSpeed(_float fUpSpeed) { m_fUpSpeed = fUpSpeed; }
	void Set_DownSpeed(_float fDownSpeed) { m_fDownSpeed = fDownSpeed; }
	void Set_AlphaLimit(_float fAlphaLimit) { m_fAlphaLimit = fAlphaLimit; }
	void Set_BlurPower(_float fBlurPower) { m_fBlurPower = fBlurPower; }
	void Set_ShaderPass(_uint iShaderPass) { m_iShaderPass = iShaderPass; }
	void Set_HairRender(_bool bRender) { m_bHairRender = bRender; }
private:
	HRESULT Ready_Components();

	
private:
	_bool m_bOn = false;
	_bool m_bUp = false;
	_uint m_iShaderPass = 8;
	_float m_fAlpha = 0.f;
	_float m_fAlphaLimit = 0.1f;
	_float m_fUpSpeed = 1.f;
	_float m_fDownSpeed = 1.f;
	_float m_fRimPower = 1.f;
	_float m_fBlurPower = 0.3f;
	_float4 m_fRimColor = { 0.99f,0.f,0.f,1.f };
	_float4 m_vMotionTrailColor = { 1.f,0.9f,0.5f,1.f };
	_bool m_bHairRender = true;
	HEADTYPE m_eHeadType;
	BODYTYPE m_eBodyType;
	_float4x4 m_NonAnimMatrix;
private:
	CShader*				m_pShaderCom = nullptr;
	CShader*				m_pNonAnimShader = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pModelCom2 = nullptr;
	CModel*					m_pHeadCom = nullptr;
	CModel*					m_pHeadCom2 = nullptr;
	CModel*					m_pHeadCom3 = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pHeadTransform = nullptr;

public:
	static CPlayer01MotionTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


