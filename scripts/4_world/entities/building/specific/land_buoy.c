class Land_Buoy extends House
{
	protected BuoyLight m_Light;
	
	override void DeferredInit()
	{
		super.DeferredInit();
		
		if (!g_Game.IsDedicatedServer())
		{
			m_Light = BuoyLight.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(BuoyLight, this, "light_01"));
			m_Light.SetSelectionID(GetHiddenSelectionIndex("Glass"));
			m_Light.UpdateLightSourceMaterial(BuoyLight.LIGHT_MAT_ON);
		}
	}
		
	void ~Land_Buoy()
	{
		if (m_Light)
			m_Light.Destroy();
	}
}

class Land_MooringBuoy extends House
{
	protected MooringBuoyLight m_Light;
	
	override void DeferredInit()
	{
		super.DeferredInit();
		
		if (!g_Game.IsDedicatedServer())
		{
			m_Light = MooringBuoyLight.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(MooringBuoyLight, this, "light_01"));
			m_Light.SetSelectionID(GetHiddenSelectionIndex("Glass"));
			m_Light.UpdateLightSourceMaterial(MooringBuoyLight.LIGHT_MAT_ON);
		}
	}
		
	void ~Land_MooringBuoy()
	{
		if (m_Light)
			m_Light.Destroy();
	}
}