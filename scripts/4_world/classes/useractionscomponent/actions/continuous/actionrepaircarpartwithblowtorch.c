class ActionRepairCarPartWithBlowtorchCB : ActionRepairCarPartCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BASEBUILDING_REPAIR_MEDIUM);
	}
}

class ActionRepairCarPartWithBlowtorch : ActionRepairCarPart 
{
	void ActionRepairCarPartWithBlowtorch()
	{
		m_CallbackClass 	= ActionRepairCarPartWithBlowtorchCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_FLAME_REPAIR;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTCursor(UAMaxDistances.REPAIR);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Blowtorch bt = Blowtorch.Cast(item);
		if (!bt.HasEnoughEnergyForRepair(UATimeSpent.BASEBUILDING_REPAIR_MEDIUM))
		{
			return false;
		}
		
		return super.ActionCondition(player, target, item);
	}

	override void OnStartAnimationLoopServer(ActionData action_data)
	{
		super.OnStartAnimationLoopServer(action_data);

		action_data.m_MainItem.GetCompEM().SwitchOn();
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		
		action_data.m_MainItem.GetCompEM().SwitchOff();
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		action_data.m_MainItem.GetCompEM().SwitchOff();
	}
}
