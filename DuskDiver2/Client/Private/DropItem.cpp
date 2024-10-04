#include "stdafx.h"
#include "..\Public\DropItem.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "EffectSpin.h"
#include "DropItemUI.h"
#include "UI.h"

CDropItem::CDropItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CDropItem::CDropItem(const CDropItem & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDropItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDropItem::Initialize(void * pArg)
{
	m_DropItemDesc = *(DROPITEMDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Create_Navigation(m_DropItemDesc.iNaviNum)))
		return E_FAIL;

	m_sTag = "Tag_DropItem";

	Set_InitState();

	CEffectSpin::EFFECTSPINDESC EffectSpinDesc;
	if (TYPE_CLUE == m_DropItemDesc.iItemType)
	{
		EffectSpinDesc.iSpinType = CEffectSpin::TYPE_ItemGroundClue;
	}
	else
	{
		EffectSpinDesc.iSpinType = CEffectSpin::TYPE_ItemGround;
	}
	m_pEffect = (CEffectSpin*)GI->Clone_GameObject(TEXT("EffectSpin"), &EffectSpinDesc);
	m_pEffect->Set_EffectPos(_float4(1000.f, 1000.f, 1000.f,1.f));

	return S_OK;
}

void CDropItem::Tick(_float fTimeDelta)
{
	Tick_DropItemGravity(fTimeDelta);
	Bounce(fTimeDelta);
	Update_EffectSize(fTimeDelta);

	if (m_bOnGround)
	{
		if (TYPE_CLUE != m_DropItemDesc.iItemType)
		{
			m_fDeleteTime += 1.f * fTimeDelta;
			if (30.f < m_fDeleteTime)
				Set_Dead();
		}

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

		if (false == m_bGotFirstPos)
		{
			_float4 vEffectPos = { 0.f,0.0f,0.f,1.f };
			XMStoreFloat4(&vEffectPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			vEffectPos.y += 0.05f;
			m_pEffect->Set_EffectPos(vEffectPos);
			m_bGotFirstPos = true;
		}
		m_pEffect->Tick(fTimeDelta); // ����Ʈ�� ƽ�� ������
	}
		
}

void CDropItem::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this); //RENDER_NONALPHABLEND
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

	if (true == m_bOnGround) 
		m_pEffect->LateTick(fTimeDelta); // ����Ʈ�� latetick�� ������
	
}

HRESULT CDropItem::Render()
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

	// For.Blur
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_EMISSIVE, "g_EmissiveTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 19)))
			return E_FAIL;
	}

	m_pEffect->Render(); // ����Ʈ�� ������ ������

	return S_OK;
}

void CDropItem::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	IG->Debug_Transform(m_pTransformCom);

	ImGui::End();
}

