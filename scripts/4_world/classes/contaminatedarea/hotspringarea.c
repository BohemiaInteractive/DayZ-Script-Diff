//! Area for hot springs
class HotSpringArea : EffectArea
{
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_UTSLConstant;
	
	override void DeferredInit()
	{
		super.DeferredInit();
		
		InitZone();		
	}

	override void InitZoneServer()
	{
		super.InitZoneServer();
		
		m_UTSSettings 							= new UniversalTemperatureSourceSettings();
		m_UTSSettings.m_Updateable				= true;
		m_UTSSettings.m_UpdateInterval			= 1;
		m_UTSSettings.m_TemperatureItemCap 		= GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
		m_UTSSettings.m_TemperatureCap			= 24;
		m_UTSSettings.m_RangeFull				= 1;
		m_UTSSettings.m_RangeMax				= m_Radius;
		m_UTSSettings.m_Position 				= m_Position;
		m_UTSSettings.m_IsWorldOverriden		= false;
		
		m_UTSLConstant 	= new UniversalTemperatureSourceLambdaConstant();
		m_UTSource 		= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLConstant);
		
		if ( m_TriggerType != "" )
			CreateTrigger(m_PositionTrigger, m_Radius);
		
		m_UTSource.SetActive(true);
	}
}