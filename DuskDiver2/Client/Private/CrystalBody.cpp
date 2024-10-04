#include "stdafx.h"
#include "..\Public\CrystalBody.h"
#include "GameInstance.h"
#include "CrystalPart.h"
#include "ToolManager.h"
#include "DropItem.h"

#include "Player_Manager.h"

CCrystalBody::CCrystalBody(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCreture(pDevice, pContext)
{
}

CCrystalBody::CCrystalBody(const CCrystalBody & rhs)
	: CCreture(rhs)
{
}

HRESULT CCrystalBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrystalBody::Initialize(void * pArg)
{
	m_BodyDesc = *(BODYDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_BodyDesc.vPosition));

	Initialize_Parts(*(_float3*)&m_BodyDesc.vPosition, m_BodyDesc.vRadianAngle);

	return S_OK;
}

void CCrystalBody::Tick(_float fTimeDelta)
{
	Emp_Col();

	Update_RenderState();

	if ((true == m_bHited) && (false == m_bItemCreated))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 100.f, 0.f, 0.f)); // 충돌체를 이동시켜 충돌 안시키는 용도
		Create_Item();

		for (auto& iter : m_PartALayer)
			iter->Phx_Awake();
		for (auto& iter : m_PartBLayer)
			iter->Phx_Awake();

		m_PartALayer[0]->Add_Force(_float3(0.f, 1.f, 0.f));
	}

	if (true == m_bHited)
	{
		Update_Dissolve(fTimeDelta);
	}
		
}

void CCrystalBody::LateTick(_float fTimeDelta)
{
	if (false == m_bHited)
	{
		if (nullptr == m_pRendererCom)
			return;

		Tick_Col(m_pTransformCom->Get_WorldMatrix());
		Choose_Col(nullptr);
		Render_Col(m_pRendererCom);
	}

	if (true == m_bRender)
	{
		for (auto& iter : m_PartALayer)
		{
			iter->LateTick(fTimeDelta);
		}
		for (auto& iter : m_PartBLayer)
		{
			iter->LateTick(fTimeDelta);
		}
	}
	
}

HRESULT CCrystalBody::Render()
{
	for (auto& iter : m_PartALayer)
	{
		iter->Render();
	}
	for (auto& iter : m_PartBLayer)
	{
		iter->Render();
	}

	return S_OK;
}

