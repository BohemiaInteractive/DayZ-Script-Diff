class PlaceObjectActionData : ActionData
{
	vector m_Position;
	vector m_Orientation;
	bool m_AlreadyPlaced;
}

class ActiondeployObjectCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(m_ActionData.m_MainItem.GetDeployTime());
		
	}
	
	void DropDuringPlacing()
	{
		EntityAI entity_for_placing = m_ActionData.m_MainItem;
		if ( entity_for_placing.IsBasebuildingKit() )
			return;
		
		m_ActionData.m_Player.PredictiveDropEntity(m_ActionData.m_MainItem);
	}
};

class ActionDeployBase: ActionContinuousBase
{
	protected const float POSITION_OFFSET = 0.5; // The forward offset at which the item will be placed ( if not using hologram )
	
	void ActionDeployBase()
	{
		m_CallbackClass		= ActiondeployObjectCB;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;
		m_FullBody			= true;
		m_Text = "#deploy_object";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasTarget()
	{
		return false;
	}

	override bool HasProgress()
	{
		return true;
	}
	
	override ActionData CreateActionData()
	{
		PlaceObjectActionData action_data = new PlaceObjectActionData;
		return action_data;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlaceObjectActionData poActionData;
		poActionData = PlaceObjectActionData.Cast(action_data);
		
		if ( !poActionData ) { return; }
		if ( !action_data.m_MainItem ) { return; }
		
		EntityAI entity_for_placing = action_data.m_MainItem;
		vector position;
		vector orientation;
		
		// In case of placement with hologram
		if ( action_data.m_Player.GetHologramServer() )
		{
			position = action_data.m_Player.GetLocalProjectionPosition();
			orientation = action_data.m_Player.GetLocalProjectionOrientation();
			
			action_data.m_Player.GetHologramServer().EvaluateCollision(action_data.m_MainItem);
			if ( GetGame().IsMultiplayer() && action_data.m_Player.GetHologramServer().IsColliding() )
			{
				return;
			}
			
			action_data.m_Player.GetHologramServer().PlaceEntity( entity_for_placing );
			
			if ( GetGame().IsMultiplayer() )
				action_data.m_Player.GetHologramServer().CheckPowerSource();
		}
		else
		{
			position = action_data.m_Player.GetPosition();
			orientation = action_data.m_Player.GetOrientation();
		
			position = position + ( action_data.m_Player.GetDirection() * POSITION_OFFSET );
		}
		
		action_data.m_Player.PlacingCompleteServer();
		entity_for_placing.OnPlacementComplete( action_data.m_Player, position, orientation );
		
		MoveEntityToFinalPosition( action_data, position, orientation );		
		GetGame().ClearJuncture( action_data.m_Player, entity_for_placing );
		action_data.m_MainItem.SetIsBeingPlaced( false );
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		poActionData.m_AlreadyPlaced = true;	
		action_data.m_MainItem.SoundSynchRemoteReset();
	}
	
	void MoveEntityToFinalPosition( ActionData action_data, vector position, vector orientation )
	{
		if ( action_data.m_MainItem.IsBasebuildingKit() ) return;
		
		EntityAI entity_for_placing = action_data.m_MainItem;
		vector rotation_matrix[3];
		float direction[4];
		InventoryLocation source = new InventoryLocation;
		InventoryLocation destination = new InventoryLocation;
		
		Math3D.YawPitchRollMatrix( orientation, rotation_matrix );
		Math3D.MatrixToQuat( rotation_matrix, direction );
		
		if ( entity_for_placing.GetInventory().GetCurrentInventoryLocation( source ) )
		{
			destination.SetGroundEx( entity_for_placing, position, direction );
			
			if ( GetGame().IsMultiplayer() )
			{
				action_data.m_Player.ServerTakeToDst(source, destination);
			}		
			//local singleplayer
			else
			{
				MoveEntityToFinalPositionSinglePlayer(action_data, source, destination);
			}			
		}
	}
	
	void MoveEntityToFinalPositionSinglePlayer(ActionData action_data, InventoryLocation source, InventoryLocation destination)
	{
		action_data.m_Player.GetInventory().TakeToDst(InventoryMode.LOCAL, source, destination);
	}
}