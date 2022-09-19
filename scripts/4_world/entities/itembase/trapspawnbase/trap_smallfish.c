class Trap_SmallFish extends TrapSpawnBase
{
	void Trap_SmallFish()
	{
		m_DefectRate = 15; 			//Added damage after trap activation
		
		m_InitWaitTime = Math.RandomFloat(900,1500);
		m_UpdateWaitTime = 30;

		m_AnimationPhaseSet 					= "inventory";
		m_AnimationPhaseTriggered 				= "placing";
		m_AnimationPhaseUsed 					= "triggered";
		
		m_MinimalDistanceFromPlayersToCatch 	= 15;
		
		m_BaitCatchProb 						= 85;
		m_NoBaitCatchProb						= 15;

		m_WaterSurfaceForSetup = true;

		m_CatchesPond = new multiMap<string, float>;
		m_CatchesPond.Insert("Bitterlings", 1);
		
		m_CatchesSea = new multiMap<string, float>;
		m_CatchesSea.Insert("Sardines",1);
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		if ( IsPlaceSound() )
		{
			PlayPlaceSound();
		}
	}
	
	override void SpawnCatch()
	{
		super.SpawnCatch();
		
		if ( m_CanCatch )
		{
			// We get the prey position for spawning
			multiMap<string, float>	catches;
			vector pos = GetPosition();
			
			ItemBase catch;
			// Select catch type depending on water type ( FRESH VS SALT )
			if ( GetGame().SurfaceIsSea( pos[0], pos[2] ) ) 
			{
				catches = m_CatchesSea;
			}
			else if ( GetGame().SurfaceIsPond( pos[0], pos[2] ) ) 
			{
				catches = m_CatchesPond;
			}

			if ( catches && catches.Count() > 0 )
			{
				// select random object from catches
				int count = catches.Count() - 1;
				int randomCatchIndex = Math.RandomInt( 0, count );
			
				if ( Math.RandomFloat(0, 100) < m_FinalCatchProb )
				{
					catch = ItemBase.Cast( GetGame().CreateObjectEx( catches.GetKeyByIndex(randomCatchIndex), m_PreyPos, ECE_NONE ) );
					
					// Set the quantity of caught prey
					if ( catch )
						CatchSetQuant( catch );	
				}
				
				// We change the trap state and visuals
				SetUsed();
				
				// We remove the bait from this trap
				if ( m_Bait )
					m_Bait.Delete();
			}
			
			// Deal damage to trap
			AddDefect();
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