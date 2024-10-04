#pragma once

#include "Base.h"

BEGIN(Engine)
class CLight;
/* 빛들을 보관한다. */

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
public:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iMaxLevel);


public:
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);


public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_ShadowLightViewMatrix(_uint iLevel, _float4x4 mMatrix);
	_float4x4 Get_ShadowLightViewMatrix(_uint iLevel);

	LIGHTDESC* Get_DirLightDesc();
	class CLight* Get_DirLight() { return m_DirLight; }

	list<class CLight*>* Get_Lights() { return &m_Lights; }

	void Set_RenderLight(_bool bRenderLight) { m_bRenderLight = bRenderLight; }
	_bool Get_RenderLight() { return m_bRenderLight; }

public:
	HRESULT Add_Light(const LIGHTDESC& LightDesc, CLight** pOut = nullptr);
	HRESULT Delete_Light(_uint iIndex);
	HRESULT Delete_Light(CLight* pLight);
	HRESULT Reset_Lights();



private:
	list<class CLight*>			m_Lights;
	class CLight*				m_DirLight = nullptr;
	typedef list<class CLight*>	LIGHTS;


	_float4x4*		m_mShadowLightViewMatrixs = nullptr;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	_bool m_bRenderLight = false;


public:
	virtual void Free() override;



};

END