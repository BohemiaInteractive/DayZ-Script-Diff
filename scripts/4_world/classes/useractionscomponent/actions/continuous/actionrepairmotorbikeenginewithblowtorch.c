class ActionRepairMotorbikeEngineWithBlowtorchCB : ActionRepairCarEngineCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.BASEBUILDING_REPAIR_MEDIUM);
	}
}

class ActionRepairMotorbikeEngineWithBlowtorch: ActionRepairMotorbikeEngine
{
	void ActionRepairMotorbikeEngineWithBlowtorch()
	{
		m_CallbackClass 	= ActionRepairMotorbikeEngineWithBlowtorchCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_FLAME_REPAIR;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Blowtorch blowTorch;
		if (Class.CastTo(blowTorch, item))
			return super.ActionCondition(player, target, item) && blowTorch.HasEnoughEnergyForRepair(UATimeSpent.BASEBUILDING_REPAIR_MEDIUM);

		return false;
	}

	override void OnStartAnimationLoopServer(ActionData action_data)
	{
		super.OnStartAnimationLoopServer(action_data);

		action_data.m_MainItem.GetCompEM().SwitchOn();
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		action_data.m_MainItem.GetCompEM().SwitchOff();
	}
}
