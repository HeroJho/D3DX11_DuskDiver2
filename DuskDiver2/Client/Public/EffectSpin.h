#pragma once
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CEffectSpin final : public CGameObject
{
public:
	typedef struct tagEffectSpinDesc {
		_uint   iSpinType;// ���� � ���� ��������
		_float  fSpinAngleSpeed;// ȸ�� ����(+-�� �ð����,�ݽð�������, ũ��� �ӵ�ũ�����)
		_float  fSpinSizeSpeed;// ����� Ű������� ���ϰ����� +-�� ����
		_float  fSpinStartSize; // ù ������ ������
		_float	fSpinStartAlpha; // ù ������ alpha 
		_uint	iPassIndex; // shader���� ��� �н��� �����ΰ�
		_uint	iPlayerType; // ĳ���� ���� : ����, �����̴�, ���
	}EFFECTSPINDESC;
	enum PLAYERTYPE { PLAYERTYPE_YUMO, PLAYERTYPE_LI, PLAYERTYPE_DD, PLAYERTYPE_END };
	enum SPINTYPE { TYPE_RING, TYPE_LENS, TYPE_SMOKEBALL, TYPE_RINGWIND, TYPE_LINEBALL, TYPE_GROUNDWIND, TYPE_GROUNDBALL, TYPE_DoubleKick, TYPE_BlastBall,
		TYPE_LANDING, TYPE_KICKAURA, TYPE_RINGWIND2, TYPE_METAMORPHOSIS, TYPE_ItemGround, TYPE_ItemGroundClue,  TYPE_END };
private:
	CEffectSpin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CEffectSpin(const CEffectSpin& rhs);
	virtual ~CEffectSpin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_EffectPos(_float4 vPos);
	void Set_UpdateSize(_float fSize);

private:
	HRESULT Ready_Components();
	void Set_InitializeDesc(SPINTYPE iSpinIndex); // �ʱ�ȭ�� �ʿ��� �Ķ���͸� ����(���� spinType�� ����)
	void Update_Spin(_float fTimeDelta);

	void Update_TypeRing(_float fTimeDelta);
	void Update_TypeLens(_float fTimeDelta);
	void Update_TypeSmokeBall(_float fTimeDelta);
	void Update_TypeRingWind(_float fTimeDelta);
	void Update_TypeLineBall(_float fTimeDelta);
	void Update_TypeGroundWind(_float fTimeDelta);
	void Update_TypeGroundBall(_float fTimeDelta);
	void Update_TypeDoubleKick(_float fTimeDelta);
	void Update_TypeBlastBall(_float fTimeDelta);
	void Update_TypeLanding(_float fTimeDelta);
	void Update_TypeKickAura(_float fTimeDelta);
	void Update_TypeMetamorphosis(_float fTimeDelta);
	void Update_TypeItemGround(_float fTimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	EFFECTSPINDESC			m_EffectSpinDesc;
	_float					m_UpdatedSize = 1.f; // ��ȭ�� ������

	_bool					m_bRender = false;
	_bool					m_bReverse = false; // ũ�� ��ȭ�� ���� ��Ű��
	_bool					m_bAlphaReverse = false; // alphaũ�⸦ ��ȭ ��Ű��
	_bool					m_bFindMat = false; // �����Ҷ��� ������� ����ֳ�
	_float					m_fAlpha = 0.f;
	_float3					m_vSkillColor = { 0.f,0.f,0.f };
	_float2					m_MoveUV = { 0.f,0.f };
	
	_float					m_fSpinTime = 0.f; // ����Ʈ�� Ư�����ǿ��� �ð��� ��� �뵵�� ����.

public:
	static CEffectSpin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END