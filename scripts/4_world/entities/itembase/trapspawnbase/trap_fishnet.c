class Trap_FishNet extends TrapSpawnBase
{
	override void InitTrapValues()
	{
		super.InitTrapValues();
		
		m_DefectRate = 2.5; 			//Added damage after trap activation
		
		m_InitWaitTimeMin 						= 120;
		m_InitWaitTimeMax 						= 180;
		m_UpdateWaitTime 						= 30;
		m_SpawnUpdateWaitTime 					= 30;
		m_MaxActiveTime 						= 1200;
		m_IsFoldable = true;
		
		m_MinimalDistanceFromPlayersToCatch 	= 15;

		m_AnimationPhaseSet 					= "inventory";
		m_AnimationPhaseTriggered 				= "placing";
		m_AnimationPhaseUsed 					= "triggered";
	}
		
	override void InitCatchingComponent()
	{
		if (!m_CatchingContext)
		{
			int updateCount = m_MaxActiveTime/m_UpdateWaitTime;
			Param2<EntityAI,int> par = new Param2<EntityAI,int>(this,updateCount);
			m_CatchingContext = new CatchingContextTrapFishLarge(par);
		}
		
		super.InitCatchingComponent();
	}
	
	//========================================================
	//============= PLACING AND INVENTORY EVENTS =============
	//========================================================
	
	override bool IsPlaceableAtPosition( vector position )
	{
		return IsSurfaceWater( position );
	}

	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		if ( !attachment.IsInherited( Worm ) )
			return false;
		
		return super.CanReceiveAttachment( attachment, slotId );
	}
	
	#ifdef PLATFORM_WINDOWS
	// How one sees the tripwire when in vicinity
	override int GetViewIndex()
	{
		if ( MemoryPointExists( "invView2" ) )
		{		
			InventoryLocation il = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation( il );
			InventoryLocationType type = il.GetType();
			switch ( type )
			{
				case InventoryLocationType.CARGO:
				{
					return 0;
				}
				case InventoryLocationType.ATTACHMENT:
				{
					return 1;
				}
				case InventoryLocationType.HANDS:
				{
					return 0;
				}
				case InventoryLocationType.GROUND:
				{
					// Different view index depending on deployment state 
					if ( IsActive() )
						return 1;
					
					// When folded
					return 0;
				}
				case InventoryLocationType.PROXYCARGO:
				{
					return 0;
				}
				default:
				{
					if ( IsActive() )
						return 1;
					
					// When folded
					return 0;
				}
			}
		}
		return 0;
	}
	#endif
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================	
	override bool IsDeployable()
	{
		return true;
	}
	
	override string GetDeploySoundset()
	{
		return "placeFishNetTrap_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "fishnet_deploy_SoundSet";
	}
}

class FishNetTrap extends Trap_FishNet 
{

}