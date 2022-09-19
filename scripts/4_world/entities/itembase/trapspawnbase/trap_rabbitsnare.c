class Trap_RabbitSnare extends TrapSpawnBase
{
	void Trap_RabbitSnare()
	{
		m_DefectRate = 15; 			//Added damage after trap activation
		
		m_InitWaitTime 							= Math.RandomInt(900, 1500);
		m_UpdateWaitTime 						= 30;
		m_IsFoldable 							= true;
		m_IsUsable 								= true;
		m_MinimalDistanceFromPlayersToCatch 	= 30;
		
		m_BaitCatchProb 						= 85;
		m_NoBaitCatchProb						= 15;

		m_AnimationPhaseSet 					= "inventory";
		m_AnimationPhaseTriggered 				= "placing";
		m_AnimationPhaseUsed 					= "triggered";

		m_WaterSurfaceForSetup = false;
		
		m_CatchesGroundAnimal = new multiMap<string, float>;
		m_CatchesGroundAnimal.Insert("DeadRooster", 1);
		m_CatchesGroundAnimal.Insert("DeadChicken_White", 1);
		m_CatchesGroundAnimal.Insert("DeadChicken_Spotted", 1);
		m_CatchesGroundAnimal.Insert("DeadChicken_Brown", 1);
		// ALWAYS keep rabbit last as that is how it gets the rabbit in case of rabbit specific bait
		m_CatchesGroundAnimal.Insert("DeadRabbit", 1);
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		if ( IsPlaceSound() )
		{
			PlayPlaceSound();
		}
	}
	
	override void SetupTrap()
	{
		if ( GetGame().IsServer() )
		{
			if ( GetHierarchyRootPlayer().CanDropEntity( this ) )
			{
				if ( IsRuined() ) 
				{
					PlayerBase player = PlayerBase.Cast( GetHierarchyRootPlayer() );
				}
				else
				{
					PlayerBase owner_player = PlayerBase.Cast( GetHierarchyRootPlayer() );
					
					//GetDirection
					vector trapPos = owner_player.GetDirection();
					trapPos[1] = 0;
					SetPosition( owner_player.GetPosition() + trapPos );

					SetActive();
					
					SetOrientation( owner_player.GetOrientation() );
				}
			}
		}
	}
	
	override void SpawnCatch()
	{
		super.SpawnCatch();
		
		if ( m_CanCatch )
		{
			multiMap<string, float>	catches;
			
			// We read the relevant catch map
			ItemBase catch;
			catches = m_CatchesGroundAnimal;

			// The catch map contains data
			if ( catches && catches.Count() > 0 )
			{
				// select random object from catches
				int count = catches.Count() - 1;
				int randomCatchIndex = Math.RandomInt( 0, count );
			
				if ( Math.RandomFloat(0, 100) < m_FinalCatchProb )
				{
					if ( m_Bait )
					{
						if ( m_Bait.IsInherited( Worm ) )
						{
							// We can only catch chicken, so exclude the rabbit
							randomCatchIndex = Math.RandomInt( 0, count - 1 );
							catch = ItemBase.Cast( GetGame().CreateObjectEx( catches.GetKeyByIndex( randomCatchIndex ), m_PreyPos, ECE_PLACE_ON_SURFACE ) );
						}
						else	
						{
							// Get the last index, which is the rabbit
							randomCatchIndex = count;
							catch = ItemBase.Cast( GetGame().CreateObjectEx( catches.GetKeyByIndex( randomCatchIndex ), m_PreyPos, ECE_PLACE_ON_SURFACE ) );
						}
					}
					else
					{
						// No bait, 50 / 50 rabbit
						randomCatchIndex = Math.RandomIntInclusive( 0, 1 );
						if ( randomCatchIndex == 0 )
						{
							randomCatchIndex = Math.RandomInt( 0, count - 1 );
							catch = ItemBase.Cast( GetGame().CreateObjectEx( catches.GetKeyByIndex( randomCatchIndex ), m_PreyPos, ECE_PLACE_ON_SURFACE ) );
						}
						else
						{
							randomCatchIndex = count;
							catch = ItemBase.Cast( GetGame().CreateObjectEx( catches.GetKeyByIndex( randomCatchIndex ), m_PreyPos, ECE_PLACE_ON_SURFACE ) );
						}
					}
					
					// We set quantity of prey
					if ( catch )
						CatchSetQuant( catch );
				}
				
				// We update the state
				SetUsed();
				
				// We remove the bait from this trap
				if ( m_Bait )
					m_Bait.Delete();
			}
			
			// We damage the trap
			AddDefect();
		}
	}
	
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
		return "placeRabbitSnareTrap_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "rabbitsnare_deploy_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		// We remove the hunting trap deploy action in order to all advanced placement
		RemoveAction(ActionDeployHuntingTrap);
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
	}
	
	// ===============================================================
	// =====================  DEPRECATED  ============================
	// ===============================================================
	
	override void AlignCatch( ItemBase obj, string catch_name )
	{
		if ( catch_name == "Animal_LepusEuropaeus" )
		{
			obj.SetOrientation( GetOrientation() );
			
			vector forward_vec = GetDirection();
			vector side_vec = forward_vec.Perpend(  ) * -0.22;
			forward_vec = forward_vec * -0.3;
			
			vector chatch_pos = obj.GetPosition() + forward_vec + side_vec;
			obj.SetPosition( chatch_pos );
		}
	}
}

class RabbitSnareTrap extends Trap_RabbitSnare 
{	

}