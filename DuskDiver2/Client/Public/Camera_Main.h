#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CPlayer;

class CCamera_Main final : public CCamera
{
public:
	typedef struct CutSceneDesc
	{

		_float3 vPos = _float3(0.f, 0.f, 0.f);				// 위치
		_float3 vLook = _float3(0.f, 0.f, 0.f);				// 룩
		_bool bLerp;										// Pos 보간
		_bool bLookLerp;									// Look 보간
		_float fLerpSpeed = 1.f;							// Pos 보간 스피드
		_float fLookLerpSpeed = 1.f;						// Look 보간 스피드
		_float3 vPivotLookPos = _float3(0.f, 0.f, 0.f);		// Look 피봇
		CTransform* pTran = nullptr;						// 상대적 트렌
		_float3 vPosDis = _float3(0.f, 0.f, 0.f);			// 고정 트렌
		_bool bPixLocal = false;							// 상대적 트렌 회전 적용 여부

	}CUTDESC;

public:
	enum CAMMODE { CAM_PLAYER, CAM_DEBUG, CAM_TRACE, CAM_END };
	enum PLAYECAMMODE {PCAM_PLAYER, PCAM_TARGET, PCAM_CUTSCENE, PCAM_HOLD, PCAM_END};

private:
	CCamera_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Main(const CCamera_Main& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera_Main() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Render_Debug() override;

public:
	CAMMODE Get_CamMode() { return m_eCamMode; }
	void Set_CamMode();

	_float3 Get_CamPos();
	_float3 Get_CamLook();
	_float3 Get_CamBackPos(_float fRightDis, _float fUpDis, _float fLooDis);

	void Set_CamPos(_float vUp, _float vRight);

private:
	void Tick_Player(_float fTimeDelta);
	void Tick_Debug(_float fTimeDelta);

	void LateTick_Player(_float fTimeDelta);
	void LateTick_Debug(_float fTimeDelta);



private:
	CAMMODE m_eCamMode = CAM_PLAYER;
	_float3 m_vDistance;
	_float3 m_vOriDistance;

public:
	static CCamera_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;



	// ======================
	//		For.GameCam
	// ======================
public:
	void Shake(_float fPow, _float fTime, _bool bIsLerp = false, _float fTurnTime = 0.f);
	void Fov(_float fZoomPower, _float fZoomTime, _float fZoomStopTime, _bool bReset = true, _bool bSoothReset = true);
	void Target(CTransform* pTran);

	void Show_Target(CUTDESC Desc);
	void End_Target();

	void Hold_Pos(_float3 vPos, class CTransform* pTran_L, class CTransform* pTran_R, _float3 vPos_R, _float fLimitDis);
	void End_Hold();

	_float3 Get_PreLook() { return *((_float3*)&m_vPreLook); }


	void Edit_CamDis(_float3 vDis);
	void End_CamDis();

	void Save_World();
	void Load_World();


private:
	void Tick_PlayerChase(_float fTimeDelta);
	void Tick_TargetChase(_float fTimeDelta);
	void Tick_TargetShow(_float fTimeDelta);
	void Tick_HoldTick(_float fTimeDelta);
	void Tick_Shake(_float fTimeDelta);
	void Tick_Fov(_float fTimeDelta);

	void Input_Target(_float fTimeDelta);

private:
	_float4 m_vPrePos;
	_float4 m_vPreLook;
	_float m_fLerpSpeed = 6.f;
	_float m_fLookLerpSpeed = 0.1f;
	_float m_fLookRatio = 0.f;

	_float4x4 m_SaveWorld;

	// Shake
	_bool m_bShake = false;
	_bool m_bIsLerp = false;

	_float m_fShakePow = 0.f;
	_float m_fShakeTime = 0.f;
	_float m_fShakeTimeAcc = 0.f;

	_float3 m_vLepDir;
	_float m_fShakePowAcc = 0.f;
	_float m_fShakeTurnTime = 0.f;
	_float m_fShakeTurnTimeAcc = 0.f;

	// Zoom
	_bool m_bIsZoom = false;
	_bool m_bIsZoomStop = false;

	_float m_fZoomPower = 0.f;
	_float m_fZoomTime = 0.f;
	_float m_fZoomStopTime = 0.f;
	_float m_fZoomTimeAcc = 0.f;

	_bool m_bReset = false;
	_bool m_bSmoothReset = false;

	// Target
	CTransform* m_pTargetTran = nullptr;
	_float3 m_vPosDis = _float3(0.f, 0.f, 0.f);
	_float3 m_vShowPos = _float3(0.f, 0.f, 0.f);
	_float3 m_vShowLook = _float3(0.f, 0.f, 0.f);
	_bool m_bLerp = false;
	_float3 m_vPivotLookPos = _float3(0.f, 0.f, 0.f);

	_bool m_bLookLerp = false;
	_bool m_bPixLocal = false;
	_float m_fCutPosLerpSpeed = 1.f;
	_float m_fCutLookLerpSpeed = 1.f;

	// Hold Pos
	_float m_fHoldLimitDis = 0.f;
	_float3 m_vHoldPos;
	CTransform* m_pHoldTarget_L = nullptr;
	CTransform* m_pHoldTarget_R = nullptr;
	_float3 m_vPos_R;

	PLAYECAMMODE m_ePCamMode = PCAM_PLAYER;

	_float4 m_vPlayerPrePos;
	// ======================
	//		For.CutScene
	// ======================
public:
	void Set_Pos(_float3 vPos);
	_bool Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance, _bool bLimitPass = false);
	void LookAt(_fvector vTargetPos);
	void ResetPrePos();

	_bool Get_CutScene() { return m_bIsCutScene; }
	void Set_CutScene(_bool bCutScene) { m_bIsCutScene = bCutScene; }

private:
	_bool m_bIsCutScene = false;

};

END