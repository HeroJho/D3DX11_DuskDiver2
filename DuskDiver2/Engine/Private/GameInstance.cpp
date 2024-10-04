#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pFrustum(CFrustum::Get_Instance())
	, m_pFont_Manager(CFont_Manager::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pSound_Manager(CSound_Manager::Get_Instance())
	, m_pCollider_Manager(CColliderManager::Get_Instance())
	, m_pValue_Manager(CValueManager::Get_Instance())
	, m_pPhysX_Manager(CPhysX_Manager::Get_Instance())
{	
	Safe_AddRef(m_pValue_Manager);
	Safe_AddRef(m_pPhysX_Manager);
	Safe_AddRef(m_pCollider_Manager);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pGraphic_Device);
}


HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* 엔진을 구동하기위한 초기화작업을 거치낟. */
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 그래픽디바이스 초기화. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext)))
		return E_FAIL;

	/* 입력 초기화. */
	if (FAILED(m_pInput_Device->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 사운드 초기화. */


	///* 타겟매니져의 디버깅 준비. */
	//if (FAILED(m_pTarget_Manager->Initialize(*ppDevice, *ppContext)))
	//	return E_FAIL;


	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (FAILED(m_pSound_Manager->Initialize()))
		return E_FAIL;

	if (FAILED(m_pLight_Manager->Init(*(ppDevice), *(ppContext), iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pPhysX_Manager->Init()))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pInput_Device)
		return;



	m_pInput_Device->Update();

	m_pObject_Manager->Tick(fTimeDelta);

	m_pPipeLine->Update();
	
	m_pFrustum->Tick();

	m_pObject_Manager->LateTick(fTimeDelta);

	m_pPhysX_Manager->Tick(fTimeDelta);
	
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pSound_Manager->Tick(fTimeDelta);
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();	
}

_uint CGameInstance::Get_Cur_LevelIndex()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_Cur_LevelIndex();
}

_uint CGameInstance::Get_Reserve_LevelIndex()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_Reserve_LevelIndex();
}

void CGameInstance::Reserve_Level(_uint iLevelIndex)
{
	if (nullptr == m_pLevel_Manager)
		return;

	m_pLevel_Manager->Reserve_Level(iLevelIndex);
}

CComponent * CGameInstance::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iLayerIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ComponentPtr(iLevelIndex, pLayerTag, pComponentTag, iLayerIndex);	
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag, pArg);	
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObjectToLayer(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

HRESULT CGameInstance::Add_Get_GameObjectToLayer(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void * pArg, CGameObject ** Out_pObj)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Get_GameObjectToLayer(pPrototypeTag, iLevelIndex, pLayerTag, pArg, Out_pObj);
}

const map<const _tchar*, class CLayer*>* CGameInstance::Get_Layers(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Layers(iLevelIndex);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);	
}

HRESULT CGameInstance::Release_Component(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Release_Component(iLevelIndex, pPrototypeTag);
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Update_Timer(pTimerTag);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

CInput_Device::INPUTMODE CGameInstance::Key_Down(_uchar eKeyID, _int iMode)
{
	if (nullptr == m_pInput_Device)
		return CInput_Device::INPUT_STOP;

	return m_pInput_Device->Key_Down(eKeyID, iMode);
}

CInput_Device::INPUTMODE CGameInstance::Key_Up(_uchar eKeyID, _int iMode)
{
	if (nullptr == m_pInput_Device)
		return CInput_Device::INPUT_STOP;

	return m_pInput_Device->Key_Up(eKeyID, iMode);
}

CInput_Device::INPUTMODE CGameInstance::Key_Pressing(_uchar eKeyID, _int iMode)
{
	if (nullptr == m_pInput_Device)
		return CInput_Device::INPUT_STOP;

	return m_pInput_Device->Key_Pressing(eKeyID, iMode);
}

CInput_Device::INPUTMODE CGameInstance::Mouse_Down(DIMK eMouseKeyID, _int iMode)
{
	if (nullptr == m_pInput_Device)
		return CInput_Device::INPUT_STOP;

	return m_pInput_Device->Mouse_Down(eMouseKeyID, iMode);
}

CInput_Device::INPUTMODE CGameInstance::Mouse_Up(DIMK eMouseKeyID, _int iMode)
{
	if (nullptr == m_pInput_Device)
		return CInput_Device::INPUT_STOP;

	return m_pInput_Device->Mouse_Up(eMouseKeyID, iMode);
}

CInput_Device::INPUTMODE CGameInstance::Mouse_Pressing(DIMK eMouseKeyID, _int iMode)
{
	if (nullptr == m_pInput_Device)
		return CInput_Device::INPUT_STOP;

	return m_pInput_Device->Mouse_Pressing(eMouseKeyID, iMode);
}

_int CGameInstance::Get_DebugInput()
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DebugInput();
}

