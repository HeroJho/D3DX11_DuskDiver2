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

class CBossMissile final : public CCreture
{
public:
	typedef struct BossMissileInfo
	{
		_uint iHitType; //hit Ÿ��
		_uint iBulletType; //�Ѿ� Ÿ�� - ����, ������
		_uint iMissileIndex; //�̻��� ��ġ 0,1,2
		_float4 vPosition;
		_float4 vDir;
		_float m_fSide;
		_float4		vColor;
		_float4		vColor2;
		_float2		vRatio;
		_float		fBlurPower;
	}BOSSMISSILEINFO;

private:
	CBossMissile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossMissile(const CBossMissile& rhs);
	virtual ~CBossMissile() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;
	virtual void Create_HitEffect(_float4 vWorldPos)override;

private:
	HRESULT		Ready_Trails();

	void		DeadTime(_float fTimeDelta);
	
	void		ReadyToBezier1();
	void		ReadyToBezier2();
	void		ReadyToGuided();
	void		ReadyToParabolic();

	void		Create_Effect();

	void		ComputeBezier1();
	void		ComputeBezier2();

	void		Dead_Sound();

	void		Parabolic_Shoot(_float fTimeDelta);
	void		Bezier_Shoot(_float fTimeDelta);
	void		Guided_Shoot(_float fTimeDelta);//����ź : ���������� �Ѿƿ�

	void		LookAt(_fvector vAt);
	void		LookAtSmooth(_fvector StartLook, _fvector TargetPos, _float _fRatio);
	void		Change_Way(_float fTimeDelta);
	void		Change_AttackColRange(_float3 vSize, _float3 vCenter);
	
	void		Create_PointTrail(_float fTimeDelta);
	void		Create_ExplosionEffect();

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CPointTrailObj*			m_PointTrails = nullptr;
	CGameObject*			m_EffectRange = nullptr;

protected:
	HRESULT		Ready_Components();

private:
	BossMissileInfo		m_MissileInfo;
	_float4				m_vPlayerPos;
	_float3				m_vTargetLook;
	_float3				m_vMyLook;


	_float3				m_vTargetPos;
	_float3				m_vMovingPos[4];

	_float				m_fGoUp = 1.f;
	_float				m_fDeadAcc = 0.f;
	_float				m_fSpeed = 0.f;
	_float				m_fBezierTime = 0.1f;
	_float				m_fMagicNum = 0.f;
	_float				m_fMagicNum2 = 0.f;


	_float				m_fMagicNumY = 0.f;
	_float				m_fMagicNumX = 0.f;

	_float				m_fDistance = 0.f;

	_bool				m_bChange = false;
	_bool				m_bOneTime = true;
	_bool				m_bClose = false;
	_bool				m_bOnecheck = true;
	_bool				m_bFixColor = true;
	//������

	_float3				m_vStartPos = { 0.f, 0.f, 0.f }; // �����
	_float3				m_vEndPos = { 0.f, 0.f, 0.f }; //������
	_float3				m_vPos = { 0.f, 0.f, 0.f }; //���� ��ġ
	_float				m_fSpeedX = 0.f; //x������ �ӵ�
	_float				m_fSpeedY = 0.f; //y������ �ӵ�
	_float				m_fSpeedZ = 0.f;  // z������ �ӵ�
	_float				m_fg = 0.f; // Y�������� �߷°��ӵ� (9.8�ƴ� �ؿ��� ������)
	_float				m_fEndTime = 0.f;	// �������� ���� �ð�
	_float				m_fMaxHeight = 0.f;  // �ִ� ����
	_float				m_fHeight = 0.f;  //  �ִ������ Y - ���۳����� Y
	_float				m_fEndHeight = 0.f;  // �������� ���� Y - �������� ���� Y
	_float				m_fTime = 0.f;  // �帣�� �ð�.
	_float				m_fMaxTime = 0.8f; // �ִ���� ���� ���� �ð�. �����ش�.


public:
	static CBossMissile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END