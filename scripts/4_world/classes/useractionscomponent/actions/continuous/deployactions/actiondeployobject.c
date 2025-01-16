class PlaceObjectActionReciveData : ActionReciveData
{
	vector m_Position;
	vector m_Orientation;
}

class ActionDeployObject : ActionDeployBase
{			 
	void ActionDeployObject()
	{
		m_CommandUID = 0;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return true;
	}
	
	override bool IsDeploymentAction()
	{
		return true;
	}
	
	override bool CanBeUsedWithBrokenLegs()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		//Client
		if (!GetGame().IsDedicatedServer())
		{
			if (player.IsPlacingLocal())
			{
				if (!player.GetHologramLocal().IsColliding())
				{
					if (item.CanBePlaced(player, player.GetHologramLocal().GetProjectionEntity().GetPosition()))
					{
						return true;
					}
				}
			}
			return false;
		}
		//Server
		return true;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		//Server
		if (GetGame().IsDedicatedServer())
		{
			if (action_data.m_Player.IsPlacingServer())
			{
				if (GetGame().IsMultiplayer())
					action_data.m_Player.GetHologramServer().EvaluateCollision(action_data.m_MainItem);
				
				if (!action_data.m_Player.GetHologramServer().IsColliding())
				{
					if (action_data.m_MainItem.CanBePlaced(action_data.m_Player, action_data.m_Player.GetHologramServer().GetProjectionEntity().GetPosition()))
						return true;
				}
				return false;
			}
			return false;
		}
		return true;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			PlaceObjectActionData poActionData;
			poActionData = PlaceObjectActionData.Cast(action_data);
			poActionData.m_AlreadyPlaced = false;
			if (!GetGame().IsDedicatedServer())
			{
				player.GetHologramLocal().SetUpdatePosition(false);
				
				Hologram hologram = player.GetHologramLocal();
				if (hologram)
				{
					poActionData.m_Position = player.GetHologramLocal().GetProjectionPosition();
					poActionData.m_Orientation = player.GetHologramLocal().GetProjectionOrientation();
			
					poActionData.m_Player.SetLocalProjectionPosition(poActionData.m_Position);
					poActionData.m_Player.SetLocalProjectionOrientation(poActionData.m_Orientation);
				}
				else
				{
					return false;
				}
			}
			
			if (!action_data.m_MainItem)
				return false;
			
