#include "stdafx.h"
#include "Fbx.h"



CFbx::CFbx()
{
	m_fAnimationPlayTime = 0.0f;
	m_llAnimationMaxTime = 0;
	m_pAnimationMatrix = nullptr;

	m_pBaseBoneMatrix = new XMFLOAT4X4[128];
	for (int i = 0; i < 128; ++i)
	{
		XMStoreFloat4x4(&m_pBaseBoneMatrix[i], XMMatrixIdentity());
	}

	m_ppResultMatrix = nullptr;
}
CFbx::~CFbx()
{
	m_ppResultMatrix = nullptr;

	if (m_pBaseBoneMatrix)
		delete[] m_pBaseBoneMatrix;
}

FbxAMatrix CFbx::GetGeometryTransformation(FbxNode* inNode)
{
	if (!inNode)
	{
		throw std::exception("Null for mesh geometry");
	}

	const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

void CFbx::FbxModel_Initialize(char* pFileName)
{
	m_pFbxManager = FbxManager::Create();
	FbxIOSettings* pFbxIOsetting = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
	m_pFbxManager->SetIOSettings(pFbxIOsetting);
	m_pFbxScene = FbxScene::Create(m_pFbxManager, "");
	FbxImporter* pImporter = FbxImporter::Create(m_pFbxManager, "");

	if (!pImporter->Initialize(pFileName, -1, m_pFbxManager->GetIOSettings()))
	{
		cout << "FBXimporter Initialize fail" << endl;
		cout << "파일이 지정한 경로나 위치에 없을 수도 있습니다. " << endl;
	}

	if (!pImporter->Import(m_pFbxScene))
	{
		cout << "FBX importer Import Scene fail" << endl;
	}

	FbxAxisSystem CurrAxisSystem = m_pFbxScene->GetGlobalSettings().GetAxisSystem();	//현재 축 형태
	FbxAxisSystem DestAxisSystem = FbxAxisSystem::eMayaYUp;								//원하는 축 형태
	if (CurrAxisSystem != DestAxisSystem) DestAxisSystem.ConvertScene(m_pFbxScene);

	FbxGeometryConverter lGeomConverter(m_pFbxManager);
	if (!lGeomConverter.Triangulate(m_pFbxScene, true))
	{
		cout << "Mesh transform fail" << endl;
	}

	pImporter->Destroy();

	m_pRoot = m_pFbxScene->GetRootNode();
}

void CFbx::Fbx_VertexParsing(D3DXVECTOR3 d3dxvScale, char* txtName)
{
	fopen_s(&fp, txtName, "wt");

	for (int i = 0; i < m_pRoot->GetChildCount(); ++i)
	{
		FbxNode* pFbxChildNode = m_pRoot->GetChild(i);
		if (pFbxChildNode->GetNodeAttribute() == NULL)
			continue;
		FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();
		if (AttributeType != FbxNodeAttribute::eMesh)
			continue;

		FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

		const unsigned int ctrlPointCnt = pMesh->GetControlPointsCount();
		const unsigned int polygonCnt = pMesh->GetPolygonCount();
		const unsigned int polygonVertexCnt = pMesh->GetPolygonVertexCount();
		size = polygonVertexCnt;
		fprintf(fp, "%d\n", polygonVertexCnt);

		D3DXVECTOR3 outPos;
		D3DXVECTOR3 outNormal;
		D3DXVECTOR3 outUV;
		FbxVector4 outFbxNormal;

		FbxVector4* mControlPoint = pMesh->GetControlPoints();
		FbxGeometryElementUV* vertexUV = pMesh->GetElementUV(0);
		FbxGeometryElementNormal* vertexNormal = pMesh->GetElementNormal(0);

		//정점 데이터 얻기
		for (int j = 0; j < pMesh->GetPolygonCount(); j++)		//ex) 손의 컨트롤 포인트만 가져와서
		{
			int iNumVertieces = pMesh->GetPolygonSize(j);
			if (iNumVertieces != 3)
			{
				cout << "폴리곤이 삼각형이 아님" << endl;
			}
			else //3개로 잘 나뉘어있으면
			{
				for (int k = 0; k < iNumVertieces; k++)
				{
					int iPolygonVertexNum = pMesh->GetPolygonVertex(j, k);
					int iNormalIndex = pMesh->GetPolygonVertexNormal(j, k, outFbxNormal);
					int iTextureUVIndex = pMesh->GetTextureUVIndex(j, k);

					// position
					outPos.x = (float)mControlPoint[iPolygonVertexNum].mData[0] * d3dxvScale.x;
					outPos.y = (float)mControlPoint[iPolygonVertexNum].mData[1] * d3dxvScale.y;
					outPos.z = (float)mControlPoint[iPolygonVertexNum].mData[2] * d3dxvScale.z;

					// normal
					outNormal.x = outFbxNormal.mData[0];
					outNormal.y = outFbxNormal.mData[1];
					outNormal.z = outFbxNormal.mData[2];

					// uv
					switch (vertexUV->GetMappingMode())
					{
					case FbxGeometryElement::eByControlPoint:
						switch (vertexUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						{
							outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(iPolygonVertexNum).mData[0]);
							outUV.y = 1 - static_cast<float>(vertexUV->GetDirectArray().GetAt(iPolygonVertexNum).mData[1]);
						}break;
						case FbxGeometryElement::eIndexToDirect:
						{
							int index = vertexUV->GetIndexArray().GetAt(iPolygonVertexNum);
							outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
							outUV.y = 1 - static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
						}break;
						default:
							throw std::exception("Invalid Reference");
						}break;
					case FbxGeometryElement::eByPolygonVertex:
						switch (vertexUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
						{
							outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(iTextureUVIndex).mData[0]);
							outUV.y = 1 - static_cast<float>(vertexUV->GetDirectArray().GetAt(iTextureUVIndex).mData[1]);
						}break;
						default:
							throw std::exception("Invalid Reference");
						}break;
					default:
						throw std::exception("Invalid Reference");
					}

					fprintf(fp, "%f %f %f\n", outPos.x, outPos.y, outPos.z);
					fprintf(fp, "%f %f %f\n", outNormal.x, outNormal.y, outNormal.z);
					fprintf(fp, "%f %f\n", outUV.x, outUV.y);
				}
			}
		}
	}

	fclose(fp);
}

