#pragma	 once

#include "Base.h"
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

#pragma region 중략

public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag);
	_float Get_CamDistance() const { return m_fCamDistance; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth() { return S_OK; }
	virtual void Create_HitEffect(_float4 vWorldPos) {}

public:
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) { return S_OK; }

public:
	bool Get_Dead() { return m_bDead; }
	void Set_Dead() { m_bDead = true; }



protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected: /* 객체에게 추가된 컴포넌트들을 키로 분류하여 보관한다. */
	map<const _tchar*, class CComponent*>			m_Components;
	typedef map<const _tchar*, class CComponent*>	COMPONENTS;

protected:
	_float				m_fCamDistance = 0.f;
	_bool				m_bDead = false;


protected:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	HRESULT Compute_CamZ(_fvector vWorldPos);



private:
	class CComponent* Find_Component(const _tchar* pComponentTag);

#pragma endregion

	//===============================
	//		For. Collider
	//===============================
public:
	/* 충돌이 발생했을 때 호출되는 함수입니다. */
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) {};

public:
	const string& Get_Tag() { return m_sTag; }
	void Set_Tag(string sTag) { m_sTag = sTag; }

	CCollider* Get_Colliders(string sTag);
	list<CCollider*>* Get_Colliders() { return &m_Colliders; }

	/* 콜라이더를 생성하는 함수입니다. */
	HRESULT AddCollider(CCollider::TYPE eType, CCollider::COLLIDERDESC Desc);

	HRESULT Choose_Col(const char* sName);

	void Emp_Col();

protected:
	/* 콜라이더를 갱신합니다. 콜라이더를 사용하려면 꼭 LateTick에서 호출해 줘야합니다! */
	void Tick_Col(_fmatrix TransformMatrix);
	/* 콜라이더를 그립니다. */
	void Render_Col(class CRenderer* pRenderer);

protected:
	map<const char*, CCollider*>  m_ProColliders;
	list<CCollider*>		m_Colliders;
	string m_sTag = "";




	//===============================
	//		For. ImGuiDebug
	//===============================
public:
	const string& Get_ImGuiTag() { return m_sImGuiTag; }
	void Set_ImGuiTag(string sTag) { m_sImGuiTag = sTag; }
	void Set_Selected(_bool bSelected) { m_bSelected = bSelected; }

public:
	void Make_ImGui();
	virtual void Render_Debug() {};

protected:
	string m_sImGuiTag = "";
	_bool	m_bSelected = false;
	static _uint m_iDebugTagCount;

public:
	void Fix_Timer(_float fRatio, _float fTimeLimit);
	void Tick_Timer(_float fTimeDelta);
	void Reset_Timer();

protected:
	_float m_fTMRatio = 1.f;
	_float m_fTMAcc = 0.f;
	_float m_fTMLimit = 0.f;
	_bool m_bFixTimer = false;



public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END