void CGameInstance::Set_DebugInput(_int iDebugInput)
{
	m_pInput_Device->Set_DebugInput(iDebugInput);
}

_uint CGameInstance::Get_Random(_uint iStart, _uint iEnd)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dis(iStart, iEnd);

	return dis(gen);
}

_float CGameInstance::Get_FloatRandom(_float iStart, _float iEnd)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(iStart, iEnd);

	return dis(gen);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eTransformState);	
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);
	
}

_float4x4 CGameInstance::Get_TransformFloat4x4_Inverse(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_Inverse(eTransformState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eTransformState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();	
}

_float3 CGameInstance::Get_CamDir(CPipeLine::DIRSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float3();

	return m_pPipeLine->Get_CamDir(eState);
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);	
}

HRESULT CGameInstance::Add_Light(const LIGHTDESC & LightDesc, CLight** pOut)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(LightDesc, pOut);
}

HRESULT CGameInstance::Add_ShadowLightViewMatrix(_uint iLevel, _float4x4 mMatrix)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	
	return m_pLight_Manager->Add_ShadowLightViewMatrix(iLevel, mMatrix);
}

_float4x4 CGameInstance::Get_ShadowLightViewMatrix(_uint iLevel)
{
	if (nullptr == m_pLight_Manager)
		return _float4x4();

	return m_pLight_Manager->Get_ShadowLightViewMatrix(iLevel);
}



HRESULT CGameInstance::Reset_Light()
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Reset_Lights();
}

LIGHTDESC * CGameInstance::Get_DirLightDesc()
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_DirLightDesc();
}

CLight * CGameInstance::Get_DirLight()
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_DirLight();
}



list<class CLight*>* CGameInstance::Get_Lights()
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_Lights();
}

HRESULT CGameInstance::Delete_Light(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Delete_Light(iIndex);
}

HRESULT CGameInstance::Delete_Light(CLight * pLight)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Delete_Light(pLight);
}


_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vWorldPos, float fRadius)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRadius);
}

HRESULT CGameInstance::Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Fonts(pDevice, pContext, pFontTag, pFontFilePath);	
}

HRESULT CGameInstance::Render_Fonts(const _tchar * pFontTag, const _tchar * pTextm, _float2 vPosition, _fvector vColor, _float fAngle, _float2 vOrigin, _float2 vScale)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Fonts(pFontTag, pTextm, vPosition, vColor, fAngle, vOrigin, vScale);	
}

HRESULT CGameInstance::Bind_RenderTargetSRV(const _tchar * pTargetTag, CShader * pShader, const char * pConstantName)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Bind_SRV(pTargetTag, pShader, pConstantName);	
}

HRESULT CGameInstance::PlaySound(TCHAR * pSoundKey, const _uint & eID, const float & fVolume)
{
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	m_pSound_Manager->PlaySound(pSoundKey, eID, fVolume);
	return S_OK;
}

HRESULT CGameInstance::PlayBGM(TCHAR * pSoundKey, const float & fVolume)
{
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
	return S_OK;
}

HRESULT CGameInstance::StopSound(const _uint & eID)
{
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	m_pSound_Manager->StopSound(eID);
	return S_OK;
}

HRESULT CGameInstance::StopAll()
{
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	m_pSound_Manager->StopAll();
	return S_OK;
}

int CGameInstance::VolumeUp(const _uint & eID, const _float & _vol)
{

	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	return m_pSound_Manager->VolumeUp(eID, _vol);


}

