#include "stdafx.h"
#include "Fbx.h"

#define AnimationNodeCount 40	// �ִϸ��̼ǿ� ������ �޴� animation Node ����
								// m_uiAnimationNodeIndexCount = m_IndexByName.size();
								// _matrix���� �� ���� �ι�° ����



CFbx::CFbx()
{
	m_pFbxScene = nullptr;
	m_pRoot = nullptr;

	m_VertexByIndex.clear();
	m_IndexByName.clear();

	m_pBaseBoneMatrix = new XMFLOAT4X4[AnimationNodeCount];
	for (int i = 0; i < AnimationNodeCount; ++i)
	{
		XMStoreFloat4x4(&m_pBaseBoneMatrix[i], XMMatrixIdentity());
	}
	m_pAnimationMatrix = nullptr;
	m_ppResultMatrix = nullptr;

	m_llAnimationMaxTime = 0;
	m_uiAnimationNodeIndexCount = 0;
	m_fAnimationPlayTime = 0.0f;

	temp = 0;
	m_iSize = 0;

	m_pTxtName = nullptr;
	m_pTxtNameAfterMatrix = nullptr;

	m_MaxVer = FbxVector4(0, 0, 0, 0);
	m_MinVer = FbxVector4(0, 0, 0, 0);

	m_pVertices = nullptr;
}
CFbx::~CFbx()
{
}

void CFbx::Import(char* pFileName, char* pTxtName, char* pTxtNameAfterMatrix)
{
	m_pTxtName = pTxtName;
	m_pTxtNameAfterMatrix = pTxtNameAfterMatrix;

	_Initialize(pFileName);

	if (!m_pTxtNameAfterMatrix)
	{	// �ִϸ��̼��� ����
		_WriteVertex();
	}
	else
	{ // �ִϸ��̼��� ����
		_WriteVertex();	// _info
		_WriteMinMaxPos();	// _info�� min/max �����(�߰�)

		_SetBoneMatrix(m_pRoot);
		_SetAnimationData(m_pRoot);

		_WriteWeight();	// _weight
		_WriteAnimationMatrix();	// _matrix
	}
}



FbxAMatrix CFbx::_GetGeometryTransformation(FbxNode* inNode)
{
	if (!inNode)
	{
		//throw std::exception("Null for mesh geometry");
		Warning("Null for mesh geometry");
	}

	const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

void CFbx::_Initialize(char* pFileName)
{
	FbxManager* m_pFbxManager = FbxManager::Create();
	FbxIOSettings* pFbxIOsetting = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
	m_pFbxManager->SetIOSettings(pFbxIOsetting);
	m_pFbxScene = FbxScene::Create(m_pFbxManager, "");
	FbxImporter* pImporter = FbxImporter::Create(m_pFbxManager, "");

	if (!pImporter->Initialize(pFileName, -1, m_pFbxManager->GetIOSettings()))
	{
		Warning("FBX importer Initialize fail\n������ ������ ��γ� ��ġ�� ���� ���� �ֽ��ϴ�.");
	}

	if (!pImporter->Import(m_pFbxScene))
	{
		Warning("FBX importer Import Scene fail");
	}

	FbxAxisSystem CurrAxisSystem = m_pFbxScene->GetGlobalSettings().GetAxisSystem();	//���� �� ����
	FbxAxisSystem DestAxisSystem = FbxAxisSystem::eMayaYUp;								//���ϴ� �� ����
	if (CurrAxisSystem != DestAxisSystem) DestAxisSystem.ConvertScene(m_pFbxScene);

	FbxGeometryConverter lGeomConverter(m_pFbxManager);
	if (!lGeomConverter.Triangulate(m_pFbxScene, true))
	{
		Warning("Mesh transform fail");
	}

	pImporter->Destroy();

	m_pRoot = m_pFbxScene->GetRootNode();
}

void CFbx::_WriteVertex()
{
	FILE *fp;
	char pPath[50] = "Data\\";
	strcat_s(pPath, 50, m_pTxtName);
	strcat_s(pPath, 50, "_info.txt");
	fopen_s(&fp, pPath, "wt");

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
		m_iSize = polygonVertexCnt;
		m_pVertices = new CAnimationVertex[m_iSize];
		fprintf(fp, "%d\n", polygonVertexCnt);

		FbxVector4 outPos;
		FbxVector4 outNormal;
		FbxVector2 outUV;

		FbxVector4* mControlPoint = pMesh->GetControlPoints();
		FbxGeometryElementUV* vertexUV = pMesh->GetElementUV(0);
		FbxGeometryElementNormal* vertexNormal = pMesh->GetElementNormal(0);

		//���� ������ ���
		for (int j = 0; j < pMesh->GetPolygonCount(); j++)		//ex) ���� ��Ʈ�� ����Ʈ�� �����ͼ�
		{
			int iNumVertieces = pMesh->GetPolygonSize(j);
			if (iNumVertieces != 3)
			{
				Warning("�������� �ﰢ���� �ƴϴ�");
			}
			else //3���� �� ������������
			{
				for (int k = 0; k < iNumVertieces; k++)
				{
					int iPolygonVertexNum = pMesh->GetPolygonVertex(j, k);
					int iNormalIndex = pMesh->GetPolygonVertexNormal(j, k, outNormal);
					int iTextureUVIndex = pMesh->GetTextureUVIndex(j, k);

					// position
					outPos.mData[0] = mControlPoint[iPolygonVertexNum].mData[0];
					outPos.mData[1] = mControlPoint[iPolygonVertexNum].mData[1];
					outPos.mData[2] = mControlPoint[iPolygonVertexNum].mData[2];
					fprintf(fp, "%f %f %f\n", outPos.mData[0], outPos.mData[1], outPos.mData[2]);

					// normal
					fprintf(fp, "%f %f %f\n", outNormal.mData[0], outNormal.mData[1], outNormal.mData[2]);

					// uv
					switch (vertexUV->GetMappingMode())
					{
					case FbxGeometryElement::eByControlPoint:
						switch (vertexUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						{
							outUV.mData[0] = vertexUV->GetDirectArray().GetAt(iPolygonVertexNum).mData[0];
							outUV.mData[1] = 1 - vertexUV->GetDirectArray().GetAt(iPolygonVertexNum).mData[1];
						}break;
						case FbxGeometryElement::eIndexToDirect:
						{
							int index = vertexUV->GetIndexArray().GetAt(iPolygonVertexNum);
							outUV.mData[0] = vertexUV->GetDirectArray().GetAt(index).mData[0];
							outUV.mData[1] = 1 - vertexUV->GetDirectArray().GetAt(index).mData[1];
						}break;
						default:
							Warning("UV Invalid Reference");
						}break;
					case FbxGeometryElement::eByPolygonVertex:
						switch (vertexUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
						{
							outUV.mData[0] = vertexUV->GetDirectArray().GetAt(iTextureUVIndex).mData[0];
							outUV.mData[1] = 1 - vertexUV->GetDirectArray().GetAt(iTextureUVIndex).mData[1];
						}break;
						default:
							Warning("UV Invalid Reference");
						}break;
					default:
						Warning("UV Invalid Reference");
					}
					fprintf(fp, "%f %f\n", outUV.mData[0], outUV.mData[1]);
				}
			}
		}
	}

	fclose(fp);
}