			SetupAnimation(action_data.m_MainItem);
			return true;
		}
		return false;
	}
	
	override void OnStartClient(ActionData action_data)
	{		
		PlaceObjectActionData poActionData = PlaceObjectActionData.Cast(action_data);
		if (!poActionData)
			return;
		
		if (GetGame().IsMultiplayer())
			action_data.m_Player.PlacingCompleteLocal();
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		if (GetGame().IsMultiplayer())
		{
			PlaceObjectActionData poActionData;
			poActionData = PlaceObjectActionData.Cast(action_data);
			
			if (!poActionData)
				return;
			
			EntityAI entity_for_placing = action_data.m_MainItem;
			poActionData.m_Player.SetLocalProjectionPosition(poActionData.m_Position);
			poActionData.m_Player.SetLocalProjectionOrientation(poActionData.m_Orientation);
			
			if (action_data.m_MainItem)
			{
				poActionData.m_Player.PlacingStartServer(action_data.m_MainItem);
			
				GetGame().AddActionJuncture(action_data.m_Player, entity_for_placing, 10000);
				action_data.m_MainItem.SetIsBeingPlaced(true);
			}
		}
		else
		{
			//local singleplayer			
			action_data.m_Player.PlacingStartServer(action_data.m_MainItem);
			action_data.m_MainItem.SetIsBeingPlaced(true);
		}
		
		ItemBase item = ItemBase.Cast(action_data.m_MainItem);
		if (item.GetPlaceSoundset() != string.Empty)
			item.StartItemSoundServer(SoundConstants.ITEM_PLACE);
		
		if (item.GetLoopDeploySoundset() != string.Empty)
			item.StartItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);
	}
			
	override void OnFinishProgressClient(ActionData action_data)
	{
		PlaceObjectActionData poActionData = PlaceObjectActionData.Cast(action_data);
		if (!poActionData)
			return;
		
		EntityAI entity_for_placing = action_data.m_MainItem;
		vector position = action_data.m_Player.GetLocalProjectionPosition();
		vector orientation = action_data.m_Player.GetLocalProjectionOrientation();

		poActionData.m_AlreadyPlaced = true;
		
		entity_for_placing.OnPlacementComplete(action_data.m_Player, position, orientation);
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		
		ItemBase item = ItemBase.Cast(action_data.m_MainItem);
		if (item.GetDeploySoundset() != string.Empty)
			item.StartItemSoundServer(SoundConstants.ITEM_DEPLOY);
	}

	override void OnEndClient(ActionData action_data)
	{
		super.OnEndClient(action_data);

		PlaceObjectActionData poActionData = PlaceObjectActionData.Cast(action_data);
		if (!poActionData.m_AlreadyPlaced)
		{
			action_data.m_Player.PlacingCancelLocal();
			
			//action terminated locally, send cancel to server
			poActionData.m_Player.GetActionManager().RequestEndAction();
			if (action_data.m_Player.GetHologramLocal())
				action_data.m_Player.GetHologramLocal().SetUpdatePosition(true);
			
			InventoryLocation source = new InventoryLocation;
			if (action_data.m_MainItem.GetInventory().GetCurrentInventoryLocation(source) && source.GetType() == InventoryLocationType.GROUND)
			{
				action_data.m_Player.PredictiveTakeEntityToHands(action_data.m_MainItem);
			}
		}
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);

		if (!action_data || !action_data.m_MainItem)
			return;
		
		PlaceObjectActionData poActionData = PlaceObjectActionData.Cast(action_data);
		if (!poActionData.m_AlreadyPlaced)
		{
			GetGame().ClearJunctureEx(action_data.m_Player, action_data.m_MainItem);
			action_data.m_MainItem.SetIsBeingPlaced(false);
		
			if (GetGame().IsMultiplayer())
			{	
				action_data.m_Player.PlacingCancelServer();
			}
			else
			{
				//local singleplayer
				action_data.m_Player.PlacingCancelLocal();
				action_data.m_Player.PlacingCancelServer();
			}
		}
		else
		{			
			if (action_data.m_MainItem.IsBasebuildingKit())
			{
				action_data.m_MainItem.Delete();
			}
			else
			{
				GetGame().ClearJunctureEx(action_data.m_Player, action_data.m_MainItem);
			}
		}
		
		ItemBase item = ItemBase.Cast(action_data.m_MainItem);
		if (item.GetLoopDeploySoundset() != string.Empty)
			item.StopItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);
	}
	
	override void OnStartAnimationLoop(ActionData action_data)
	{			
		if (!GetGame().IsDedicatedServer())
		{
			if (action_data.m_Player.GetItemInHands())
				ActiondeployObjectCB.Cast(action_data.m_Callback).DropDuringPlacing(); //legacy stuff
			
			if (HasProgress())	// if object has no progress, it is moved directly when action finishes
			{
				ClearInventoryReservationEx(action_data);	// Clear reservation as we put the main item from hands on ground
				DropDuringPlacing(action_data.m_Player);
			}
		}
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		PlaceObjectActionData poActionData = PlaceObjectActionData.Cast(action_data);

		ctx.Write(poActionData.m_Position);
		ctx.Write(poActionData.m_Orientation);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
			action_recive_data = new PlaceObjectActionReciveData;

		super.ReadFromContext(ctx, action_recive_data);
		PlaceObjectActionReciveData action_data_po = PlaceObjectActionReciveData.Cast(action_recive_data);
		
		vector entity_position = "0 0 0";
		vector entity_orientation = "0 0 0";
		if (!ctx.Read(entity_position))
			return false;
		if (!ctx.Read(entity_orientation))
			return false;
		
		action_data_po.m_Position = entity_position;
		action_data_po.m_Orientation = entity_orientation;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		PlaceObjectActionReciveData recive_data_po = PlaceObjectActionReciveData.Cast(action_recive_data);
		PlaceObjectActionData action_data_po = PlaceObjectActionData.Cast(action_data);
		
		action_data_po.m_Position = recive_data_po.m_Position;
		action_data_po.m_Orientation = recive_data_po.m_Orientation;
	}
	
	void SetupAnimation(ItemBase item)
	{
		if (item.IsDeployable())
		{
			if (item.IsHeavyBehaviour())
			{
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_HEAVY;
			}
			else if (item.IsOneHandedBehaviour())
			{
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_1HD; 
			}
			else if (item.IsTwoHandedBehaviour())
			{
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD;
			}
			else
			{
				Debug.Log("Error: check " + item + " behaviour");
			}
		}
		else
		{
			if (item.IsHeavyBehaviour())
			{
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_PLACING_HEAVY;
			}
			else if (item.IsOneHandedBehaviour())
			{
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_PLACING_1HD; 
			}
			else if (item.IsTwoHandedBehaviour())
			{
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_PLACING_2HD;
			}
			else
			{
				Debug.Log("Error: check " + item + " behaviour");
			}
		}
	}
};