void CFbx::Fbx_AnimationVertexParsing(FbxNode* pNode, CAnimationVertex* v)
{
	FbxNodeAttribute *pFbxNodeAttribute = pNode->GetNodeAttribute();
	if (pFbxNodeAttribute != nullptr && pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* pMesh = pNode->GetMesh();
		const unsigned int IndexCount = pMesh->GetPolygonVertexCount();	//const unsigned int하면 안되던데 왤까
		cout << "PolygonVertexCnt:   " << IndexCount << endl;
		temp = IndexCount;

		int *IndexArr = new int[IndexCount];

		for (int i = 0; i < IndexCount; ++i)
		{
			IndexArr[i] = pMesh->GetPolygonVertices()[i];
			m_VertexByIndex[IndexArr[i]].push_back(i);
		}
		/*----------------------------------------------------------*/

		FbxAMatrix geometryTransform = GetGeometryTransformation(pNode);

		FbxGeometry *pGeo = pNode->GetGeometry();
		int SkinCount = pGeo->GetDeformerCount(FbxDeformer::eSkin);

		for (int i = 0; i < SkinCount; ++i)
		{
			FbxSkin* pSkin = (FbxSkin*)pGeo->GetDeformer(i, FbxDeformer::eSkin);
			int ClusterCount = pSkin->GetClusterCount();

			for (int j = 0; j < ClusterCount; ++j)
			{
				FbxCluster *pCluster = pSkin->GetCluster(j);
				int ClusterIndexCount = pCluster->GetControlPointIndicesCount();
				int *ClusterIndices = pCluster->GetControlPointIndices();
				double *ClusterWeights = pCluster->GetControlPointWeights();

				for (int k = 0; k < ClusterIndexCount; k++)
				{

					std::string BoneName = std::string(pCluster->GetLink()->GetName());
					int INDEX = m_IndexByName[BoneName];

					FbxAMatrix transformMatrix;
					FbxAMatrix transformLinkMatrix;
					FbxAMatrix ResultMtx;

					pCluster->GetTransformMatrix(transformMatrix); // The transformation of the mesh at binding time
					pCluster->GetTransformLinkMatrix(transformLinkMatrix); // The transformation of the cluster(joint) at binding time from joint space to world space
					ResultMtx = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

					for (int m = 0; m < 4; m++)
					{
						for (int n = 0; n < 4; n++)
						{
							//오류가 났던건 매트릭스 초기화를 안해줘서!!! 생성자에서 초기화 시켜줬다..
							m_pBaseBoneMatrix[INDEX].m[m][n] = ResultMtx.Get(m, n);
						}
					}

					float BoneWeight = (float)ClusterWeights[k];
					int BoneIndex = ClusterIndices[k];

					for (auto iter : m_VertexByIndex[BoneIndex])
					{
						if (INDEX == 0 || INDEX == -1)
						{

						}
						else
						{
							v[iter].AddBone(INDEX, BoneWeight);
						}
					}
				}

			}
		}

		delete[]IndexArr;
	}


	int nNodeChilde = pNode->GetChildCount();
	for (int i = 0; i < nNodeChilde; ++i)
	{
		FbxNode* pChildNode = pNode->GetChild(i);
		Fbx_AnimationVertexParsing(pChildNode, v);
	}
}