void CDropItem::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if (true == m_bOnGround)
	{
		if ("Tag_Player" == Desc.pOther->Get_Tag())
		{
			// �ܼ��ΰ�쿡�� �浹 ó�� ���ϰ�
			if ((TM->Get_Stage3_Bool()) && (TYPE_CLUE == m_DropItemDesc.iItemType))
			{
				return;
			}
			if ((!strcmp("DropItem_Chase", Desc.pMyDesc->sTag) &&
				!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
			{
				// �÷��̾ �Ѿư��� �ڵ�
				Chase_Player();
				m_pEffect->Set_EffectPos({ 100000.f, 100000.f , 100000.f , 1.f });
			}
			if ((!strcmp("DropItem_Gain", Desc.pMyDesc->sTag) &&
				!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
			{
				// �������� �Դ� �ڵ�
				Play_ItemSound();

				// UI �����ڵ�
				CUI::UIINFODESC UiInfoDesc;
				ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
				
				UiInfoDesc.fSizeX = 322.f * 0.65f;
				UiInfoDesc.fSizeY = 322.f * 0.65f;
				UiInfoDesc.fX = 1170.2f;
				UiInfoDesc.fY = 255.f;
				UiInfoDesc.pDesc = &(m_DropItemDesc.iPopUpType);

				if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_DropItemUI"), LEVEL_STATIC, TEXT("Layer_UI"), &UiInfoDesc)))
					return;

				// �ܼ��� ��� �ܼ� ī��Ʈ �����ڵ�
				if (TYPE_CLUE == m_DropItemDesc.iItemType)
				{
					UM->Add_ClueCnt();
					if (1 < UM->Get_ClueCnt()) // ù��° �ܼ��� ���� ī��Ʈ�� �������� ����
					{
						UM->Plus_Coin();
					}
					Update_ClueEvent();
				}
				// ����Ʈ ���� �ڵ�
				if ((TYPE_CLUE == m_DropItemDesc.iItemType) || ((TYPE_MONEY == m_DropItemDesc.iItemType) && (5 == m_DropItemDesc.iPopUpType)))
					PM->Create_ItemGetEffect(CPlayer_Manager::ITEMGETTYPE::ITEMGET_RARE);
				else
					PM->Create_ItemGetEffect(CPlayer_Manager::ITEMGETTYPE::ITEMGET_NORMAL);

				// ���� �ڵ�
				m_bDead = true;
			}
		}
	}

}

void CDropItem::Play_ItemSound()
{
	//���� �Ծ��� ��
	if (TYPE_GOLD == m_DropItemDesc.iItemType)
	{
		int Index = GI->Get_Random(0, 3);

		switch (Index)
		{
		case 0:
			GI->PlaySoundW(L"se_Env12_Coin1.wav", SD_DROPITEM, 1.f);
			break;
		case 1:
			GI->PlaySoundW(L"se_Env12_Coin2.wav", SD_DROPITEM, 1.f);
			break;
		case 2:
			GI->PlaySoundW(L"se_Env12_Coin3.wav", SD_DROPITEM, 1.f);
			break;
		case 3:
			GI->PlaySoundW(L"se_Env12_Coin4.wav", SD_DROPITEM, 1.f);
			break;
		}
	}
	
	//�ܼ� ����� ��
	else if (TYPE_CLUE == m_DropItemDesc.iItemType)
	{
		int Index = GI->Get_Random(0, 1);

		switch (Index)
		{
		case 0:
			GI->PlaySoundW(L"btAllow.wav", SD_DROPITEM, 1.f);
			break;
		case 1:
			GI->PlaySoundW(L"btClick.wav", SD_DROPITEM, 1.f);
			break;
		}
	}

	else
	{
 		int Index = GI->Get_Random(0, 2);

		switch (Index)
		{
		case 0:
			GI->PlaySoundW(L"se_Item_Fly-01.wav", SD_DROPITEM, 0.8f);
			break;
		case 1:
			GI->PlaySoundW(L"se_Item_Fly-02.wav", SD_DROPITEM, 0.8f);
			break;
		case 2:
			GI->PlaySoundW(L"se_Item_Fly-03.wav", SD_DROPITEM, 0.8f);
			break;
		}
	}
}

HRESULT CDropItem::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Emissive"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	switch (m_DropItemDesc.iItemType)
	{
	case TYPE_GOLD:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_VA_GoldA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_WAFER:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Wafer"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_MONEY:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_MoneyA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_SKIN:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_DisasterSkin"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_BLADE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_FusionBlade"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_CLAW:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BetaClaw"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_ARM:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_AlloyedArm"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_PLATED:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_Plated"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_CHIP:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_Chip"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_CLUE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_Clue"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.5f, 2.5f, 2.5f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "DropItem_Chase");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "DropItem_Gain");
	ColDesc.iFlag = 2;
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDropItem::Create_Navigation(_uint iNaviNum)
{
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;


	switch (iNaviNum)
	{
	case 3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Stage1"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	case 5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Stage1"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	case 6:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage2Navigation"), TEXT("Stage2"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	case 7:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage3Navigation"), TEXT("Stage3"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	case 8:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage4Navigation"), TEXT("Stage4"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	case 9:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage5Navigation"), TEXT("Stage5"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_DropItemDesc.vStartPosition));
	_uint iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	return S_OK;
}

void CDropItem::Set_InitState()
{
	// ���� ��ġ�� �Է¹ް� ������ ������ ���� ���´�.
	_float4 vInitRight = { 0.f,0.f,0.f,0.f };
	_float4 vInitUp = { 0.f,1.f,0.f,0.f };
	_float4 vInitLook = { 0.f,0.f,0.f,0.f };

	vInitLook.x = GI->Get_FloatRandom(0.f, 1.f);
	vInitLook.y = 0.f;
	vInitLook.z = GI->Get_FloatRandom(0.f, 1.f);

	XMStoreFloat4(&vInitLook, XMVector4Normalize(XMLoadFloat4(&vInitLook)));
	XMStoreFloat4(&vInitRight, XMVector4Normalize(XMVector3Cross(XMLoadFloat4(&vInitUp), XMLoadFloat4(&vInitLook))));

	m_ItemWorldMat._11 = vInitRight.x;
	m_ItemWorldMat._12 = vInitRight.y;
	m_ItemWorldMat._13 = vInitRight.z;
	m_ItemWorldMat._14 = vInitRight.w;

	m_ItemWorldMat._21 = vInitUp.x;
	m_ItemWorldMat._22 = vInitUp.y;
	m_ItemWorldMat._23 = vInitUp.z;
	m_ItemWorldMat._24 = vInitUp.w;

	m_ItemWorldMat._31 = vInitLook.x;
	m_ItemWorldMat._32 = vInitLook.y;
	m_ItemWorldMat._33 = vInitLook.z;
	m_ItemWorldMat._34 = vInitLook.w;

	m_ItemWorldMat._41 = m_DropItemDesc.vStartPosition.x;
	m_ItemWorldMat._42 = m_DropItemDesc.vStartPosition.y;
	m_ItemWorldMat._43 = m_DropItemDesc.vStartPosition.z;
	m_ItemWorldMat._44 = 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4(&vInitRight));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMLoadFloat4(&vInitUp));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat4(&vInitLook));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&m_ItemWorldMat).r[3]);

	// �����ӵ��� �̵��ӵ��� ���������� �޴´�.
	switch (m_DropItemDesc.iItemType)
	{
	case TYPE_WAFER:
		m_DropItemDesc.fGravityAcc = 1.5f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(6.f, 10.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(1.f, 3.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.25f;
		m_DropItemDesc.iPopUpType = 0;
		break;
	case TYPE_MONEY:
		m_DropItemDesc.fGravityAcc = 1.5f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(6.f, 10.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(1.f, 3.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.25f;
		break;
	case TYPE_SKIN:
		m_DropItemDesc.fGravityAcc = 2.f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(8.f, 12.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(2.f, 4.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.2f;
		m_DropItemDesc.iPopUpType = 6;
		break;
	case TYPE_BLADE:
		m_DropItemDesc.fGravityAcc = 2.f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(8.f, 12.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(2.f, 4.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.2f;
		m_DropItemDesc.iPopUpType = 7;
		break;
	case TYPE_CLAW:
		m_DropItemDesc.fGravityAcc = 2.f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(8.f, 12.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(2.f, 4.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.2f;
		m_DropItemDesc.iPopUpType = 8;
		break;
	case TYPE_ARM:
		m_DropItemDesc.fGravityAcc = 2.f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(8.f, 12.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(2.f, 4.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.2f;
		m_DropItemDesc.iPopUpType = 9;
		break;
	case TYPE_PLATED:
		m_DropItemDesc.fGravityAcc = 2.f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(8.f, 12.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(2.f, 4.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.2f;
		m_DropItemDesc.iPopUpType = 10;
		break;
	case TYPE_CHIP:
		m_DropItemDesc.fGravityAcc = 2.f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(8.f, 12.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(2.f, 4.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.2f;
		m_DropItemDesc.iPopUpType = 11;
		break;
	case TYPE_GOLD:
		m_DropItemDesc.fGravityAcc = 2.f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(8.f, 12.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(2.f, 4.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.2f;
		break;
	case TYPE_CLUE:
		m_DropItemDesc.fGravityAcc = 2.f * 9.8f;
		m_DropItemDesc.iInitJumpPower = GI->Get_FloatRandom(8.f, 12.f);
		m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
		m_fSpeed = GI->Get_FloatRandom(2.f, 4.f);
		m_fChaseSpeed = 0.2f;
		m_fBlurPower = 0.2f;
		break;
	default:
		break;
	}
	
	if (TYPE_MONEY == m_DropItemDesc.iItemType)
	{
		_uint iRandomA = GI->Get_Random(0, 150);

		if (10 >= iRandomA)
			m_DropItemDesc.iPopUpType = 5;
		else if ((10 < iRandomA) && (30 > iRandomA))
			m_DropItemDesc.iPopUpType = 4;
		else if ((30 <= iRandomA) && (60 > iRandomA))
			m_DropItemDesc.iPopUpType = 3;
		else if ((60 <= iRandomA) && (100 > iRandomA))
			m_DropItemDesc.iPopUpType = 2;
		else if ((100 <= iRandomA) && (150 >= iRandomA))
			m_DropItemDesc.iPopUpType = 1;
	}
}

void CDropItem::Tick_DropItemGravity(_float fTimeDelta)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_float fCellY = 0.f;

	if (m_pNavigationCom->isGround(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &fCellY) && (0.f >= m_fCurJumpPower))
	{
		m_fCurJumpPower = 0.f;
		// ���⼭ �¿��.
		vPos.y = fCellY;
		Rebound();
	}
	else
	{
		m_fCurJumpPower -= m_DropItemDesc.fGravityAcc * fTimeDelta;
	}

	vPos.y += m_fCurJumpPower * fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
}

void CDropItem::Rebound()
{
	m_DropItemDesc.iInitJumpPower *= 0.6f;
	m_fSpeed *= 0.6f;

	if (0.1f >= m_DropItemDesc.iInitJumpPower)
	{
		m_fCurJumpPower = 0.f;
		m_bOnGround = true;
		return;
	}

	m_fCurJumpPower = m_DropItemDesc.iInitJumpPower;
}

void CDropItem::Bounce(_float fTimeDelta)
{
	if (false == m_bOnGround)
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fSpeed, fTimeDelta, m_pNavigationCom);
}

void CDropItem::Chase_Player()
{
	_float4 vDir = { 0.f,0.f,0.f,0.f };
	XMStoreFloat4(&vDir, XMVector4Normalize(XMLoadFloat4(&PM->Get_PlayerPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_fChaseSpeed * XMLoadFloat4(&vDir));
}

void CDropItem::Update_EffectSize(_float fTimeDelta)
{
	if (false == m_bReverse)
		m_fEffectSize += 1.f * fTimeDelta;
	else
		m_fEffectSize -= 1.f * fTimeDelta;

	if (1.f > m_fEffectSize)
	{
		m_fEffectSize = 1.f;
		m_bReverse = false;
	}
	if (2.f < m_fEffectSize)
	{
		m_fEffectSize = 2.f;
		m_bReverse = true;
	}

	m_pEffect->Set_UpdateSize(m_fEffectSize);
}

void CDropItem::Update_ClueEvent()
{
	if (1 == UM->Get_ClueCnt())
	{
		// ��Ӻ��� ���ε� �ܼ� ��
		UM->Render_Clue1();
	}
	else if ((1 < UM->Get_ClueCnt()) && (6 > UM->Get_ClueCnt()))
	{
		// ���� ����Ʈ ���� ��
		UM->Render_Clue2();
	}
	else if (6 == UM->Get_ClueCnt())
	{
		// �� ���ε� �̺�Ʈ
		UM->Set_TalkIndex(53);
		UM->Set_FrontUI(false);
	}
}

CDropItem * CDropItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDropItem*		pInstance = new CDropItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDropItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDropItem::Clone(void * pArg)
{
	CDropItem*		pInstance = new CDropItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDropItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDropItem::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