void CFbx::_SetAnimationData(FbxNode* pNode)
{
	FbxNodeAttribute *pFbxNodeAttribute = pNode->GetNodeAttribute();
	if (pFbxNodeAttribute && pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* pMesh = pNode->GetMesh();
		const unsigned int IndexCount = pMesh->GetPolygonVertexCount();	//const unsigned int�ϸ� �ȵǴ��� �ͱ�
		temp = IndexCount;

		int *IndexArr = new int[IndexCount];

		for (unsigned int i = 0; i < IndexCount; ++i)
		{
			IndexArr[i] = pMesh->GetPolygonVertices()[i];
			m_VertexByIndex[IndexArr[i]].push_back(i);
		}

		FbxAMatrix geometryTransform = _GetGeometryTransformation(pNode);
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
							m_pBaseBoneMatrix[INDEX].m[m][n] = static_cast<float>(ResultMtx.Get(m, n));
						}
					}

					float BoneWeight = (float)ClusterWeights[k];
					int BoneIndex = ClusterIndices[k];

					for (auto iter : m_VertexByIndex[BoneIndex])
					{
						if (INDEX != 0 && INDEX != -1)
						{
							m_pVertices[iter].AddBone(INDEX, BoneWeight);
						}
					}
				}

			}
		}
		delete[] IndexArr;
	}

	int nNodeChild = pNode->GetChildCount();
	for (int i = 0; i < nNodeChild; ++i)
	{
		FbxNode* pChildNode = pNode->GetChild(i);
		_SetAnimationData(pChildNode);
	}
}

void CFbx::_SetAnimationMatrix(FbxNode *pNode, FbxAnimStack *FbxAS)
{
	if (pNode)
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
					m_pAnimationMatrix[i][BoneIndex].m[m][n] = static_cast<float>((pNode->EvaluateGlobalTransform(n_time)).Get(m, n));
				}
			}

			XMFLOAT4X4 pp = m_pAnimationMatrix[i][BoneIndex];
		}
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		_SetAnimationMatrix(pNode->GetChild(i), FbxAS);
	}
}

