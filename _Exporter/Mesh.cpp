#include "stdafx.h"
#include "Mesh.h"
#include "Fbx.h"



CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nReferences = 1;
	m_pd3dRasterizerState = NULL;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;

	m_pResultMatrix = nullptr;
	for (int i = 0; i < 5; ++i)
		m_ppResult[i] = nullptr;
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
}

void CMesh::AddRef()
{
	m_nReferences++;
}

void CMesh::Release()
{
	m_nReferences--;
	if (m_nReferences == 0) delete this;
}

void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pd3dVertexBuffer) pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer, &m_nStride, &m_nOffset);
	//인덱스 버퍼가 있으면 인덱스 버퍼를 디바이스 컨텍스트에 연결한다.
	if (m_pd3dIndexBuffer) pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pd3dDeviceContext->IASetPrimitiveTopology(
		m_d3dPrimitiveTopology);
	if (m_pd3dRasterizerState) pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	// 인덱스 버퍼가 있으면 인덱스 버퍼를 사용하여 메쉬를 렌더링하고
	// 없으면 정점 버퍼만을 사용하여 메쉬를 렌더링한다.
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nOffset);
}

void CMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
}







CMyModel::CMyModel(ID3D11Device *pd3dDevice, char* pFbxName, char* ptxtName, D3DXVECTOR3 d3dxvScale) : CMesh(pd3dDevice)
{
	//CFbx::GetInstance()->Fbx_ReadTextFile_Mesh(ptxtName, pVertices);

	m_nVertices = pVertices.size();
	std::cout << "사이즈: " << m_nVertices << std::endl;

	ppVertices = new CTexturedNormalVertex[m_nVertices];

	for (int i = 0; i < pVertices.size(); ++i)
	{
		ppVertices[i].m_d3dxvPosition = pVertices.at(i)->m_d3dxvPosition;
		ppVertices[i].m_d3dxvNormal = pVertices.at(i)->m_d3dxvNormal;
		ppVertices[i].m_d3dxvTexCoord = pVertices.at(i)->m_d3dxvTexCoord;
	}

	m_nStride = sizeof(CTexturedNormalVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = ppVertices;

	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);

	CreateRasterizerState(pd3dDevice);

	delete[] ppVertices;
}

CMyModel::~CMyModel()
{
	CMesh::~CMesh();
}

void CMyModel::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CMyModel::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}






///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
CMyAni::CMyAni(ID3D11Device *pd3dDevice, char* pFbxName, char* pTxtName) : CMesh(pd3dDevice)
{
	//CFbx::GetInstance()->Fbx_ReadTextFile_Info(pTxtName);
	//m_nVertices = CFbx::GetInstance()->GetSize();
	//ppVertices = new CAnimationVertex[m_nVertices];

	//CFbx::GetInstance()->LoadMesh(ppVertices);
	//m_AnimationMaxTime = CFbx::GetInstance()->GetAnimationMaxTime();
	//m_pResultMatrix = CFbx::GetInstance()->GetResultMatrix();


	m_nStride = sizeof(CAnimationVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = ppVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);
	CreateRasterizerState(pd3dDevice);
}


//수정시작합니다. 
CMyAni::CMyAni(ID3D11Device *pd3dDevice, int CharNum, int StateCnt) : CMesh(pd3dDevice)
{
	//CFbx::GetInstance()->Fbx_ReadTextFile_Info(CharNum);
	//m_nVertices = CFbx::GetInstance()->GetSize();

	//ppVertices = new CAnimationVertex[m_nVertices];

	//CFbx::GetInstance()->Fbx_ReadTextFile_Mesh(CharNum, ppVertices);

	//for (int i = 0; i < StateCnt; ++i)
	//{
	//	CFbx::GetInstance()->Fbx_ReadTextFile_Ani(CharNum,i);
	//	m_ppResult[i] = CFbx::GetInstance()->GetResult(i);
	//	m_AniMaxTime[i] = CFbx::GetInstance()->GetTime();
	//	cout << i << "번째 maxtime*_*:" << m_AniMaxTime[i] << endl;
	//}
	//m_AnimationIndexCount = CFbx::GetInstance()->GetAnimationIndexCount();
	//CFbx::GetInstance()->Fbx_ReadTextFile_Weight(CharNum, ppVertices);

	m_nStride = sizeof(CAnimationVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = ppVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);
	CreateRasterizerState(pd3dDevice);
}

CMyAni::~CMyAni()
{
	CMesh::~CMesh();
}

void CMyAni::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}

void CMyAni::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}


