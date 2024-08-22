class AnniversaryBox extends Container_Base
{
	private const int SPAWN_SHIRTS_MAX = 60;
	
	#ifndef SERVER
	protected AnniversaryBoxLight m_Light;
	#endif
	
	override void DeferredInit()
	{		
		super.DeferredInit();		
		
		#ifndef SERVER
		m_Light = AnniversaryBoxLight.Cast(ScriptedLightBase.CreateLight(AnniversaryBoxLight, "0 0 0"));
		if (m_Light)
			m_Light.AttachOnMemoryPoint(this, "light");
		#endif
				
	}
		
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		#ifndef SERVER
		if (m_Light)
			m_Light.Destroy();
		#endif
	}	
	
	override bool IsTakeable()
	{
		return false;
	}
	
	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		return false;
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}
	
	override bool DisableVicinityIcon()
	{
		if (GetAnimationPhase("lidclosing") == 1)
		{
			return true;
		}
		else
			return false;	
	}
	
	override bool CanDisplayCargo()
	{
		if (GetAnimationPhase("lidclosing") == 1)
		{
			return false;
		}
		else
			return true;
	}
	
	void EmtpyInventory()
	{
		if (GetGame().IsServer())//server or single player
		{		
			for ( int j = 0; j < GetInventory().GetCargo().GetItemCount(); j++ )
			{
				GetInventory().GetCargo().GetItem(j).Delete();
			}
		}
	}
	
	void FillInventory()
	{
		if (GetGame().IsServer())//server or single player
		{		
			EntityAI ent = GetInventory().CreateInInventory("TShirt_10thAnniversary");
			
			for (int i = 0; ent && i < SPAWN_SHIRTS_MAX - 1; i++)
			{
				ent = GetInventory().CreateInInventory("TShirt_10thAnniversary");
			}
		}
	}
}