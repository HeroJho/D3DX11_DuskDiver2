#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CAnimMesh final : public CGameObject
{
public:
	typedef struct tagMeshInfo
	{
		_tchar* sTag;
		_float3 fScale = { 1.f,1.f,1.f };
		_float4 fPos = { 0.f,0.f,0.f,1.f };
		_float3 fAngle = { 0.f,0.f,0.f };

	}MESHINFO;

private:
	CAnimMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimMesh(const CAnimMesh& rhs);
	virtual ~CAnimMesh() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Rotation(_float3 vAxis, _float fRadian, _float3 vAxis2, _float fRadian2, _float3 vAxis3, _float fRadian3);
	_float3 Get_Angles() { return m_fAngles; }
	_float3 Get_Pos();
	void Set_Pos(_float4 Pos);
	void Set_Scale(_float3 fScale) { m_pTransformCom->Set_Scale(XMLoadFloat3(&fScale)); }
	_float3 Get_Scale() { return m_pTransformCom->Get_Scale(); }
	const _tchar* Get_Tag() { return sTag.c_str(); }
	int Get_AnimIndex() { return m_iAniIndex; }
	void Set_AnimIndex(_int iAniIndex) { m_iAniIndex = iAniIndex; }
	int Get_NumAnimations();
	const char* Get_AnimName(int AniIndex);
	void Set_AnimName(const char* Name, int AniIndex);
	HRESULT SaveBinary();
	void ChangeAni(int iAniIndex);

	_float Get_Duration();
	void Set_Duration(_float fDuration);

	_float& Get_PlaySpeed() { return m_fPlaySpeed; }
	void Set_PlaySpeed(_float& m_fPlaySpeed) { m_fPlaySpeed = m_fPlaySpeed; }

	_float Get_TickPerSecond();
	void Set_TickPerSecond(_float fTickPerSecond);

	_float Get_PlayTime();
	void Set_PlayTime(_float fPlayTime);

	_uint Get_KeyFrame();
	void Set_KeyFrame(_float fKeyFrame);

	_bool& Get_bPlay() { return m_bPlay; }
	void Set_bPlay(_bool bPlay) { m_bPlay = bPlay; }

	_bool& Get_bLocalPos() { return m_bLocalPos; }

	vector<_float>& Get_TimeLimit();
	_float Get_TimeLimit(int iIndex);
	void Set_TimeLimit(_float fTimeLimit, int iIndex);
	void Add_TimeLimit();
	void Delete_TimeLimit();
	void Set_RootHierarchy(_int iIndex);
	void Change_AniIndex(int Index1, int Index2);

	void Reset_KeyFrame();

	void DeleteAnimation(int Index);

private:
	_float3 m_fAngles;
	wstring sTag;
	MESHINFO*				m_MeshInfo;
	_bool					m_bPlay = true;
	_bool					m_bLocalPos = false;
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	

	
private:
	HRESULT Ready_Components();
	int m_iAniIndex = 0;
	int m_iPreAniIndex = 0;
	_float m_fPlaySpeed = 1.f;

public:
	static CAnimMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END