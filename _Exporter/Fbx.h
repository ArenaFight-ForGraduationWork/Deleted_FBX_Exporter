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

	XMFLOAT4X4 *m_pBaseBoneMatrix;		//�ִϸ��̼� BaseMatrix[�ε���]
	XMFLOAT4X4 **m_pAnimationMatrix;		//�ִϸ��̼� Matrix[�ð�][�ε���]

	XMFLOAT4X4 **m_ppResultMatrix;
	FbxAnimStack* m_AnimStack;

	long long m_llAnimationMaxTime;			//�ִϸ��̼� �ִ����
	unsigned int m_uiAnimationNodeIndexCount;	//�ִϸ��̼� ��� ����
	float m_fAnimationPlayTime;				//�ִϸ��̼� ��� �ð�

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
	void FindMinMaxVerPos();		// _info �� �Ʒ��� Min/Max �߰�

	void SetFbxBoneMatrix(FbxNode*);
	void Fbx_AnimationVertexParsing(FbxNode*, CAnimationVertex* v);
	void WriteWeight(CAnimationVertex* v);		// _weight
	void Fbx_WriteTextFile_Animation();			// _matrix
};

