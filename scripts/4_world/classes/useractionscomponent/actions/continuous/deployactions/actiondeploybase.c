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
	
	//!DEPRECATED
	void DropDuringPlacing();
}

class ActionDeployBase : ActionContinuousBase
{
	protected const float POSITION_OFFSET = 0.5; // The forward offset at which the item will be placed (if not using hologram)
	
	protected ref array<ItemBase> m_MovedItems;
	
	void ActionDeployBase()
	{
		m_CallbackClass		= ActiondeployObjectCB;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;
		m_FullBody			= true;

		m_Text = "#deploy_object";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNone();
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
		PlaceObjectActionData action_data = new PlaceObjectActionData();
		m_MovedItems = new array<ItemBase>();
		
		return action_data;
	}
	
	override Vector2 GetCameraUDAngle()
	{
		Vector2 udAngle = new Vector2(-80, -20);
		return udAngle;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		PlaceObjectActionData poActionData;
		poActionData = PlaceObjectActionData.Cast(action_data);
		
		if (!poActionData)
			return;

		if (!action_data.m_MainItem)
			return;
		
		EntityAI entity_for_placing = action_data.m_MainItem;
		vector position;
		vector orientation;
		
		// In case of placement with hologram
		if (action_data.m_Player.GetHologramServer())
		{
			position 	= action_data.m_Player.GetLocalProjectionPosition();
			orientation = action_data.m_Player.GetLocalProjectionOrientation();
			
			action_data.m_Player.GetHologramServer().EvaluateCollision(action_data.m_MainItem);
			if (GetGame().IsMultiplayer() && action_data.m_Player.GetHologramServer().IsColliding())
				return;
			
			action_data.m_Player.GetHologramServer().PlaceEntity(entity_for_placing);
			
			if (GetGame().IsMultiplayer())
				action_data.m_Player.GetHologramServer().CheckPowerSource();
		}
		else
		{
			position 	= action_data.m_Player.GetPosition();
			orientation = action_data.m_Player.GetOrientation();
			position 	= position + (action_data.m_Player.GetDirection() * POSITION_OFFSET);
		}
		
		MoveEntityToFinalPosition(action_data, position, orientation);			
		GetGame().ClearJunctureEx(action_data.m_Player, entity_for_placing);
		action_data.m_MainItem.SetIsBeingPlaced(false);
		poActionData.m_AlreadyPlaced = true;
		
		entity_for_placing.OnPlacementComplete(action_data.m_Player, position, orientation); //beware, this WILL fire on server before the item is moved to final position!
		action_data.m_Player.PlacingCompleteServer();
		
		m_MovedItems.Clear();
	}
	
	override void OnItemLocationChanged(ItemBase item)
	{
		super.OnItemLocationChanged(item);
		
		if (!GetGame().IsDedicatedServer())
		{
			if (m_MovedItems)
				m_MovedItems.Insert(item);
		}
	}
	
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);
		
		foreach (ItemBase item : m_MovedItems)
		{
			if (item == action_data.m_MainItem)
			{
				InventoryLocation loc = new InventoryLocation();
				item.GetInventory().GetCurrentInventoryLocation(loc);
				if (loc && loc.GetType() == InventoryLocationType.GROUND)	// if main item is placed on ground during deploy, re-reserve it
					InventoryReservation(action_data);
			}
		}
		
		m_MovedItems.Clear();
	}
	
	void DropDuringPlacing(PlayerBase player)
	{
		ItemBase item;
		if (!Class.CastTo(item, player.GetItemInHands()))
			return;
		
		if (item.IsBasebuildingKit())
			return;
		
		player.PredictiveDropEntity(item);
	}
	
	void MoveEntityToFinalPosition(ActionData action_data, vector position, vector orientation)
	{
		if (action_data.m_MainItem.IsBasebuildingKit())
			return;
		
		EntityAI entity_for_placing = action_data.m_MainItem;
		vector rotation_matrix[3];
		float direction[4];
		InventoryLocation source = new InventoryLocation;
		InventoryLocation destination = new InventoryLocation;
		
		Math3D.YawPitchRollMatrix(orientation, rotation_matrix);
		Math3D.MatrixToQuat(rotation_matrix, direction);
		
		if (entity_for_placing.GetInventory().GetCurrentInventoryLocation(source))
		{
			destination.SetGroundEx(entity_for_placing, position, direction);
			
			if (GetGame().IsMultiplayer())
				action_data.m_Player.ServerTakeToDst(source, destination);
			else // singleplayer
				MoveEntityToFinalPositionSinglePlayer(action_data, source, destination);
		
		}
	}
	
	void MoveEntityToFinalPositionSinglePlayer(ActionData action_data, InventoryLocation source, InventoryLocation destination)
	{
		if (HasProgress())
			action_data.m_Player.GetInventory().TakeToDst(InventoryMode.LOCAL, source, destination); // from ground to target position
		else 
			action_data.m_Player.GetDayZPlayerInventory().RedirectToHandEvent(InventoryMode.LOCAL, source, destination); // placing directly from inventory
	}
}