void CCrystalBody::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	//IG->Debug_Transform(m_pTransformCom);
	
	// 영혼을 렌더 하냐 마냐
	ImGui::Checkbox("RenderBody", &m_bRender); 
	// A or B타입을 생성
	ImGui::Checkbox("Create A", &m_bSelectA); 
	ImGui::SameLine();
	ImGui::Checkbox("Create B", &m_bSelectB);
	// 블럭 생성 코드
	if (true == m_bSelectA) // A타입 생성
	{
		if (ImGui::Button("Create A Type"))
		{
			CCrystalPart::PARTDESC CryStalPartDesc;
			CryStalPartDesc.iCrystalType = CCrystalPart::TYPE_A;
			CryStalPartDesc.fSize = 1.f;
			CryStalPartDesc.vLocalPosition = { 0.f,0.f,0.f,1.f };
			CryStalPartDesc.fAngle = 0.f;

			CGameObject*		pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);

			m_PartALayer.push_back((CCrystalPart*)pGameObject);
		}
	}
	if (true == m_bSelectB) // B타입 생성
	{
		if (ImGui::Button("Create B Type"))
		{
			CCrystalPart::PARTDESC CryStalPartDesc;
			CryStalPartDesc.iCrystalType = CCrystalPart::TYPE_B;
			CryStalPartDesc.fSize = 1.f;
			CryStalPartDesc.vLocalPosition = { 0.f,0.f,0.f,1.f };
			CryStalPartDesc.fAngle = 0.f;

			CGameObject*		pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);

			m_PartBLayer.push_back((CCrystalPart*)pGameObject);
		}
	}

	// A or B타입을 edit
	ImGui::Checkbox("Edit A", &m_bEditA);
	ImGui::SameLine();
	ImGui::Checkbox("Edit B", &m_bEditB);
	// A or B타입중 생성된 객체 인덱스
	ImGui::SliderInt("Choose Index", &m_iChooseIndex, 0, 10/*, "ratio = &d"*/);
	//ImGui::SliderInt("Selected Type's Index", &m_iSelectIndex, 0, 10/*, "ratio = &d"*/);

	ImGui::DragFloat3("Pos", (_float*)&m_SelectedPosition, 0.05f);
	ImGui::DragFloat("Rotation_Angle", &m_SelectedAngle, 0.05f);
	ImGui::DragFloat("Size", &m_SelectedSize, 0.05f);

	// 블럭 에디트 코드
	if (true == m_bEditA)
	{
		m_bEditB = false;
		if (m_iSelectIndex >= m_PartALayer.size())
			goto ENDEDIT;

		m_PartALayer[m_iSelectIndex]->Change_Position(m_SelectedPosition);
		m_PartALayer[m_iSelectIndex]->Change_Rotation(m_SelectedAngle);
		if (0.f < m_SelectedSize)
			m_PartALayer[m_iSelectIndex]->Change_Size(m_SelectedSize);
	}
	if (true == m_bEditB)
	{
		m_bEditA = false;
		if (m_iSelectIndex >= m_PartBLayer.size())
			goto ENDEDIT;

		m_PartBLayer[m_iSelectIndex]->Change_Position(m_SelectedPosition);
		m_PartBLayer[m_iSelectIndex]->Change_Rotation(m_SelectedAngle);
		if (0.f < m_SelectedSize)
			m_PartBLayer[m_iSelectIndex]->Change_Size(m_SelectedSize);
	}

	
	// 아래의 버튼을 누르면 choose에 해당하느 인덱스의 블럭의 위치를 얻어온다
	if (ImGui::Button("Get_ChooseA_Info"))
	{
		if (m_iChooseIndex >= m_PartALayer.size())
			goto ENDEDIT;
		
		m_SelectedPosition = m_PartALayer[m_iChooseIndex]->Get_Position();
		m_SelectedAngle = m_PartALayer[m_iChooseIndex]->Get_RotationAngle();
		m_SelectedSize = m_PartALayer[m_iChooseIndex]->Get_Size();

		m_bEditA = false;
		m_bEditB = false;

		m_iSelectIndex = m_iChooseIndex;
	}
	if (ImGui::Button("Get_ChooseB_Info"))
	{
		if (m_iChooseIndex >= m_PartBLayer.size())
			goto ENDEDIT;
			
		m_SelectedPosition = m_PartBLayer[m_iChooseIndex]->Get_Position();
		m_SelectedAngle = m_PartBLayer[m_iChooseIndex]->Get_RotationAngle();
		m_SelectedSize = m_PartBLayer[m_iChooseIndex]->Get_Size();

		m_bEditA = false;
		m_bEditB = false;

		m_iSelectIndex = m_iChooseIndex;
	}

		
	

ENDEDIT:
	ImGui::End();
}

void CCrystalBody::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}

void CCrystalBody::On_Hit(CCreture * pOther)
{
	m_bHited = true;
}