void CFbx::_WriteMinMaxPos()
{
	FILE *fp;
	char pPath[50] = "Data\\";
	strcat_s(pPath, 50, m_pTxtName);
	strcat_s(pPath, 50, "_info.txt");
	fopen_s(&fp, pPath, "a+");

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
			if (m_MaxVer.mData[0] < mControlPoints[j].mData[0])
				m_MaxVer.mData[0] = mControlPoints[j].mData[0];

			if (m_MaxVer.mData[1] < mControlPoints[j].mData[2])
				m_MaxVer.mData[1] = mControlPoints[j].mData[2];

			if (m_MaxVer.mData[2] < mControlPoints[j].mData[1])
				m_MaxVer.mData[2] = mControlPoints[j].mData[1];
		}
		fprintf(fp, "%f %f %f\n", m_MaxVer.mData[0], m_MaxVer.mData[1], m_MaxVer.mData[2]);

		// min
		for (int j = 0; j < pMesh->GetControlPointsCount(); ++j)
		{
			if (m_MinVer.mData[0] > mControlPoints[j].mData[0])
				m_MinVer.mData[0] = mControlPoints[j].mData[0];

			if (m_MinVer.mData[1] > mControlPoints[j].mData[2])
				m_MinVer.mData[1] = mControlPoints[j].mData[2];

			if (m_MinVer.mData[2] > mControlPoints[j].mData[1])
				m_MinVer.mData[2] = mControlPoints[j].mData[1];
		}
		fprintf(fp, "%f %f %f\n", m_MinVer.mData[0], m_MinVer.mData[1], m_MinVer.mData[2]);
	}

	fclose(fp);
}

void CFbx::_WriteWeight()
{
	FILE *fp;
	char pPath[50] = "Data\\";
	strcat_s(pPath, 50, m_pTxtName);
	strcat_s(pPath, 50, "_weight.txt");
	fopen_s(&fp, pPath, "wt");

	for (int i = 0; i < temp; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			fprintf(fp, "%d %d %f\n", i, m_pVertices[i].BoneIndexArr[j], m_pVertices[i].BoneWeightArr[j]);
		}
	}

	fclose(fp);
}

void CFbx::_WriteAnimationMatrix()
{
	if (!m_pTxtNameAfterMatrix)
	{
		Warning("_matrix ���� �ڿ� ���� �̸��� Initialize�Լ��� �� ������. �����");
	}

	FILE *fp;
	char pPath[50] = "Data\\";
	strcat_s(pPath, 50, m_pTxtName);
	strcat_s(pPath, 50, "_matrix_");
	strcat_s(pPath, 50, m_pTxtNameAfterMatrix);
	strcat_s(pPath, 50, ".txt");
	fopen_s(&fp, pPath, "wt");

	FbxAnimStack* AnimStack = m_pFbxScene->GetSrcObject<FbxAnimStack>();
	if (AnimStack)
	{
		//�ִϸ��̼� ������ �ҷ�����

		//�ִϸ��̼� �ִ����
		m_llAnimationMaxTime = AnimStack->GetLocalTimeSpan().GetDuration().GetMilliSeconds();
		cout << "�ִϸ��̼� �ִ���� : " << m_llAnimationMaxTime << endl;

		//�ִϸ��̼ǿ� ������ �޴� animation Node ����
		m_uiAnimationNodeIndexCount = m_IndexByName.size();

		//�ִϸ��̼� 2���� �迭 ����
		m_pAnimationMatrix = new XMFLOAT4X4*[static_cast<unsigned int>(m_llAnimationMaxTime) / 10];	//�ִ�ð�/10��ŭ �ִϸ��̼���� �迭 �Ҵ�
		m_ppResultMatrix = new XMFLOAT4X4*[static_cast<unsigned int>(m_llAnimationMaxTime) / 10];		//�ִ�ð�/10��ŭ �ִϸ��̼� ���� ��ȯ��� �迭 �Ҵ�

		for (long long i = 0; i < m_llAnimationMaxTime / 10; ++i)
		{
			m_pAnimationMatrix[i] = new XMFLOAT4X4[m_uiAnimationNodeIndexCount];	//i��° �ð��� : �迭�� �ִϸ��̼� ��� ������ŭ �迭 �Ҵ�
			m_ppResultMatrix[i] = new XMFLOAT4X4[m_uiAnimationNodeIndexCount];		//i��° �ð��� : ������ȯ��Ŀ� �ִϸ��̼� ��� ������ŭ �迭 �Ҵ�
		}

		//Animation Matrixä���
		_SetAnimationMatrix(m_pRoot, AnimStack);

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

void CFbx::_SetBoneMatrix(FbxNode* pNode)
{
	m_IndexByName[pNode->GetName()] = m_IndexByName.size();
	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		_SetBoneMatrix(pNode->GetChild(i));
	}
}








void Warning(char* pWarningText)
{
	cout << pWarningText << endl;
	cout << "�ƹ� Ű�� ������ �����մϴ�" << endl;
	char a;
	cin >> a;
	exit(0);
}



