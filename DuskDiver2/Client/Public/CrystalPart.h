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

class CCrystalPart : public CGameObject
{
public:
	typedef struct tagCrystalPartDesc {
		_uint iCrystalType;
		_float4 vLocalPosition; // 영혼의 위치가 0일때 상대위치
		_float fSize; // 크기
		_float fAngle; // 회전값
	}PARTDESC;
	enum CRYSTALTYPE { TYPE_A, TYPE_B , TYPE_END };
private:
	CCrystalPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrystalPart(const CCrystalPart& rhs);
	virtual ~CCrystalPart() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	void Phx_Awake();
	void Add_Force(_float3 vForce);
	void Change_DissolveAcc(_float fChanges) { m_fDissolveAcc = fChanges; }

#pragma region Debug & ImGui
	void Change_Position(_float4 vPos);
	void Change_Rotation(_float fAngle);
	void Change_Size(_float fSize);
	_float4 Get_Position();
	_float Get_RotationAngle();
	_float Get_Size();
#pragma endregion Debug & ImGui

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRigid*					m_pRigidCom = nullptr;

	PARTDESC				m_PartDesc;
	_float					m_fDissolveAcc = 0.f;

private:
	HRESULT Ready_Components();
	void Set_InitState();

public:
	static CCrystalPart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END