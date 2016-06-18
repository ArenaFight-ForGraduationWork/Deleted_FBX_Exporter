#ifndef VERTEX_H_
#define VERTEX_H_



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





#endif