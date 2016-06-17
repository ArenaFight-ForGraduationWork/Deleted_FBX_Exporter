#pragma once

#include "Mesh.h"


class CFbx
{
private:
	FbxNode* m_pRoot;

	FbxManager* m_pFbxManager;
	FbxScene* m_pFbxScene;

	map<unsigned int, vector<unsigned int>> m_VertexByIndex;
	std::map<std::string, unsigned int> m_IndexByName;

	XMFLOAT4X4 *m_pBaseBoneMatrix;		//애니메이션 BaseMatrix[인덱스]
	XMFLOAT4X4 **m_pAnimationMatrix;		//애니메이션 Matrix[시간][인덱스]

	XMFLOAT4X4 **m_ppResultMatrix;
	FbxAnimStack* m_AnimStack;

	long long m_llAnimationMaxTime;			//애니메이션 최대길이
	unsigned int m_uiAnimationNodeIndexCount;	//애니메이션 노드 갯수
	float m_fAnimationPlayTime;				//애니메이션 재생 시간

	FILE *fp;

	int temp = 0;
	int size = 0;

	D3DXVECTOR3 m_MaxVer;
	D3DXVECTOR3 m_MinVer;

	FbxAMatrix GetGeometryTransformation(FbxNode* inNode);

	void Fbx_AnimationParsing(FbxNode* pNode, FbxAnimStack *pFbxAS);

public:
	CFbx();
	~CFbx();

	FbxNode* GetRoot() { return m_pRoot; }
	int GetSize() { return size; }

	void FbxModel_Initialize(char* pFileName);

	void Fbx_VertexParsing(D3DXVECTOR3 d3dxvScale, char* txtName);			// _info
	void FindMinMaxVerPos();		// _info 맨 아래에 Min/Max 추가

	void SetFbxBoneMatrix(FbxNode*);
	void Fbx_AnimationVertexParsing(FbxNode*, CAnimationVertex* v);
	void WriteWeight(CAnimationVertex* v);		// _weight
	void Fbx_WriteTextFile_Animation();			// _matrix
};

