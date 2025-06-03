//----------------------------------------------------------------------------------------
/*
	Allows weighted blending of values defined by their 2D position in space.
*/
class Blend2D<Class T>
{
	private ref array<vector> m_Positions;
	private ref array<T>      m_Values;
	private ref array<float>  m_Weights;

	//----------------------------------------------------------------------------------------
	/*!
		Create new blend structure.
	*/
	void Blend2D()
	{
		m_Positions = {};
		m_Weights   = {};
		m_Values    = {};
	}

	//----------------------------------------------------------------------------------------
	/*!
		Insert new `value` at coordinate [posX, posY].
	*/
	void Insert(float posX, float posY, T value)
	{
		m_Positions.Insert(Vector(posX, posY, 0));
		m_Values.Insert(value);
		m_Weights.Insert(0);
	}

	//----------------------------------------------------------------------------------------
	/*!
		Empty the blend structure.
	*/
	void Clear()
	{
		m_Positions.Clear();
		m_Values.Clear();
		m_Weights.Clear();
	}

	//----------------------------------------------------------------------------------------
	/*
		Evaluate and return the result of the blend sampled at coordinate [posX, posY].
	*/
	T Blend(float posX, float posY)
	{
		vector samplePosition = Vector(posX, posY, 0);
		Math3D.BlendCartesian(samplePosition, m_Positions, m_Weights);

		T result;
		int numValues = m_Values.Count();
		for (int v = 0; v < numValues; ++v)
		{
			result += (m_Values[v] * m_Weights[v]);
		}

		return result;
	}

}

//----------------------------------------------------------------------------------------
typedef  Blend2D<vector> Blend2DVector;