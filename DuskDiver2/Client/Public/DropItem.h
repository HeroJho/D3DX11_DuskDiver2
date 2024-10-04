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
		_uint		iItemType;		// 어떤 드랍아이템인가
		_uint		iNaviNum;		// 현재 어떤 네비를 탈것인가
		_float4		vStartPosition;	// 아이템의 라업룩포지션
		_float		fGravityAcc;	// 중력가속도의 크기
		_float		iInitJumpPower; // 처음시작할때의 점프속도
		_uint		iPopUpType; // 어떤 아이템을 먹었는가(UI인텍스와 통일)
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
	_float4x4				m_ItemWorldMat; // 매트릭스를 쓸려고 만들었는데 코드를 짜면서 갱신을 안하고 있어서 사실상 안쓰지만 지우면 안되는 변수.
	_float					m_fCurJumpPower = 0.f; // 아이템의 생성시의 점프속도
	_float					m_fSpeed = 0.f; // 튕겨나가는 속도
	_bool					m_bOnGround = false; // 드랍할때 튕기는 행위가 끝인가?
	_float					m_fChaseSpeed = 0.f; // 플레이어를 쫓아가는 속도
	_float					m_fBlurPower = 0.f;

	_bool					m_bGotFirstPos = false;
	_float					m_fDeleteTime = 0.f; // 오랜시간 안 먹었으면 삭제

	_float					m_fEffectSize = 0.f;
	_bool					m_bReverse = false;
	
	_bool	m_bFlow = false; // 디버깅 용
	

private:
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);
	void Set_InitState();
	void Tick_DropItemGravity(_float fTimeDelta); // 중력을 받는 함수
	void Rebound(); // 땅에 도착후 튕기는 속도를 재정의 해주는 함수
	void Bounce(_float fTimeDelta); // 튕겨서 앞으로 나아가는 함수
	void Chase_Player(); // 플레이어를 쫓아가는 함수
	void Update_EffectSize(_float fTimeDelta);
	void Update_ClueEvent(); // 단서 획득때마다 카운트수에 해당하는 이벤트 발동 함수

public:
	static CDropItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END