void CFbx::Fbx_AnimationParsing(FbxNode *pNode, FbxAnimStack *FbxAS)
{
	if (pNode != nullptr)
	{
		unsigned int BoneIndex = m_IndexByName[pNode->GetName()];	//m_IndexByName.size();

		FbxTime maxTime = FbxAS->GetLocalTimeSpan().GetDuration();
		for (long long i = 0; i < maxTime.GetMilliSeconds() / 10; ++i)
		{
			FbxTime n_time;
			n_time.SetMilliSeconds(i * 10);

			for (int m = 0; m < 4; ++m)
			{
				for (int n = 0; n < 4; ++n)
				{
					m_pAnimationMatrix[i][BoneIndex].m[m][n] = (pNode->EvaluateGlobalTransform(n_time)).Get(m, n);
				}
			}

			XMFLOAT4X4 pp = m_pAnimationMatrix[i][BoneIndex];
			int a = 1 + 1;
		}
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		Fbx_AnimationParsing(pNode->GetChild(i), FbxAS);
	}
}

void CFbx::FindMinMaxVerPos()
{
	float x = 0;
	float y = 0;
	float z = 0;

	fopen_s(&fp, "Data\\MainCharacter_Info.txt", "a+");

	for (int i = 0; i < m_pRoot->GetChildCount(); ++i)
	{
		FbxNode* pFbxChildNode = m_pRoot->GetChild(i);
		if (pFbxChildNode->GetNodeAttribute() == NULL)
			continue;
		FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();
		if (AttributeType != FbxNodeAttribute::eMesh)
			continue;
		FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

		FbxVector4* mControlPoints = pMesh->GetControlPoints();
		//max
		for (int j = 0; j < pMesh->GetControlPointsCount(); ++j)
		{
			if (x < (float)mControlPoints[j].mData[0])
				x = (float)mControlPoints[j].mData[0];
			if (y < (float)mControlPoints[j].mData[1])
				y = (float)mControlPoints[j].mData[1];
			if (z < (float)mControlPoints[j].mData[2])
				z = (float)mControlPoints[j].mData[2];
		}
		m_MaxVer = D3DXVECTOR3(x, z, y);

		m_MaxVer.x *= 1;
		m_MaxVer.y *= 1;
		m_MaxVer.z *= 1;

		fprintf(fp, "%f %f %f\n", m_MaxVer.x, m_MaxVer.y, m_MaxVer.z);

		x, y, z = 0;
		for (int j = 0; j < pMesh->GetControlPointsCount(); ++j)
		{
			if (x > (float)mControlPoints[j].mData[0])
				x = (float)mControlPoints[j].mData[0];
			if (y > (float)mControlPoints[j].mData[1])
				y = (float)mControlPoints[j].mData[1];
			if (z > (float)mControlPoints[j].mData[2])
				z = (float)mControlPoints[j].mData[2];
		}
		m_MinVer = D3DXVECTOR3(x, z, y);
		m_MinVer.x *= 1;
		m_MinVer.y *= 1;
		m_MinVer.z *= 1;

		fprintf(fp, "%f %f %f\n", m_MinVer.x, m_MinVer.y, m_MinVer.z);
	}

	fclose(fp);
}

