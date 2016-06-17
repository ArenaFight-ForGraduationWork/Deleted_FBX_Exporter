#ifndef MESH_H_
#define MESH_H_



class CDiffusedVertex
{
private : 
	D3DXVECTOR3 m_d3dxvPosition;
	//������ ������ ��Ÿ���� ��� ����(D3DXCOLOR ����ü)�� �����Ѵ�. 
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//�����ڿ� �Ҹ��ڸ� �����Ѵ�.
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

		//���� �迭�� ����� ù��° �ε����� �� ���Ѵ�.
		BoneWeightArr[0] += Weight;
	}
};







class CMesh
{
public:
	//CMesh Ŭ������ �����ڿ� �Ҹ��ڸ� �����Ѵ�. 
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

	//����
	long long m_AnimationMaxTime = 0;
	XMFLOAT4X4** m_pResultMatrix;

	//���������մϴ�.
	long long m_AniMaxTime[5];
	XMFLOAT4X4** m_ppResult[5];
	int m_AnimationIndexCount;
private:
	int m_nReferences;
public:
	//�����Ͷ����� �����Ͷ����� ���� ��ü�� ���� �������̽� �����͸� �����Ѵ�.
	ID3D11RasterizerState *m_pd3dRasterizerState;
	void AddRef();
	void Release();
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);

protected:
	/*�ε��� ����(�ε����� �迭)�� ���� �������̽� �������̴�. �ε��� ���۴� ���� ����(�迭)�� ���� �ε����� ������.*/
	ID3D11Buffer *m_pd3dIndexBuffer;
	//�ε��� ���۰� �����ϴ� �ε����� �����̴�. 
	UINT m_nIndices;
	//�ε��� ���ۿ��� �޽��� ǥ���ϱ� ���� ���Ǵ� ���� �ε����̴�. 
	UINT m_nStartIndex;
	//�� �ε����� ������ �ε����̴�. 
	int m_nBaseVertex;

	//���� �����͸� �����ϱ� ���� ���� ���� �������̽� �����͸� �����Ѵ�.
	ID3D11Buffer *m_pd3dVertexBuffer;

	/*���� ������ ���� ����, ������ ����Ʈ ��, ���� �����Ͱ� ���� ������ ��𿡼����� �����ϴ� ���� ��Ÿ���� ������ �����Ѵ�.*/
	UINT m_nVertices;
	UINT m_nStride;
	UINT m_nOffset;

	//���� �����Ͱ� � ������Ƽ�긦 ǥ���ϰ� �ִ� ���� ��Ÿ���� ��� ������ �����Ѵ�.
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	//���� �����͸� �������ϴ� ��� �Լ��� �����Ѵ�.
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

	//���������մϴ�. ����
	CMyAni(ID3D11Device *pd3dDevice, int CharNum, int StateCnt);

	virtual ~CMyAni();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

private:
	vector<CAnimationVertex*> pVertices;
	CAnimationVertex* ppVertices;
};




#endif