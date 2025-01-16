class Trap_RabbitSnare extends TrapSpawnBase
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
		m_IsFoldable 							= true;
		m_MinimalDistanceFromPlayersToCatch 	= 15;
		
		m_AnimationPhaseSet 					= "inventory";
		m_AnimationPhaseTriggered 				= "placing";
		m_AnimationPhaseUsed 					= "triggered";
	}
	
	override bool CanBePlaced(Man player, vector position)
	{
		if (m_IsBeingPlaced)
			return true;

		int liquidType;
		string surfaceType;
		g_Game.SurfaceUnderObject(PlayerBase.Cast(player).GetHologramLocal().GetProjectionEntity(), surfaceType, liquidType);

		return g_Game.IsSurfaceDigable(surfaceType);
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
	
	override void InitCatchingComponent()
	{
		if (!m_CatchingContext)
		{
			int updateCount = m_MaxActiveTime/m_UpdateWaitTime;
			Param2<EntityAI,int> par = new Param2<EntityAI,int>(this,updateCount);
			m_CatchingContext = new CatchingContextTrapLandSnare(par);
		}
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
			
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		SetOrientation(orientation);	
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