HRESULT CCrystalBody::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Emissive"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.4f, 0.4f, 0.4f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1"); // Monster_Body1 //CrystalBody
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.1f, 1.1f, 1.1f);
	ColDesc.vCenter = _float3(0.f, 0.6f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

void CCrystalBody::Initialize_Parts(_float3 WorldPos, _float WorldRadianAngle)
{
	//=====================================================================================================AAAAAAAAAA
	CCrystalPart::PARTDESC CryStalPartDesc;
	CryStalPartDesc.iCrystalType = CCrystalPart::TYPE_A;
	CryStalPartDesc.vLocalPosition = { -0.35f,0.f,-0.25f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = -0.45f + WorldRadianAngle;
	CryStalPartDesc.fSize = 1.f;
	CGameObject*		pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartALayer.push_back((CCrystalPart*)pGameObject);

	CryStalPartDesc.vLocalPosition = { 0.3f,0.f,-0.15f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = 0.55f + WorldRadianAngle;
	CryStalPartDesc.fSize = 1.1f;
	pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartALayer.push_back((CCrystalPart*)pGameObject);

	CryStalPartDesc.vLocalPosition = { -0.15f,0.f,0.4f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = 1.15f + WorldRadianAngle;
	CryStalPartDesc.fSize = 0.85f;
	pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartALayer.push_back((CCrystalPart*)pGameObject);

	CryStalPartDesc.vLocalPosition = { 0.1f,0.75f,0.f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = 0.35f + WorldRadianAngle;
	CryStalPartDesc.fSize = 0.8f;
	pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartALayer.push_back((CCrystalPart*)pGameObject);

	CryStalPartDesc.vLocalPosition = { -0.35f,1.35f,-0.2f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = 0.6f + WorldRadianAngle;
	CryStalPartDesc.fSize = 0.85f;
	pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartALayer.push_back((CCrystalPart*)pGameObject);

	CryStalPartDesc.vLocalPosition = { 0.f,1.3f,0.35f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = 1.2f + WorldRadianAngle;
	CryStalPartDesc.fSize = 1.f;
	pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartALayer.push_back((CCrystalPart*)pGameObject);

	CryStalPartDesc.vLocalPosition = { 0.5f,1.4f,-0.15f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = -1.45f + WorldRadianAngle;
	CryStalPartDesc.fSize = 0.8f;
	pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartALayer.push_back((CCrystalPart*)pGameObject);

	//=====================================================================================================BBBBBBBBBB
	CryStalPartDesc.iCrystalType = CCrystalPart::TYPE_B;
	CryStalPartDesc.vLocalPosition = { -0.05f,0.45f,0.f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = 0.f + WorldRadianAngle;
	CryStalPartDesc.fSize = 1.f;
	pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartBLayer.push_back((CCrystalPart*)pGameObject);

	CryStalPartDesc.vLocalPosition = { -0.1f,1.05f,-0.15f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = 0.f + WorldRadianAngle;
	CryStalPartDesc.fSize = 1.f;
	pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartBLayer.push_back((CCrystalPart*)pGameObject);

	CryStalPartDesc.vLocalPosition = { 0.4f,1.2f,0.15f,1.f };
	XMStoreFloat4(&CryStalPartDesc.vLocalPosition, XMLoadFloat4(&CryStalPartDesc.vLocalPosition) + XMLoadFloat3(&WorldPos));
	CryStalPartDesc.fAngle = 0.4f + WorldRadianAngle;
	CryStalPartDesc.fSize = 0.8f;
	pGameObject = GI->Clone_GameObject(TEXT("CrystalPart"), &CryStalPartDesc);
	m_PartBLayer.push_back((CCrystalPart*)pGameObject);
}

void CCrystalBody::Create_Item()
{
	// 던서 생성용이 아니라면 아이템을 생성
	if (false == m_BodyDesc.bClue)
	{
		_uint iRandomA = GI->Get_Random(1, 3);

		CDropItem::DROPITEMDESC DropItemDesc;
		DropItemDesc.iItemType = CDropItem::TYPE_WAFER;
		DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
		DropItemDesc.vStartPosition = m_BodyDesc.vPosition;
		for (_uint i = 0; iRandomA > i; ++i)
		{
			GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}

		_uint iRandomB = GI->Get_Random(1, 3);

		DropItemDesc.iItemType = CDropItem::TYPE_MONEY;
		for (_uint i = 0; iRandomB > i; ++i)
		{
			GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}
	}
	// 단서 생성용이라면 단서를 생성
	if (true == m_BodyDesc.bClue)
	{
		CDropItem::DROPITEMDESC DropItemDesc;
		DropItemDesc.iItemType = CDropItem::TYPE_CLUE;
		DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
		DropItemDesc.vStartPosition = m_BodyDesc.vPosition;
		GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
	}

	m_bItemCreated = true;
}

void CCrystalBody::Update_Dissolve(_float fTimeDelta)
{
	m_fDissolveAcc += 0.05f * fTimeDelta;

	for (auto& iter : m_PartALayer)
	{
		iter->Change_DissolveAcc(m_fDissolveAcc);
	}
	for (auto& iter : m_PartBLayer)
	{
		iter->Change_DissolveAcc(m_fDissolveAcc);
	}

	if (1.f < m_fDissolveAcc)
		m_bDead = true;
}

void CCrystalBody::Update_RenderState()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();

	// 조건1 : 거리가 maximum이내인가?
	_float fLength = 0.f;
	fLength = sqrt((vPlayerPos.x - m_BodyDesc.vPosition.x) * (vPlayerPos.x - m_BodyDesc.vPosition.x) + (vPlayerPos.z - m_BodyDesc.vPosition.z) * (vPlayerPos.z - m_BodyDesc.vPosition.z));
	// 조건2 : 절두체 컬링범위내에 있는가?
	_bool bisInFrustum = GI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f);

	if ((65.f > fLength) && ((true == m_bHited) || (true == bisInFrustum)))
		m_bRender = true;
	else
		m_bRender = false;
}

CCrystalBody * CCrystalBody::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCrystalBody*		pInstance = new CCrystalBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCrystalBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCrystalBody::Clone(void * pArg)
{
	CCrystalBody*		pInstance = new CCrystalBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCrystalBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrystalBody::Free()
{
	__super::Free();

	for (auto& iter : m_PartALayer)
	{
		Safe_Release(iter);
	}
	for (auto& iter : m_PartBLayer)
	{
		Safe_Release(iter);
	}

	//Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
