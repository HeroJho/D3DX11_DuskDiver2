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

class CFissure : public CGameObject
{
public:
	typedef struct tagFissureDesc
	{
		_float4 vPos;
		_uint iIndex;
		_float fRange; // 시야에 나오는 범위
	}FISSUREDESC;
private:
	enum CRACKSTATE { STATE_CLOSED, STATE_RIPPED, STATE_END };
	enum MASKTYPE { MASK_UV, MASK_RANGE, MASK_BORDER, MASK_END };
private:
	CFissure(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFissure(const CFissure& rhs);
	virtual ~CFissure() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	_float4 Get_FissurePos();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom[MASK_END] = { nullptr };

	FISSUREDESC				m_FissureDesc;

	_bool					m_bRenderState = false;
	_bool					m_bTest = false;
	_bool					m_bChanged = false;
	_bool					m_bSoundOn = false;
	CRACKSTATE				m_CurCrackState = STATE_CLOSED;
	CRACKSTATE				m_LastCrackState = STATE_CLOSED;

	_float2					m_MoveUV = { 0.f,-0.6f }; // 마스크맵을 움직일 uv
	_float					m_fSizeZ = 0.f; // 변동할 Z 크기
	_float					m_fMeshAlpha = 0.f;

	_float					m_fSizeMin = 0.1f;
	_float					m_fSizeMax = 2.f;
	_float					m_fSizeSpeed = 9.f;
	_float					m_fUVSpeed = 2.f;

	_uint					m_iShaderPass = 24;

	_bool					m_bSpecialEvent = false; // 특수 상황에 사용 : 4번 -> 가까울때 OnSight일때 전투대화 출력
	
	_bool					m_bMonsterSpawn = false;

	_bool					m_bBlindStart = false;
	_bool					m_bBlindEnd = false;
private:
	HRESULT Ready_Components();
	_bool Check_OnSight(); // 플레이어가 범위에 들어왔는가?
	void Update_Crack(_float fTimeDelta); // 균열이 찢기거나 닫히거나
	void Rip_Crack(_float fTimeDelta); // 균열을 찢는다. 이전상태는 닫힘
	void Close_Crack(_float fTimeDelta); // 균열을 닫는다. 이전상태는 찢김

public:
	static CFissure* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END