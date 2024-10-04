#include "stdafx.h"
#include "..\Public\MinimapTest.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"



CMinimapTest::CMinimapTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMinimapTest::CMinimapTest(const CMinimapTest & rhs)
	: CUI(rhs)
{
}

HRESULT CMinimapTest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMinimapTest::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	m_UiInfo.fSizeX = m_UiInfo.fSizeX * 0.7f;
	m_UiInfo.fSizeY = m_UiInfo.fSizeY * 0.7f;

	return S_OK;
}

void CMinimapTest::Tick(_float fTimeDelta)
{

	m_Pos = PM->Get_PlayerWorldMatrix();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_float4x4 TargetViewMatrix;
		XMStoreFloat4x4(&TargetViewMatrix,
			(XMLoadFloat4x4(&m_Pos) * pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)) * pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		_vector ScreenPos = XMVector3Project(
			XMVectorSet(0.f, 0.f, 0.f, 1.f),
			0,
			0,
			1280.f,
			720.f,
			0.f,
			1.f,
			pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
			pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW),
			XMLoadFloat4x4(&m_Pos)
		);

		_float x = XMVectorGetX(ScreenPos);
		_float y = XMVectorGetY(ScreenPos);


		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet((x - g_iWinSizeX * 0.5f), (-y + g_iWinSizeY * 0.5f), 0.0f, 1.f));
		m_UiInfo.fX = (x + 130.f);
		m_UiInfo.fY = (y - 300.f);

	__super::Tick(fTimeDelta);
}

void CMinimapTest::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMinimapTest::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	//if (m_bRender)
	//{
	//	if (FAILED(m_pVIBufferCom->Render()))
	//		return E_FAIL;
	//}

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CMinimapTest::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MinimapTest"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CMinimapTest::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMinimapTest * CMinimapTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMinimapTest*		pInstance = new CMinimapTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMinimapTest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMinimapTest::Clone(void * pArg)
{
	CMinimapTest*		pInstance = new CMinimapTest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMinimapTest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMinimapTest::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
