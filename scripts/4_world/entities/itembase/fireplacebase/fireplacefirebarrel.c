class FireplaceFireBarrel : FireplaceIndoor
{
	void FireplaceFireBarrel()
	{
		m_UTSSettings.m_TemperatureItemCap 		= GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
		m_UTSSettings.m_TemperatureCap			= 20;
		
		m_UnderObjectDecalSpawnSettings.m_ScaleMax = 0.44;

		m_ThawnSurfaceUnderSupport = true;
	}
	
	override bool IsBaseFireplace()
	{
		return true;
	}
}
