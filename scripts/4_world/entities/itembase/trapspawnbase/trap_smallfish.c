class Trap_SmallFish extends TrapSpawnBase
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

		m_MinimalDistanceFromPlayersToCatch 	= 15;
		
		m_AnimationPhaseSet 					= "inventory";
		m_AnimationPhaseTriggered 				= "placing";
		m_AnimationPhaseUsed 					= "triggered";
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		if ( IsPlaceSound() )
		{
			PlayPlaceSound();
		}
	}
	
	override void InitCatchingComponent()
	{
		if (!m_CatchingContext)
		{
			int updateCount = m_MaxActiveTime/m_UpdateWaitTime;
			Param2<EntityAI,int> par = new Param2<EntityAI,int>(this,updateCount);
			m_CatchingContext = new CatchingContextTrapFishSmall(par);
		}
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
					if ( IsDeployed() )
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
					if ( IsDeployed() )
						return 1;
					
					// When folded
					return 0;
				}
			}
		}
		return 0;
	}
	#endif
}

class SmallFishTrap extends Trap_SmallFish 
{
	// DEPRECATED
	ref RainProcurementManager m_RainProcurement;
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		SetIsPlaceSound( true );
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	override string GetDeploySoundset()
	{
		return "placeSmallFishTrap_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "fishtrap_deploy_SoundSet";
	}
	
	override bool DoPlacingHeightCheck()
	{
		return true; //has to be able to catch rain, default distance raycast
	}
}