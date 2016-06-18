#ifndef FBX_H_
#define FBX_H_

#include "Vertex.h"



class CFbx
{
public:
	CFbx();
	~CFbx();

	void Import(char* pFileName, char* pTxtName, char* pTxtNameAfterMatrix = nullptr);

private:
	FbxScene* m_pFbxScene;
	FbxNode* m_pRoot;

	map<unsigned int, vector<unsigned int>> m_VertexByIndex;
	std::map<std::string, unsigned int> m_IndexByName;

	//FbxMatrix�� ����
	XMFLOAT4X4 *m_pBaseBoneMatrix;		//�ִϸ��̼� BaseMatrix[�ε���]
	XMFLOAT4X4 **m_pAnimationMatrix;	//�ִϸ��̼� Matrix[�ð�][�ε���]
	XMFLOAT4X4 **m_ppResultMatrix;

	long long m_llAnimationMaxTime;			//�ִϸ��̼� �ִ����
	unsigned int m_uiAnimationNodeIndexCount;	//�ִϸ��̼� ��� ����
	float m_fAnimationPlayTime;				//�ִϸ��̼� ��� �ð�

	int temp;
	int m_iSize;

	char *m_pTxtName;
	char *m_pTxtNameAfterMatrix;

	FbxVector4 m_MaxVer;
	FbxVector4 m_MinVer;

	CAnimationVertex *m_pVertices;

	FbxAMatrix _GetGeometryTransformation(FbxNode* inNode);

	void _Initialize(char* pFileName);
	void _WriteVertex();			// _info
	void _WriteMinMaxPos();			// _info �� �Ʒ��� Min/Max �߰�
	void _SetBoneMatrix(FbxNode*);
	void _SetAnimationData(FbxNode*);
	void _WriteWeight();			// _weight
	void _SetAnimationMatrix(FbxNode* pNode, FbxAnimStack *pFbxAS);
	void _WriteAnimationMatrix();	// _matrix
};



void Warning(char* pWarningText);


#endif