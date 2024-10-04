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
class CSwordTrailObj;

class CBossBomb final : public CCreture
{
public:
	typedef struct BossBombInfo
	{
		_uint iIndex;
		_uint iAttackType;
		_float4 vPosition;
		_float4	vDir;
		_float	fJumpPower;
		_uint	iNaviNum;
		_float	fSpeed;
		_float	fGravity;
		_float4		vColor;
		_float4		vColor2;
		_float2		vRatio;
		_float		fBlurPower;
		_float4x4 vWorldMatrix;

	}BOMBINFO;

private:
	CBossBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossBomb(const CBossBomb& rhs);
	virtual ~CBossBomb() = default;


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
	void		Create_PointTrail(_float fTimeDelta);
	void		Create_SwordTrail(_float fTimeDelta);
	void		Create_ExplosionFloorEffect();
	void		Create_ExplosionAirEffect();
	void		Create_OtherBomb();
	void		DeadTime(_float fTimeDelta);
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CPointTrailObj*			m_PointTrails = nullptr;
	CSwordTrailObj*			m_SwordTrails = nullptr;

protected:
	HRESULT		Ready_Components();
	HRESULT		Create_Navigation(_uint iNaviNum);

private:
	_float4		m_vPlayerPos;
	_float3		m_vTargetLook;

	_float		m_fDeadAcc = 0.f;
	_bool		m_bOneCheck = false;
	_bool		m_bOneHit = true;
	_bool		m_bOne = true;
	_bool		m_bFix = true;
	_float		m_fSpeed = 0.f;
	BOMBINFO	m_BombInfo;
	_float      m_fCurJumpPower = 0.f;
public:
	static CBossBomb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END