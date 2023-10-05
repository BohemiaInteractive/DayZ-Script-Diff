class AnniversaryBox extends Container_Base
{
	const int SPAWN_SHIRTS_MAX = 60;
	protected AnniversaryBoxLight m_Light;
	
	override void DeferredInit()
	{		
		super.DeferredInit();
		
		if (GetGame().IsServer())//server or single player
		{
			EntityAI ent = GetInventory().CreateInInventory("TShirt_10thAnniversary");
			
			for (int i = 0; ent && i < SPAWN_SHIRTS_MAX - 1; i++)
			{
				ent = GetInventory().CreateInInventory("TShirt_10thAnniversary");
			}
		}
		
		#ifndef SERVER//not dedicated server, ie. client or single player
		m_Light = AnniversaryBoxLight.Cast(ScriptedLightBase.CreateLight(AnniversaryBoxLight, "0 0 0"));
		if (m_Light)
			m_Light.AttachOnMemoryPoint(this, "light");
		#endif
				
	}
		
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (m_Light)
			m_Light.Destroy();
	}	
	
	override bool IsContainer()
	{
		return true;
	}
	
	override bool IsTakeable()
	{
		return false;
	}
	
	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		return false;
	}
}