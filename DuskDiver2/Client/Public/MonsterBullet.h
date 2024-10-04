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

class CMonsterBullet final : public CCreture
{
public:
	typedef struct MonsterBulletInfo
	{
		_uint iHitType;
		_uint iBulletType;
		_float4 vPosition;
		_float4	vDir;
		_float4		vColor;
		_float4		vColor2;
		_float2		vRatio;
		_float		fBlurPower;
		_bool		bFixColor = true;
	}BULLETINFO;

private:
	CMonsterBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterBullet(const CMonsterBullet& rhs);
	virtual ~CMonsterBullet() = default;


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
	void		Create_Particle(_float fTimeDelta);

	void		DeadTime(_float fTimeDelta);
	
	void		ReadyToDirect();
	void		ReadyToDiagonal();
	void		ReadyToParabolic();

	void		Direct_Shoot(_float fTimeDelta);
	void		Parabolic_Shoot(_float fTimeDelta);

	void		Create_PointTrail(_float fTimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CPointTrailObj*			m_PointTrails = nullptr;

protected:
	HRESULT		Ready_Components();

private:
	BULLETINFO	m_BulletInfo;
	_float4		m_vPlayerPos;
	_float3		m_vTargetLook;

	_uint		m_iBulletType = 0;
	_float		m_fDeadAcc = 0.f;

	_float		m_fSpeed = 0.f;


	_float3		m_vStartPos = { 0.f, 0.f, 0.f }; // �����
	_float3		m_vEndPos = { 0.f, 0.f, 0.f }; //������
	_float3		m_vPos = { 0.f, 0.f, 0.f }; //���� ��ġ
	_float		m_fSpeedX = 0.f; //x������ �ӵ�
	_float		m_fSpeedY = 0.f; //y������ �ӵ�
	_float		m_fSpeedZ = 0.f;  // z������ �ӵ�
	_float		m_fg = 0.f; // Y�������� �߷°��ӵ� (9.8�ƴ� �ؿ��� ������)
	_float		m_fEndTime = 0.f;	// �������� ���� �ð�
	_float		m_fMaxHeight = 0.f;  // �ִ� ����
	_float		m_fHeight = 0.f;  //  �ִ������ Y - ���۳����� Y
	_float		m_fEndHeight = 0.f;  // �������� ���� Y - �������� ���� Y
	_float		m_fTime = 0.f;  // �帣�� �ð�.
	_float		m_fMaxTime = 0.8f; // �ִ���� ���� ���� �ð�. �����ش�. s

	_float		m_fParticleAcc = 0.f;
	_bool		m_bParticle = true;

	_float      m_fDeadY = 0.f;

public:
	static CMonsterBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END