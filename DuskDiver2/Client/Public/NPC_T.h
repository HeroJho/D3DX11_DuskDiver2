#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CNPC_T : public CGameObject
{
public:
	typedef struct tagNPCIdleDesc {
		
		_float3 vPos;
		_float3 vRot;
		_float3 vScale;

		_tchar sModelName[MAX_PATH];
		_uint iAnimIndex;
		// _float4 vNoneAnimColor;

	}NPCIDLEDESC;

private:
	CNPC_T(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_T(const CNPC_T& rhs);
	virtual ~CNPC_T() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual HRESULT Render_ShadowDepth();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


public:
	NPCIDLEDESC Get_NPCDesc() { return m_Desc; }


private:
	void Set_DelayAnim(_bool bAnimDelay, _float fTime = 0.f, _float fDelayTime = 0.f);
	void Tick_DelayAnim(_float fTimeDelta);


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;


private:
	_bool m_bAnimDelay = false;
	_float m_fAnimDelayTimeAcc = 0.f;
	_float m_fAnimDelayTime = 0.f;
	_float m_fDelayTime = 0.f;

	_bool m_bStopAnim = false;


	NPCIDLEDESC m_Desc;




private:
	HRESULT Ready_Components();


public:
	static CNPC_T* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END