class ActionRepositionPluggedItem : ActionInteractBase
{
	// Through this action players can reposition already placed electric devices without unplugging them from the power source.
	void ActionRepositionPluggedItem()
	{
		m_CallbackClass = ActionInteractBaseCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;

		m_Text = "#reposition";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool HasProgress()
	{
		return false;
	}
	
	override bool CanBeUsedOnBack()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ItemBase targetItem = ItemBase.Cast(target.GetObject());
		EntityAI targetParent = EntityAI.Cast(targetItem.GetHierarchyParent());
		if (!targetItem || !targetItem.IsTakeable() || targetItem.IsBeingPlaced() || (targetItem.GetCompEM() && !targetItem.GetCompEM().IsPlugged()) || ( targetItem.GetHierarchyParent() && !BaseBuildingBase.Cast(targetItem.GetHierarchyParent()) ) || ((targetParent && !targetItem.CanDetachAttachment(targetParent)) || (targetParent && !targetParent.CanReleaseAttachment(targetItem))))
			return false;

		if (targetItem.HasEnergyManager() && targetItem.GetCompEM().IsPlugged())
			return player.GetInventory().CanAddEntityIntoHands(targetItem);
		
		return false;
	}
	
	override bool CanContinue(ActionData action_data)
	{
		EntityAI ent = EntityAI.Cast(action_data.m_Target.GetObject());
		return ent && ent.GetCompEM().GetCordLength() > 0;
	}
	
	override void Start(ActionData action_data)
	{
		super.Start(action_data);

		ItemBase ntarget = ItemBase.Cast(action_data.m_Target.GetObject());
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
			action_data.m_Player.PlacingStartServer(ntarget);
		else
			action_data.m_Player.PlacingStartLocal(ntarget);
	}
	
	override void OnExecuteServer(ActionData action_data)
	{	
		ClearActionJuncture(action_data);
		
		if (GetGame().IsMultiplayer())
			return;
		
		InventoryLocation il = new InventoryLocation();
		ItemBase ntarget = ItemBase.Cast(action_data.m_Target.GetObject());
		ClearInventoryReservationEx(action_data);
		
		action_data.m_Player.PredictiveTakeEntityToHands(ntarget);
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		InventoryLocation il = new InventoryLocation();
		ItemBase ntarget = ItemBase.Cast(action_data.m_Target.GetObject());
		ClearInventoryReservationEx(action_data);
		
		action_data.m_Player.PredictiveTakeEntityToHands(ntarget);
	}
	
	override void CreateAndSetupActionCallback(ActionData action_data)
	{
		EntityAI target = EntityAI.Cast(action_data.m_Target.GetObject());
		ActionBaseCB callback;
		if (!target)
			return;
		
		if (target.IsHeavyBehaviour())
		{
			Class.CastTo(callback, action_data.m_Player.StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HEAVY,GetCallbackClassTypename(), DayZPlayerConstants.STANCEMASK_ERECT));
		}
		else
		{
			Class.CastTo(callback, action_data.m_Player.AddCommandModifier_Action(DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS,GetCallbackClassTypename()));
		}

		callback.SetActionData(action_data); 
		callback.InitActionComponent();
		action_data.m_Callback = callback;
	}

	//! DEPRECATED
	protected ItemBase m_SourceForReplug;
}
