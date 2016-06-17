#ifndef MESH_H_
#define MESH_H_



class CDiffusedVertex
{
private : 
	D3DXVECTOR3 m_d3dxvPosition;
	//정점의 색상을 나타내는 멤버 변수(D3DXCOLOR 구조체)를 선언한다. 
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//생성자와 소멸자를 선언한다.
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }
};

class CTexturedNormalVertex
{
public:
	D3DXVECTOR3 m_d3dxvPosition;
	D3DXVECTOR3 m_d3dxvNormal;
	D3DXVECTOR2 m_d3dxvTexCoord;

	CTexturedNormalVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxvNormal = D3DXVECTOR3(nx, ny, nz); m_d3dxvTexCoord = D3DXVECTOR2(u, v); }
	CTexturedNormalVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR3 d3dxvNormal, D3DXVECTOR2 d3dxvTexCoord) { m_d3dxvPosition = d3dxvPosition; m_d3dxvNormal = d3dxvNormal; m_d3dxvTexCoord = d3dxvTexCoord; }
	CTexturedNormalVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxvTexCoord = D3DXVECTOR2(0.0f, 0.0f); }
	~CTexturedNormalVertex() { }
};

class CAnimationVertex
{
public:
	D3DXVECTOR3 m_d3dxvPosition;
	D3DXVECTOR3 m_d3dxvNormal;
	D3DXVECTOR2 m_d3dxvTexCoord;
	
	int BoneIndexArr[8];
	float BoneWeightArr[8];

	CAnimationVertex()
	{
		m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_d3dxvNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 8; ++i)
		{
			BoneIndexArr[i] = 0;
			BoneWeightArr[i] = -1.0f;
		}
	}
	CAnimationVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR3 d3dxvNormal)
	{
		m_d3dxvPosition = d3dxvPosition;
		m_d3dxvNormal = d3dxvNormal;
	}
	CAnimationVertex(D3DXVECTOR3 d3dxvPosition, D3DXVECTOR3 d3dxvNormal, D3DXVECTOR2 d3dxvUV)
	{
		m_d3dxvPosition = d3dxvPosition;
		m_d3dxvNormal = d3dxvNormal;
		m_d3dxvTexCoord = d3dxvUV;
	}
	~CAnimationVertex(){}

	void AddBone(int index, float Weight)
	{
		for (int i = 0; i < 8; ++i)
		{
			if (BoneWeightArr[i] <= 0.0f)
			{
				BoneIndexArr[i] = index;
				BoneWeightArr[i] = Weight;
				return;
			}
		}

		//만약 배열을 벗어나면 첫번째 인덱스에 다 더한다.
		BoneWeightArr[0] += Weight;
	}
};







class CMesh
{
public:
	//CMesh 클래스의 생성자와 소멸자를 선언한다. 
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

	//여기
	long long m_AnimationMaxTime = 0;
	XMFLOAT4X4** m_pResultMatrix;

	//수정시작합니다.
	long long m_AniMaxTime[5];
	XMFLOAT4X4** m_ppResult[5];
	int m_AnimationIndexCount;
private:
	int m_nReferences;
public:
	//래스터라이저 래스터라이저 상태 객체에 대한 인터페이스 포인터를 선언한다.
	ID3D11RasterizerState *m_pd3dRasterizerState;
	void AddRef();
	void Release();
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);

protected:
	/*인덱스 버퍼(인덱스의 배열)에 대한 인터페이스 포인터이다. 인덱스 버퍼는 정점 버퍼(배열)에 대한 인덱스를 가진다.*/
	ID3D11Buffer *m_pd3dIndexBuffer;
	//인덱스 버퍼가 포함하는 인덱스의 개수이다. 
	UINT m_nIndices;
	//인덱스 버퍼에서 메쉬를 표현하기 위해 사용되는 시작 인덱스이다. 
	UINT m_nStartIndex;
	//각 인덱스에 더해질 인덱스이다. 
	int m_nBaseVertex;

	//정점 데이터를 저장하기 위한 정점 버퍼 인터페이스 포인터를 선언한다.
	ID3D11Buffer *m_pd3dVertexBuffer;

	/*정점 버퍼의 정점 개수, 정점의 바이트 수, 정점 데이터가 정점 버퍼의 어디에서부터 시작하는 가를 나타내는 변수를 선언한다.*/
	UINT m_nVertices;
	UINT m_nStride;
	UINT m_nOffset;

	//정점 데이터가 어떤 프리미티브를 표현하고 있는 가를 나타내는 멤버 변수를 선언한다.
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	//정점 데이터를 렌더링하는 멤버 함수를 선언한다.
public:
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};





class CMyModel : public CMesh
{
public:
	CMyModel(ID3D11Device *pd3dDevice, char* pFbxName, char* pTxtName, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	virtual ~CMyModel();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

private:
	std::vector<CTexturedNormalVertex*> pVertices;
	CTexturedNormalVertex* ppVertices;
};

class CMyAni : public CMesh
{
public:
	CMyAni(ID3D11Device *pd3dDevice, char* pFbxName, char* pTxtName);

	//수정시작합니다. 한줄
	CMyAni(ID3D11Device *pd3dDevice, int CharNum, int StateCnt);

	virtual ~CMyAni();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

private:
	vector<CAnimationVertex*> pVertices;
	CAnimationVertex* ppVertices;
};




#endif