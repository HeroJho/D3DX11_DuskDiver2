#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CTransform;
END

BEGIN(Client)

class CStage4Star : public CCreture
{
public:
	typedef struct tagStarDesc {
		_uint iStarIndex;// 몇번째 별인가
		_float fRadius; // 공전 반지름
	}STARDESC;
private:
	
private:
	CStage4Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage4Star(const CStage4Star& rhs);
	virtual ~CStage4Star() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();





private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTransform*				m_pCenterTransformCom = nullptr;

	STARDESC				m_StarDesc;
	_float4					m_vRtAxis = { 0.f,0.f,0.f,0.f };
	_float					m_fRtSpeed = 0.f;

	_bool					m_bReverse = false; // 사이즈변화를 반전시키냐
	_float					m_fStarSize = 1.f;

	_float m_fBlurPower = 0.15f;


	//for Debug
	//========================================
	_float					NewX = 0.f;
	_float					NewY = 0.f;
	_float					NewZ = 0.f;
	_float					Speed = 1.f;

private:
	HRESULT Ready_Components();
	void Init_RotateAxis();
	void Update_Star(_float fTimeDelta); // Tick에서 한번만 전체 Update를 관리
	void Update_Center(_float fTimeDelta);// 중심가상 물체의 상태 체인지
	void Update_SelfState(_float fTimeDelta);// Star본인의 상태 라업룩을 갱신

public:
	static CStage4Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END