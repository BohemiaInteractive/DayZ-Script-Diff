class ActionRepairItemWithBlowtorchCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BASEBUILDING_REPAIR_MEDIUM);
	}
}

class ActionRepairItemWithBlowtorch : ActionContinuousBase 
{
	void ActionRepairItemWithBlowtorch()
	{
		m_CallbackClass 	= ActionRepairItemWithBlowtorchCB;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;

		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_FLAME_REPAIR;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody 			= true;
		m_LockTargetOnUse	= false;
		m_Text 				= "#repair";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTCursor(UAMaxDistances.REPAIR);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		PluginRepairing moduleRepairing;
		Class.CastTo(moduleRepairing, GetPlugin(PluginRepairing));
		Blowtorch bt = Blowtorch.Cast(item);

		return bt.HasEnoughEnergyForRepair(UATimeSpent.BASEBUILDING_REPAIR_MEDIUM) && moduleRepairing && moduleRepairing.CanRepair(item, target.GetObject());
	}

	override void OnStartAnimationLoopServer(ActionData action_data)
	{
		super.OnStartAnimationLoopServer(action_data);

		action_data.m_MainItem.GetCompEM().SwitchOn();
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		
		ItemBase targetItem = ItemBase.Cast(action_data.m_Target.GetObject());
		if (targetItem) 
		{
			PluginRepairing module_repairing;
			Class.CastTo(module_repairing, GetPlugin(PluginRepairing));
			module_repairing.Repair(action_data.m_Player, action_data.m_MainItem, targetItem, m_SpecialtyWeight);
		}
		
		action_data.m_MainItem.GetCompEM().SwitchOff();
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		action_data.m_MainItem.GetCompEM().SwitchOff();
	}
}
