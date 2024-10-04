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

class CBeastStonePart : public CGameObject
{
public:
	typedef struct tagStonePartDesc {
		_uint iStoneType; //모델 다르게 하기위함
		
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
	}PARTDESC;
	enum STONEPARTTYPE { TYPE_A, TYPE_B , TYPE_END };
private:
	CBeastStonePart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastStonePart(const CBeastStonePart& rhs);
	virtual ~CBeastStonePart() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void	Phx_Awake();
	void	Render_OnOff(_bool	bOnOff) { m_bRender = bOnOff; }
	void	Fall_Start() { m_bFall = true; }
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRigid*					m_pRigidCom = nullptr;
	CTexture* m_pDissolveTexture = nullptr;

	PARTDESC				m_PartDesc;
private:
	_bool					m_bRender = false;
	_bool					m_bFall = false;
	_float					m_fFallAcc = 0.f;
	_bool					m_bDissolve = false;
	_float					m_fDissolve = 0.f;
	_uint					m_iPathIndex = 0;
private:
	HRESULT Ready_Components();
	HRESULT Ready_ModelCom();

public:
	static CBeastStonePart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END