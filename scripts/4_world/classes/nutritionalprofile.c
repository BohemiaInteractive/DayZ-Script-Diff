class NutritionalProfile
{
	float m_Energy;
	float m_WaterContent;
	float m_NutritionalIndex;
	float m_FullnessIndex;
	float m_Toxicity;
	int m_LiquidType;
	int m_Agents;
	string m_LiquidClassname;
	float m_Digestibility;
	
	void NutritionalProfile(float energy, float water_content, float nutritional_index, float fullness_index, float toxicity, int agents, float digestibility)
	{
		m_Energy = energy;
		m_WaterContent = water_content;
		m_NutritionalIndex = nutritional_index;
		m_FullnessIndex = fullness_index;
		m_Toxicity = toxicity;
		m_Agents = agents;
		m_Digestibility = digestibility;
	}
	
	void MarkAsLiquid(int liquid_type, string classname)
	{
		m_LiquidType = liquid_type;
		m_LiquidClassname = classname;
	}
	
	int GetAgents()
	{
		return m_Agents;
	}
	
	int GetLiquidType()
	{
		return m_LiquidType;
	}
	
	string GetLiquidClassname()
	{
		return m_LiquidClassname;
	}
	
	bool IsLiquid()
	{
		return (m_LiquidType > 0);
	}
	
	float GetEnergy()
	{
		return m_Energy;
	}
	
	float GetWaterContent()
	{
		return m_WaterContent;
	}
	
	float GetNutritionalIndex()
	{
		return m_NutritionalIndex;
	}
	
	float GetFullnessIndex()
	{
		return m_FullnessIndex;
	}
	
	float GetToxicity()
	{
		return m_Toxicity;
	}	
	
	float GetDigestibility()
	{
		return m_Digestibility;
	}
}
