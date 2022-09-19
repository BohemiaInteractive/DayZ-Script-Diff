//! Newer implementation equivalent of "AreaDamageRegular", hitzone selection only
class AreaDamageComponentRandomHitzone : AreaDamageComponent
{
	ref array<string>	m_HitZones;
	
	void AreaDamageComponentRandomHitzone(AreaDamageManager parent)
	{
		m_HitZones = new array<string>;
	}
	
	void SetHitZones( array<string> hitzones )
	{
		m_HitZones = hitzones;
	}
	
	override protected AreaDamageComponentData GetAreaDamageComponentData(Object object)
	{
		AreaDamageComponentData data = new AreaDamageComponentData;
		data.Hitzone = m_HitZones.GetRandomElement();
		
		return data;
	}
}