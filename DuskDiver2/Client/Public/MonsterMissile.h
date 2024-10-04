#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)
class CPointTrailObj;

class CMonsterMissile final : public CCreture
{
public:
	typedef struct MonsterMissileInfo
	{
		_uint iHitType; //hit 타입
		_uint iBulletType; //총알 타입 - 직선, 베지어
		_uint iMissileIndex; //미사일 위치 0,1,2
		_float4 vPosition;
		_float4 vDir;
		_float m_fSide;
	}MISSILEINFO;

private:
	CMonsterMissile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterMissile(const CMonsterMissile& rhs);
	virtual ~CMonsterMissile() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void Create_HitEffect(_float4 vWorldPos)override;
private:
	HRESULT		Ready_Trails();

	void		DeadTime(_float fTimeDelta);
	
	void		ReadyToDirect();
	void		ReadyToBezier1();


	void		ComputeBezier1();
	void		ComputeBezier2();


	void		Direct_Shoot(_float fTimeDelta);
	void		Bezier_Shoot(_float fTimeDelta);

	void		LookAt(_fvector vAt);

	void		Create_PointTrail(_float fTimeDelta);
	void		Create_ExplosionEffect();
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CPointTrailObj*			m_PointTrails = nullptr;
	

protected:
	HRESULT		Ready_Components();

private:
	_float4		m_vPlayerPos;
	_float4		m_vPosistion;
	_float4		m_vLook;
	_float3		m_vTargetLook;

	_float3		m_vTargetPos;
	_float3		m_vMovingPos[4];
	_uint		m_iMissileIndex = 0;
	_uint		m_iBulletType = 0;
	_float		m_fDeadAcc = 0.f;
	_float		m_fSpeed = 0.f;
	_float		m_fBezierTime = 0.1f;
	_float		m_fSide = 0.f;

	_bool		m_bFixColor = true;
	_bool		m_bOnecheck = true;
	_float      m_fDeadY = 0.f;

	_float4		vColor;
	_float4		vColor2;
	_float2		vRatio;
	_float		fBlurPower;
public:
	static CMonsterMissile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END