#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)
class CParts;

class CStage2Player04 final : public CCreture
{
public:
	enum Parts { PARTS_HAIR, PARTS_END };
	enum Socket { SOCKET_HEAD, SOCKET_WEAPON_R, SOCKET_WEAPONBACK, SOCKET_END };
	enum STATE {
		RUN, WALK, AIRATTACK1, AIRATTACK2, AIRATTACK3, ATTACK1, ATTACK2, ATTACK3, ATTACK4, ATTACK5, BOUNCE, BROKEN, BURST, DASH,
		GROGGYEND, GROGGYLOOF, GROGGYSTART, DOWN, VOIDFRONT, VOIDAIR, VOIDATTACK, FINALBURST, FINISHNORMAL, FINISHWALL, GETUP, SKILL1, HURTBACK,
		HURTFLY, HURTFRONT, IDLE1, IDLE2, JUMPBACK, JUMPLOOF, JUMPSTART, JUMPEND, QICKSTAND, SKILL2, SKILL3START, SKILL3LOOF, SKILL3END,
		SKILL4START, SKILL4END, SKILL5, SKILL6, SKILL7START, SKILL7END, SKILL8, SLAPDOWN, SKILL9, SKILL10, SKILL11START, SKILL11LOOF,
		SKILL11END, SKILL12START, SKILL12END, SKILL13START, SKILL13END, RUNFASTEND, RUNFASTLOOF, RUNFASTSTART, STEPBACK, WALKLEFT, WALKRIGHT,
		STUN, STUNEND, RAGESKILL1, RAGESKILL2, FALLDOWN, HITAWAY, HITSPIN, HITDOWN, WALKBACK
	};
private:
	CStage2Player04(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage2Player04(const CStage2Player04& rhs);
	virtual ~CStage2Player04() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private://���� �÷��̾��Լ�
	void Set_State(STATE eState); // ���¸� ���� ������ �ϴ� �ִϸ��̼��� ���⼭ �ִϸ��̼� ����
	void End_Animation(); // �ִϸ��̼��� ������ �� �۾��� ���� ������ ���� �ִϸ��̼��� ���⼭ �ִϸ��̼� ����
	void Update(_float fTimeDelta); // ���� ���¿� ���� �۾�����


private://������Ʈ, �������� �Լ���
	HRESULT Ready_Components();
	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();
	HRESULT Update_Parts();
	void Render_Parts();

private: //�÷��̾� ���κ���
	STATE m_eCurState; // ���� ����
	STATE m_eNextState; // �ٲ���� ����
	_float3 m_vTargetLook; //�ٶ� ����

	//===_matrix===
	_matrix m_HeadMatrix;
	_matrix m_WeaponBack;
	_matrix m_Weapon_R;

	_bool m_bTalk = false;
	_bool m_bTalkEnd = false;
	_bool m_bQuestPointOn = false;
private:// STL �����̳�
	vector<CParts*>				m_Parts;
	vector<class CHierarchyNode*>		m_Sockets;

private: //������Ʈ
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

public:
	static CStage2Player04* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END