void CFbx::WriteWeight(CAnimationVertex* v)
{
	fopen_s(&fp, "Data\\MainCharacter_Weight.txt", "wt");

	for (int i = 0; i < temp; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			fprintf(fp, "%d %d %f\n", i, v[i].BoneIndexArr[j], v[i].BoneWeightArr[j]);
		}
	}

	fclose(fp);
}

void CFbx::Fbx_WriteTextFile_Animation()
{
	FbxAnimStack* AnimStack = m_pFbxScene->GetSrcObject<FbxAnimStack>();

	fopen_s(&fp, "Data\\MainCharacter_matrix.txt", "wt");

	if (AnimStack != nullptr)
	{
		//애니메이션 데이터 불러오기

		//애니메이션 최대길이
		m_llAnimationMaxTime = AnimStack->GetLocalTimeSpan().GetDuration().GetMilliSeconds();
		cout << "애니메이션 최대길이 : " << m_llAnimationMaxTime << endl;

		//애니메이션에 영향을 받는 animation Node 개수
		m_uiAnimationNodeIndexCount = m_IndexByName.size();

		//애니메이션 2차원 배열 생성
		m_pAnimationMatrix = new XMFLOAT4X4*[m_llAnimationMaxTime / 10];	//최대시간/10만큼 애니메이션행렬 배열 할당
		m_ppResultMatrix = new XMFLOAT4X4*[m_llAnimationMaxTime / 10];		//최대시간/10만큼 애니메이션 최종 변환행렬 배열 할당

		for (long long i = 0; i < m_llAnimationMaxTime / 10; ++i)
		{
			m_pAnimationMatrix[i] = new XMFLOAT4X4[m_uiAnimationNodeIndexCount];	//i번째 시간대 : 배열에 애니메이션 노드 개수만큼 배열 할당
			m_ppResultMatrix[i] = new XMFLOAT4X4[m_uiAnimationNodeIndexCount];		//i번째 시간대 : 최종변환행렬에 애니메이션 노드 개수만큼 배열 할당
		}

		//Animation Matrix채우기
		Fbx_AnimationParsing(m_pRoot, AnimStack);

		fprintf(fp, "%lld %d\n", m_llAnimationMaxTime, m_uiAnimationNodeIndexCount);

		for (long long i = 0; i < m_llAnimationMaxTime / 10; ++i)
		{
			for (unsigned int j = 0; j < m_uiAnimationNodeIndexCount; ++j)
			{
				XMMATRIX BaseBoneMatrixTEMP = XMLoadFloat4x4(&m_pBaseBoneMatrix[j]);
				XMMATRIX AnimationMatrixTEMP = XMLoadFloat4x4(&m_pAnimationMatrix[i][j]);

				XMStoreFloat4x4(&m_ppResultMatrix[i][j], (BaseBoneMatrixTEMP * AnimationMatrixTEMP));
				for (int k = 0; k < 4; ++k)
				{
					for (int l = 0; l < 4; ++l)
					{
						fprintf(fp, "%f\n", m_ppResultMatrix[i][j](k, l));
					}
				}
			}
		}
	}

	fclose(fp);
}

void CFbx::SetFbxBoneMatrix(FbxNode* pNode)
{
	m_IndexByName[pNode->GetName()] = m_IndexByName.size();
	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		this->SetFbxBoneMatrix(pNode->GetChild(i));
	}
}


