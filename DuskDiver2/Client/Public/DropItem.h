#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CDropItem : public CGameObject
{
public:
	typedef struct tagDropItemDesc
	{
		_uint		iItemType;		// � ����������ΰ�
		_uint		iNaviNum;		// ���� � �׺� Ż���ΰ�
		_float4		vStartPosition;	// �������� �����������
		_float		fGravityAcc;	// �߷°��ӵ��� ũ��
		_float		iInitJumpPower; // ó�������Ҷ��� �����ӵ�
		_uint		iPopUpType; // � �������� �Ծ��°�(UI���ؽ��� ����)
	}DROPITEMDESC;
	enum ITEMTYPE { TYPE_WAFER, TYPE_MONEY, TYPE_SKIN, TYPE_BLADE, TYPE_CLAW, TYPE_ARM, TYPE_PLATED, TYPE_CHIP, TYPE_GOLD, TYPE_CLUE, TYPE_END };
private:
	CDropItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDropItem(const CDropItem& rhs);
	virtual ~CDropItem() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	void Play_ItemSound();
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

	class CEffectSpin*		m_pEffect = nullptr;

private:
	DROPITEMDESC			m_DropItemDesc;
	_float4x4				m_ItemWorldMat; // ��Ʈ������ ������ ������µ� �ڵ带 ¥�鼭 ������ ���ϰ� �־ ��ǻ� �Ⱦ����� ����� �ȵǴ� ����.
	_float					m_fCurJumpPower = 0.f; // �������� �������� �����ӵ�
	_float					m_fSpeed = 0.f; // ƨ�ܳ����� �ӵ�
	_bool					m_bOnGround = false; // ����Ҷ� ƨ��� ������ ���ΰ�?
	_float					m_fChaseSpeed = 0.f; // �÷��̾ �Ѿư��� �ӵ�
	_float					m_fBlurPower = 0.f;

	_bool					m_bGotFirstPos = false;
	_float					m_fDeleteTime = 0.f; // �����ð� �� �Ծ����� ����

	_float					m_fEffectSize = 0.f;
	_bool					m_bReverse = false;
	
	_bool	m_bFlow = false; // ����� ��
	

private:
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);
	void Set_InitState();
	void Tick_DropItemGravity(_float fTimeDelta); // �߷��� �޴� �Լ�
	void Rebound(); // ���� ������ ƨ��� �ӵ��� ������ ���ִ� �Լ�
	void Bounce(_float fTimeDelta); // ƨ�ܼ� ������ ���ư��� �Լ�
	void Chase_Player(); // �÷��̾ �Ѿư��� �Լ�
	void Update_EffectSize(_float fTimeDelta);
	void Update_ClueEvent(); // �ܼ� ȹ�涧���� ī��Ʈ���� �ش��ϴ� �̺�Ʈ �ߵ� �Լ�

public:
	static CDropItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END