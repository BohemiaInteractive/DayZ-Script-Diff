
class PluginTargetTemperature extends PluginBase
{
	protected PlayerBase m_pPlayer;
	
	void Init(PlayerBase player)
	{
		m_pPlayer = player;
	}
	
	void ShowDebug(bool show)
	{
		DbgUI.BeginCleanupScope();

		if (show && m_pPlayer)
		{
			EntityAI ent = GetTargetEntity();
			
			DbgUI.Begin("Target Temperature", 10, 10);
			DbgUI.Panel("", 150, 1);
			if (ent)
			{
				DbgUI.Text("Entity: : " + ent.GetType());
				DbgUI.Text("Temperature: " + ent.GetTemperature());
				DbgUI.Text("Frozen State: " + ent.GetIsFrozen());
				DbgUI.Text("Freeze/Thaw Progress: " + ent.GetFreezeThawProgress());
			}
			DbgUI.End();
		}

		DbgUI.EndCleanupScope();
	}
	
	//! utilizes camera position from the CGame
	EntityAI GetTargetEntity()
	{
		float hitFraction;
		vector start = GetGame().GetCurrentCameraPosition();
		vector end = start + (GetGame().GetCurrentCameraDirection() * 5.0);	

		vector hitPos, hitNormal;
		Object hitObj;
		
		PhxInteractionLayers hitMask = 0xFFFFFFFFFF;

		DayZPhysics.RayCastBullet(start, end, hitMask, m_pPlayer, hitObj, hitPos, hitNormal, hitFraction);
		
		return EntityAI.Cast(hitObj);
	} 
}