int CGameInstance::VolumeDown(const _uint & eID, const _float & _vol)
{
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	return m_pSound_Manager->VolumeDown(eID, _vol);


}

void CGameInstance::PlaySound_Linear(TCHAR * pSoundKey, const _uint & eID, const float & fVolume, _bool bLoof, float fLinearSpeed)
{
	if (m_pSound_Manager == nullptr)
	return;

	m_pSound_Manager->PlaySound_Linear(pSoundKey, eID, fVolume, bLoof, fLinearSpeed);
}

void CGameInstance::Add_ColGroup(CColliderManager::COLLIDERGROUP eGroup, CGameObject * pObj)
{
	if (nullptr == m_pCollider_Manager)
		return;

	m_pCollider_Manager->Add_ColGroup(eGroup, pObj);
}

void CGameInstance::Calcul_ColGroup(CColliderManager::COLLIDERGROUP eGroupL, CColliderManager::COLLIDERGROUP eGroupR)
{
	if (nullptr == m_pCollider_Manager)
		return;

	m_pCollider_Manager->Calcul_ColGroup(eGroupL, eGroupR);
}

void CGameInstance::Clear_ColGroup()
{
	if (nullptr == m_pCollider_Manager)
		return;

	m_pCollider_Manager->Clear_ColGroup();
}

CValueManager::VALUEDESC * CGameInstance::Get_ValueDesc()
{
	if (nullptr == m_pValue_Manager)
		return nullptr;

	return m_pValue_Manager->Get_ValueDesc();
}


void CGameInstance::Add_Actor(PxActor * pAxtor)
{
	if (nullptr == m_pPhysX_Manager)
		return;

	m_pPhysX_Manager->Add_Actor(pAxtor);
}

void CGameInstance::Remove_Actor(PxActor * pAxtor)
{
	if (nullptr == m_pPhysX_Manager)
		return;

	m_pPhysX_Manager->Remove_Actor(pAxtor);
}

PxRigidDynamic * CGameInstance::Create_JointBox(PxTransform vPos, _float3 vExtent, PxMaterial * pMaterial, _uint iFlag, _bool bKin)
{
	if (nullptr == m_pPhysX_Manager)
		return nullptr;

	return m_pPhysX_Manager->Create_JointBox(vPos, vExtent, pMaterial, iFlag, bKin);
}

PxJoint * CGameInstance::Create_DampedD6(PxRigidActor * a0, const PxTransform & t0, PxRigidActor * a1, const PxTransform & t1)
{
	if (nullptr == m_pPhysX_Manager)
		return nullptr;

	return m_pPhysX_Manager->Create_DampedD6(a0, t0, a1, t1);
}

PxMaterial * CGameInstance::Create_Material(_float fA, _float fB, _float fC)
{
	if (nullptr == m_pPhysX_Manager)
		return nullptr;

	return m_pPhysX_Manager->Create_Material(fA, fB, fC);
}

PxRigidStatic* CGameInstance::Create_Static(_float3 vPos, _float3 vRot, _float3 vExtent, _uint iFlag)
{
	if (nullptr == m_pPhysX_Manager)
		return nullptr;

	return m_pPhysX_Manager->Create_Static(vPos, vRot, vExtent, iFlag);
}

void CGameInstance::Tick(_float fTimeDelta)
{
	if (nullptr == m_pPhysX_Manager)
		return;

	m_pPhysX_Manager->Tick(fTimeDelta);
}



void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Destroy_Instance();

	CValueManager::Get_Instance()->Destroy_Instance();

	CColliderManager::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CPhysX_Manager::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CSound_Manager::Get_Instance()->Destroy_Instance();

	CLight_Manager::Get_Instance()->Destroy_Instance();

	CFrustum::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();	

	CFont_Manager::Get_Instance()->Destroy_Instance();

	CTarget_Manager::Get_Instance()->Destroy_Instance();

	CPipeLine::Get_Instance()->Destroy_Instance();
	
	CGraphic_Device::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pPhysX_Manager);
	Safe_Release(m_pValue_Manager);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device); 
	
}
