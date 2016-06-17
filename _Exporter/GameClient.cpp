#include "stdafx.h"
#include "Fbx.h"



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ int nCmdShow)
{
	CFbx *pFbx = new CFbx();

	//// 애니메이션 없음
	//pFbx->FbxModel_Initialize("Data\\Grass\\grass.fbx");
	//pFbx->Fbx_VertexParsing(D3DXVECTOR3(1, 1, 1), "Data\\Brock_Info.txt");

	// 애니메이션 있음
	//pFbx->FbxModel_Initialize("Data\\monster\\mst@attack.FBX");
	pFbx->FbxModel_Initialize("Data\\monster\\mst@dash.FBX");
	//pFbx->FbxModel_Initialize("Data\\monster\\mst@dead.FBX");
	//pFbx->FbxModel_Initialize("Data\\monster\\mst@idle.FBX");
	//pFbx->FbxModel_Initialize("Data\\monster\\mst@jump.FBX");
	//pFbx->FbxModel_Initialize("Data\\monster\\mst@jumpjump.FBX");
	//pFbx->FbxModel_Initialize("Data\\monster\\mst@move.FBX");

	pFbx->Fbx_VertexParsing(D3DXVECTOR3(1, 1, 1), "Data\\MainCharacter_Info.txt");	// _info
	pFbx->FindMinMaxVerPos();	// _info에 min/max 덮어쓰기(추가)

	pFbx->SetFbxBoneMatrix(pFbx->GetRoot());
	int m_nVertices = pFbx->GetSize();
	CAnimationVertex *v = new CAnimationVertex[m_nVertices];
	pFbx->Fbx_AnimationVertexParsing(pFbx->GetRoot(), v);

	pFbx->WriteWeight(v);	// _weight
	pFbx->Fbx_WriteTextFile_Animation();	// _matrix

	return 0;
}
