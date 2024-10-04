#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CMiniGame_Yabawe : public CGameObject
{
public:
	typedef struct tagMiniGame_YabaDesc
	{
		_float4x4 mMatrix;
	}MINIGAMEYABADESC;


private:
	CMiniGame_Yabawe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMiniGame_Yabawe(const CMiniGame_Yabawe& rhs);
	virtual ~CMiniGame_Yabawe() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


public:
	void Set_IsShake(_bool bIsShake) { m_bIsShake = bIsShake; }
	_bool Get_IsShake() { return m_bIsShake; }

	void Level_Up();

	class CYabaweBall* Get_YabaweBall() { return m_pYabaweBall; }

	_uint Get_Level() { return m_iLevel; }

public:
	void Make_Yabawe();
	void Make_Loot(); 

	// 어떤 야바위를, 어떤 c
	void Shake_Random();
	void Shake(_uint iIndex_1, _uint iRootIndex_1, _bool bInv1, _float fSpeed_1, _float fSens_1, _uint iIndex_2, _uint iRootIndex_2, _bool bInv2, _float fSpeed_2, _float fSens_2, _float fStopDis);

	void Set_Reset();

	class CRealYabawe* Get_RealYabawe() { return m_pRealYabawe; }

	_bool Get_End() { return m_bEnd; }

private:
	class CYabawe* m_pYabawes[3];
	class CRealYabawe* m_pRealYabawe = nullptr;
	class CYabaweBall* m_pYabaweBall = nullptr;
	_float3 m_Loots[6][3];

	_uint m_iShakeCount = 0; // 섞는 카운트
	_bool m_bIsShake = false; // 섞고 있는지
	_float m_fShakeSpeed = 0.f; // 섞는 속도
	_float m_fSens = 0.f;
	_float m_fStopDis = 0.f;

	_uint m_iLevel = 0;

	_float4x4 m_Matrix;


	_bool m_bEnd = false;

private:
	CTransform*				m_pTransformCom = nullptr;


private:
	HRESULT Ready_Components();


public:
	static CMiniGame_Yabawe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END