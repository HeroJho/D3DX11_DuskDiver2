#include "stdafx.h"
#include "..\Public\Example.h"
#include "GameInstance.h"
#include "HierarchyNode.h"


#include "DataManager.h"

#pragma region �����ӿ�ũ ����
/*
	Ű��ǲ F10, F12 ����
	
	F1 ����� ���� Ű��
	F2 Ű��ǲ ��庯ȯ
	F3 ī�޶� ��庯ȯ
	F4 ī�޶� �������
	F5 = �� Ű��ǲ On,Off
	F6 = �� ��ŷ On,Off
	F7 = ����Ÿ�� On, Off
	
	Ű�Է� TK_DOWN TK_UP, TK_PRESS			T = ���������
	CK_DOWN CK_UP, CK_PRESS					C = Ŭ���̾�Ʈ����
	K_DOWN, CK_UP, CK_PRESS					K = ������ �޴´�
	
	���콺�Է� TM_DOWN, TM_UP, TM_PRESS		T = ���������
	CM_DOWN, CM_UP, CM_PRESS				C = Ŭ���̾�Ʈ����
	M_DOWN, M_UP, M_PRESS					K = ������ �޴´�

	
	=====��ũ��========
	IG = ImGUi_Manager
	GI = GameInstance
	TM = Tool_Manager
	DM = Data_Manager
	PM = Player_Manager
	CRM = Camera_Manager
	(UM = UI_Manager)
	
	=====���ҽ� Load�ϴ¹�=======
	�ϴ� ����ƽ���� ����Ұ����� ���� ���������� ������� ���� ��
	�ش緹�� �ε忡�� �ۼ����ָ� �ȴ�.
	ex -> �������� 1,2,3�� ������ ������ ��� Static�������� �ε�
	�������� 4���� ������ ������ ��� Stage4�������� �ε�

	���� ��� Anim, NonAnim ��ο� fbx�� �ؽ�ó�� �־�θ�ȴ�.
	������!!
	1. �����̸��̶� fbx�̸��̶� �Ȱ��ƾ���
	2. �������� ��������fbx�� ������ �ȵ� �������� fbx �Ѱ���
	
	���� �ƴѰ�� 
	���� �ۼ����ָ� �ȴ�.

	
	
	���̴��� �۾��� ������ �����ѰŶ� ���ϰ� �۾�
	(���� ������ �ȵ� �����ϸ鼭 �ؾ���)
	
	
*/
#pragma endregion �����ӿ�ũ ����


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

	// *[HeroJho]: �浹ó������ ����� �±��Դϴ�.
	m_sTag = "Tag_Player";
	// *[HeroJho]: ImGui�� ������� �Ϸ��� �� ȣ������� �մϴ�.
	Make_ImGui();

	m_pModelCom->Set_AnimIndex(57);

	//if (FAILED(Ready_Sockets()))
	//	return E_FAIL;

	//if (FAILED(Ready_PlayerParts()))
	//	return E_FAIL;

	// m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));

	// *[HeroJho]: ���� ���� ���ִ� �׺� �ε����� �ڵ����� �����ݴϴ�. ����� ��α� ������ Tick�� ���� �ݺ� ȣ�� �����Դϴ�!
	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);


	return S_OK;
}

void CExample::Tick(_float fTimeDelta)
{


	if (CK_DOWN(DIK_SPACE))
	{
		// *[HeroJho]: �����մϴ��� 1���� = JumpPower
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
	// *[HeroJho]: Tran�� ������ �ִ�Pos�� ����� ����˴ϴ�. Navi�� ������ ���� �� ����� �մϴ�.
	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);
	// *[HeroJho]: �߷� ���� �ڵ��Դϴ�. 2����-�׺���̼��� ������ ���� Ÿ��, ���������� �� �������ϴ�. 3����- �߷� ���ӵ��Դϴ�. ���� ���� �޿� �ִ� ��ó�� �յ� ���ٴմϴ�.
	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 7.5f);
	// *[HeroJho]: �浹ó���� �Ϸ��� �� ȣ���ؾ��մϴ�! (�ݶ��̴� ���� ����)
	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	// *[HeroJho]: 1����-�� ��ü�� �з� ��, 2����- �ڱ��ڽ��� ������(this)
	GI->Add_ColGroup(CColliderManager::COLLIDER_PLAYER, this);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);


	// *[HeroJho]: �浹ü�� ������ ȣ���ϼ���.
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

// *[HeroJho]: ����� â�Դϴ�. ImGui�� �۵��˴ϴ�.
void CExample::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);


	ImGui::End();
}

// *[HeroJho]: �浹�� �߻��� �� ȣ��Ǵ� �Լ��Դϴ�.
void CExample::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	// *[HeroJho]:
	// Desc.pOther		- �浹�� ����� ��ü�� �ּ��Դϴ�.
	// Desc.pMyDesc		- ������ �浹�� ���� �ݶ��̴��� �����Դϴ�.
	// Desc.pOtherDesc	- ���� �浹�� ������� �ݶ��̴� �����Դϴ�.

	// �������̶��
	if ("Tag_Item" == Desc.pOther->Get_Tag())
	{
		// �� OBB�� ������� OBB�� �浹 �ߴٸ�
		if (!strcmp("Col_Test_OBB", Desc.pMyDesc->sTag) &&
			!strcmp("Col_Test_OBB", Desc.pOtherDesc->sTag))
		{
			_int i = 0;
		}
	}
	// �������̶��
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
	// *[HeroJho]: �ִϸ��̼� ������ ����� �����ϱ� ���ؼ� ��! ȣ���ؾ� �մϴ�.
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
