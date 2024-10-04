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

class CPlayer04MotionTrail : public CGameObject
{
public:
	enum HEADTYPE {HEADA, HEADB, HEADEND};
public:
	typedef struct tagPlayer04MotionTrailInfo
	{
		_float4x4 WorldMatrix;
		HEADTYPE eHeadType;
		_float4x4 HeadAMatrix;
		_float4x4 HeadBMatrix;
		vector<CHierarchyNode*>* Bodys;
		vector<CHierarchyNode*>* Heads;
		vector<CHierarchyNode*>* Heads2;
	}PLAYER04MOTIONTRAILINFO;
private:
	CPlayer04MotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer04MotionTrail(const CPlayer04MotionTrail& rhs);
	virtual ~CPlayer04MotionTrail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

public:
	void Set_OnTrail(PLAYER04MOTIONTRAILINFO Info);
	void Set_UpSpeed(_float fUpSpeed) { m_fUpSpeed = fUpSpeed; }
	void Set_DownSpeed(_float fDownSpeed) { m_fDownSpeed = fDownSpeed; }
	void Set_AlphaLimit(_float fAlphaLimit) { m_fAlphaLimit = fAlphaLimit; }
	void Set_BlurPower(_float fBlurPower) { m_fBlurPower = fBlurPower; }
	void Set_ShaderPass(_uint iShaderPass) { m_iShaderPass = iShaderPass; }

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
	_float4 m_vMotionTrailColor = { 0.5f,0.7f,1.f,1.f };
	HEADTYPE m_eHeadType;
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pHeadCom = nullptr;
	CModel*					m_pHeadCom2 = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pHeadTransform = nullptr;

public:
	static CPlayer04MotionTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


