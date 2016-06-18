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

	//FbxMatrix로 변경
	XMFLOAT4X4 *m_pBaseBoneMatrix;		//애니메이션 BaseMatrix[인덱스]
	XMFLOAT4X4 **m_pAnimationMatrix;	//애니메이션 Matrix[시간][인덱스]
	XMFLOAT4X4 **m_ppResultMatrix;

	long long m_llAnimationMaxTime;			//애니메이션 최대길이
	unsigned int m_uiAnimationNodeIndexCount;	//애니메이션 노드 갯수
	float m_fAnimationPlayTime;				//애니메이션 재생 시간

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
	void _WriteMinMaxPos();			// _info 맨 아래에 Min/Max 추가
	void _SetBoneMatrix(FbxNode*);
	void _SetAnimationData(FbxNode*);
	void _WriteWeight();			// _weight
	void _SetAnimationMatrix(FbxNode* pNode, FbxAnimStack *pFbxAS);
	void _WriteAnimationMatrix();	// _matrix
};



void Warning(char* pWarningText);


#endif