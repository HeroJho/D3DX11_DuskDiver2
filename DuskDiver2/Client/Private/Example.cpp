#include "stdafx.h"
#include "..\Public\Example.h"
#include "GameInstance.h"
#include "HierarchyNode.h"


#include "DataManager.h"

#pragma region 프레임워크 사용법
/*
	키인풋 F10, F12 금지
	
	F1 디버깅 끄기 키기
	F2 키인풋 모드변환
	F3 카메라 모드변환
	F4 카메라 잠금제어
	F5 = 툴 키인풋 On,Off
	F6 = 툴 피킹 On,Off
	F7 = 랜더타겟 On, Off
	
	키입력 TK_DOWN TK_UP, TK_PRESS			T = 툴모드전용
	CK_DOWN CK_UP, CK_PRESS					C = 클라이언트전용
	K_DOWN, CK_UP, CK_PRESS					K = 언제든 받는다
	
	마우스입력 TM_DOWN, TM_UP, TM_PRESS		T = 툴모드전용
	CM_DOWN, CM_UP, CM_PRESS				C = 클라이언트전용
	M_DOWN, M_UP, M_PRESS					K = 언제든 받는다

	
	=====매크로========
	IG = ImGUi_Manager
	GI = GameInstance
	TM = Tool_Manager
	DM = Data_Manager
	PM = Player_Manager
	CRM = Camera_Manager
	(UM = UI_Manager)
	
	=====리소스 Load하는법=======
	일단 스테틱에서 사용할것인지 일정 레벨에서만 사용할지 정한 후
	해당레벨 로드에서 작성해주면 된다.
	ex -> 스테이지 1,2,3에 나오는 몬스터인 경우 Static레벨에서 로드
	스테이지 4에만 나오는 몬스터의 경우 Stage4레벨에서 로드

	모델의 경우 Anim, NonAnim 경로에 fbx와 텍스처를 넣어두면된다.
	주의점!!
	1. 폴더이름이랑 fbx이름이랑 똑같아야함
	2. 한폴더에 여러개의fbx를 넣으면 안됨 한폴더에 fbx 한개씩
	
	모델이 아닌경우 
	직접 작성해주면 된다.

	
	
	셰이더는 작업시 무조건 간단한거라도 말하고 작업
	(따로 머지가 안됨 소통하면서 해야함)
	
	
*/
#pragma endregion 프레임워크 사용법


CExample::CExample(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CExample::CExample(const CExample & rhs)
	: CGameObject(rhs)
{
}

HRESULT CExample::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CExample::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	// *[HeroJho]: 충돌처리에서 사용할 태그입니다.
	m_sTag = "Tag_Player";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();

	m_pModelCom->Set_AnimIndex(57);

	//if (FAILED(Ready_Sockets()))
	//	return E_FAIL;

	//if (FAILED(Ready_PlayerParts()))
	//	return E_FAIL;

	// m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));

	// *[HeroJho]: 현재 내가 서있는 네비 인덱스를 자동으로 구해줍니다. 비용이 비싸기 때문에 Tick과 같은 반복 호출 금지입니다!
	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);


	return S_OK;
}

void CExample::Tick(_float fTimeDelta)
{


	if (CK_DOWN(DIK_SPACE))
	{
		// *[HeroJho]: 점프합니다잉 1인자 = JumpPower
		m_pTransformCom->Jump(7.f);
	}

	if (CK_PRESS(DIK_W))
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f), 5.f, fTimeDelta, m_pNavigationCom);
	else if (CK_PRESS(DIK_A))
		m_pTransformCom->Go_Dir(XMVectorSet(-1.f, 0.f, 0.f, 0.f), 5.f, fTimeDelta, m_pNavigationCom);
	else if (CK_PRESS(DIK_S))
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 0.f, -1.f, 0.f), 5.f, fTimeDelta, m_pNavigationCom);
	else if (CK_PRESS(DIK_D))
		m_pTransformCom->Go_Dir(XMVectorSet(1.f, 0.f, 0.f, 0.f), 5.f, fTimeDelta, m_pNavigationCom);


}

void CExample::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	// *[HeroJho]: Tran을 넣으면 애니Pos가 월드로 적용됩니다. Navi도 넣으면 셀을 못 벗어나게 합니다.
	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);
	// *[HeroJho]: 중력 적용 코드입니다. 2인자-네비게이션을 넣으면 셀을 타고, 넣지않으면 쭉 떨어집니다. 3인자- 중력 가속도입니다. 작을 수록 달에 있는 것처럼 둥둥 떠다닙니다.
	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 7.5f);
	// *[HeroJho]: 충돌처리를 하려면 꼭 호출해야합니다! (콜라이더 월드 갱신)
	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	// *[HeroJho]: 1인자-이 객체의 분류 값, 2인자- 자기자신의 포인터(this)
	GI->Add_ColGroup(CColliderManager::COLLIDER_PLAYER, this);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);


	// *[HeroJho]: 충돌체를 보려면 호출하세요.
	Render_Col(m_pRendererCom);

}

HRESULT CExample::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}



	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CExample::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);


	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CExample::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	// *[HeroJho]:
	// Desc.pOther		- 충돌한 상대편 객체의 주소입니다.
	// Desc.pMyDesc		- 상대편과 충돌한 나의 콜라이더의 정보입니다.
	// Desc.pOtherDesc	- 나와 충돌한 상대편의 콜라이더 정보입니다.

	// 아이템이라면
	if ("Tag_Item" == Desc.pOther->Get_Tag())
	{
		// 내 OBB와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Col_Test_OBB", Desc.pMyDesc->sTag) &&
			!strcmp("Col_Test_OBB", Desc.pOtherDesc->sTag))
		{
			_int i = 0;
		}
	}
	// 슬라임이라면
	else if ("Tag_Slime" == Desc.pOther->Get_Tag())
	{

	}
}


HRESULT CExample::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player01Body"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	// *[HeroJho]: 애니메이션 포스를 월드로 적용하기 위해서 꼭! 호출해야 합니다.
	m_pModelCom->Set_RootHierarchy(3);



	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 20.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	strcpy(ColDesc.sTag, "Col_Test_OBB");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	strcpy(ColDesc.sTag, "Col_Test_AABB");
	if (FAILED(AddCollider(CCollider::TYPE_AABB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(5.f, 5.f, 5.f);
	strcpy(ColDesc.sTag, "Col_Test_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CExample::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("SWORD");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);

	return S_OK;
}

HRESULT CExample::Ready_PlayerParts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Sword */
	CGameObject*		pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Sword"));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_Parts.push_back(pGameObject);

	return S_OK;
}

HRESULT CExample::Update_Weapon()
{

	return S_OK;
}

CExample * CExample::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CExample*		pInstance = new CExample(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CExample"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CExample::Clone(void * pArg)
{
	CExample*		pInstance = new CExample(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CExample"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExample::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();


	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
