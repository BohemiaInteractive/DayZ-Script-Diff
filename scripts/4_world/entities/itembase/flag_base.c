class Flag_Base extends ItemBase
{
	void Flag_Base()
	{
		//synchronized variables
		//RegisterNetSyncVariableBool( "m_IsMounted" );
		ShowSelection("folded");
		HideSelection("unfolded");
	}
	
	void ~Flag_Base()
	{
	}
	
	// --- SYNCHRONIZATION
	void Synchronize()
	{
		if ( GetGame().IsServer() )
		{
			SetSynchDirty();
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		
	}
	
	// --- EVENTS
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		return true;
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();
				
		//set mounted state based on locked slot after everything is loaded
		//SetMountedState( GetSlotLockedState() );
	}
	
	/*override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);
		
		ShowSelection("folded");
		HideSelection("unfolded");
	}
	
	override void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);
		
		HideSelection("folded");
		ShowSelection("unfolded");
	}*/
	
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