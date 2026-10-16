class Flag_Base extends ItemBase
{
	void Flag_Base()
	{
		//synchronized variables
		//RegisterNetSyncVariableBool( "m_IsMounted" );
		ShowSelection("folded");
		HideSelection("unfolded");
	}
	
	// --- SYNCHRONIZATION
	void Synchronize()
	{
		if ( g_Game.IsServer() )
		{
			SetSynchDirty();
		}
	}
	
	// --- EVENTS
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent,slot_id);
		
		HideSelection("folded");
		ShowSelection("unfolded");
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent,slot_id);
		
		ShowSelection("folded");
		HideSelection("unfolded");
	}
	
	// SOUNDS
	// ---------------------------------------------------------
	//TODO
	// ---------------------------------------------------------
	
	override int GetViewIndex()
	{
		if ( MemoryPointExists( "invView2" ) )
		{		
			InventoryLocation il = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation( il );
			InventoryLocationType type = il.GetType();
			switch ( type )
			{
				case InventoryLocationType.ATTACHMENT:
				{
					return 1;
				}
				default:
				{
					return 0;
				}
			}
		}
		return 0;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttach);
	}
}

class Flag_Chernarus extends Flag_Base {};
class Flag_Chedaki extends Flag_Base {};
class Flag_NAPA extends Flag_Base {};
class Flag_CDF extends Flag_Base {};
class Flag_Livonia extends Flag_Base {};
class Flag_Altis extends Flag_Base {};
class Flag_SSahrani extends Flag_Base {};
class Flag_NSahrani extends Flag_Base {};
class Flag_DayZ extends Flag_Base {};
class Flag_LivoniaArmy extends Flag_Base {};
class Flag_White extends Flag_Base {};
class Flag_Bohemia extends Flag_Base {};
class Flag_APA extends Flag_Base {};
class Flag_UEC extends Flag_Base {};
class Flag_Pirates extends Flag_Base {};
class Flag_Cannibals extends Flag_Base {};
class Flag_Bear extends Flag_Base {};
class Flag_Wolf extends Flag_Base {};
class Flag_BabyDeer extends Flag_Base {};
class Flag_Rooster extends Flag_Base {};
class Flag_LivoniaPolice extends Flag_Base {};
class Flag_CMC extends Flag_Base {};
class Flag_TEC extends Flag_Base {};
class Flag_CHEL extends Flag_Base {};
class Flag_Zenit extends Flag_Base {};
class Flag_HunterZ extends Flag_Base {};
class Flag_BrainZ extends Flag_Base {};
class Flag_Refuge extends Flag_Base {};
class Flag_RSTA extends Flag_Base {};
class Flag_Snake extends Flag_Base {};
class Flag_Rex extends Flag_Base {};
class Flag_Zagorky extends Flag_Base {};
class Flag_Crook extends Flag_Base {};
