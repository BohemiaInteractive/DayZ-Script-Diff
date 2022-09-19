class CTSaveStructure
{
	int		m_InterpType;
	int		m_InterpSpeed;
	float	m_BlurValue;
	
	ref array<ref Param6<vector, vector, float, float, float, int>>				m_Cameras;
	ref array<ref Param4<int, int, float, bool>>								m_Events;
	ref array<ref Param5<vector, vector, string, ref array<string>, string>>	m_Actors;
	
	void CTSaveStructure()
	{
		m_Cameras	= new array<ref Param6<vector, vector, float, float, float, int>>;
		m_Events	= new array<ref Param4<int, int, float, bool>>;
		m_Actors	= new array<ref Param5<vector, vector, string, ref array<string>, string>>;
	}
}