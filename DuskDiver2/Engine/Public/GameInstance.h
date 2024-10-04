#pragma once
#include <random>
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "Frustum.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include "Sound_Manager.h"
#include "ColliderManager.h"
#include "Light_Manager.h"
#include "ValueManager.h"
#include "PhysX_Manager.h"


/* 클라이언트로 보여주기위한 가장 대표적인 클래스이다. */
/* 각종 매니져클래스들의 주요함수를 클라로 보여준다.  */
/* 엔진초기화. */
/* 엔진정리. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(_uint iNumLevel, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	
public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Level_Manager */				
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	HRESULT Render_Level();
	_uint Get_Cur_LevelIndex();
	_uint Get_Reserve_LevelIndex();
	void Reserve_Level(_uint iLevelIndex);

public: /* For.Object_Manager */
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iLayerIndex);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);
	HRESULT Add_Get_GameObjectToLayer(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr, CGameObject** Out_pObj = nullptr);
	const map<const _tchar*, class CLayer*>* Get_Layers(_uint iLevelIndex);

public: /*For.Component_Manager*/
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT Release_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);


public: /* for.Timer_Manager */
	_float Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT Add_Timer(const _tchar* pTimerTag);
	HRESULT Update_Timer(const _tchar* pTimerTag);	

public: /* For.Input_Device */
	_long Get_DIMMoveState(DIMM eMouseMoveID);
	CInput_Device::INPUTMODE Key_Down(_uchar eKeyID, _int iMode);
	CInput_Device::INPUTMODE Key_Up(_uchar eKeyID, _int iMode);
	CInput_Device::INPUTMODE Key_Pressing(_uchar eKeyID, _int iMode);
	CInput_Device::INPUTMODE Mouse_Down(DIMK eMouseKeyID, _int iMode);
	CInput_Device::INPUTMODE Mouse_Up(DIMK eMouseKeyID, _int iMode);
	CInput_Device::INPUTMODE Mouse_Pressing(DIMK eMouseKeyID, _int iMode);
	_int Get_DebugInput();
	void Set_DebugInput(_int iDebugInput);
public:
	_uint Get_Random(_uint iStart, _uint iEnd);
	_float Get_FloatRandom(_float iStart, _float iEnd);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const;		
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_Inverse(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4 Get_CamPosition();
	_float3 Get_CamDir(CPipeLine::DIRSTATE eState);

public: /* For.Light_Manager */
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHTDESC& LightDesc, CLight** pOut = nullptr);
	HRESULT Add_ShadowLightViewMatrix(_uint iLevel, _float4x4 mMatrix);
	_float4x4 Get_ShadowLightViewMatrix(_uint iLevel);
	HRESULT Reset_Light();
	LIGHTDESC* Get_DirLightDesc();
	class CLight* Get_DirLight();
	list<class CLight*>* Get_Lights();
	HRESULT Delete_Light(_uint iIndex);
	HRESULT Delete_Light(CLight* pLight);


public: /* For.Frustum */
	_bool isIn_Frustum_WorldSpace(_fvector vWorldPos, float fRadius = 0.f);

public: /* For.Font_Manager */
	HRESULT Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Fonts(const _tchar* pFontTag, const _tchar* pTextm, _float2 vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fAngle = 0.f, _float2 vOrigin = _float2(0.f, 0.f), _float2 vScale = _float2(1.f, 1.f));

public:  /* For.Target_Manager*/ 
	HRESULT Bind_RenderTargetSRV(const _tchar* pTargetTag, class CShader* pShader, const char* pConstantName);

public:/*For. Sound_Manager*/
	HRESULT PlaySound(TCHAR* pSoundKey, const _uint& eID, const float& fVolume);
	HRESULT PlayBGM(TCHAR * pSoundKey, const float& fVolume);
	HRESULT StopSound(const _uint& eID);
	HRESULT StopAll();
	int  VolumeUp(const _uint& eID, const _float& _vol);
	int  VolumeDown(const _uint& eID, const _float& _vol);
	void PlaySound_Linear(TCHAR* pSoundKey, const _uint& eID, const float& fVolume, _bool bLoof = false, float fLinearSpeed = 0.f);

public: /* For. ColliderManager */
	void Add_ColGroup(CColliderManager::COLLIDERGROUP eGroup, class CGameObject* pObj);
	void Calcul_ColGroup(CColliderManager::COLLIDERGROUP eGroupL, CColliderManager::COLLIDERGROUP eGroupR);
	void Clear_ColGroup();

public: /* For. ValueManager */
	CValueManager::VALUEDESC* Get_ValueDesc();

public: /* For. PhysX_Manager */
	void Add_Actor(PxActor * pAxtor);
	void Remove_Actor(PxActor* pAxtor);
	PxRigidDynamic*	Create_JointBox(PxTransform vPos, _float3 vExtent, PxMaterial* pMaterial, _uint iFlag = 0, _bool bKin = false);
	PxJoint* Create_DampedD6(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);
	PxMaterial*		Create_Material(_float fA, _float fB, _float fC);
	PxRigidStatic* Create_Static(_float3 vPos, _float3 vRot, _float3 vExtent, _uint iFlag);
	void Tick(_float fTimeDelta);


private:
	CGraphic_Device*				m_pGraphic_Device = nullptr;
	CInput_Device*					m_pInput_Device = nullptr;
	CSound_Manager*					m_pSound_Manager = nullptr;
	CLevel_Manager*					m_pLevel_Manager = nullptr;
	CObject_Manager*				m_pObject_Manager = nullptr;
	CComponent_Manager*				m_pComponent_Manager = nullptr;
	CTimer_Manager*					m_pTimer_Manager = nullptr;
	CPipeLine*						m_pPipeLine = nullptr;
	CLight_Manager*					m_pLight_Manager = nullptr;
	CFrustum*						m_pFrustum = nullptr;
	CFont_Manager*					m_pFont_Manager = nullptr;
	CTarget_Manager*				m_pTarget_Manager = nullptr;
	CColliderManager*				m_pCollider_Manager = nullptr;
	CValueManager*					m_pValue_Manager = nullptr;
	CPhysX_Manager*					m_pPhysX_Manager = nullptr;

public:
	static void Release_Engine();
	virtual void Free() override;
};

END