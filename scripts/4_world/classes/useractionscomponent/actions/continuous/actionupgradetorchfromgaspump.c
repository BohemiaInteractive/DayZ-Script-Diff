class ActionUpgradeTorchFromGasPumpCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.ADD_FUEL_TO_TORCH);
	}
};

class ActionUpgradeTorchFromGasPump: ActionContinuousBase
{	
	void ActionUpgradeTorchFromGasPump()
	{
		m_CallbackClass = ActionUpgradeTorchFromGasPumpCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#str_upgrade_torch_fuel";
	}
	
	override void CreateConditionComponents()  
	{	
		
		m_ConditionTarget = new CCTDummy;
		m_ConditionItem = new CCINonRuined;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Land_FuelStation_Feed fuelstation = Land_FuelStation_Feed.Cast(target.GetObject());
		Torch torch = Torch.Cast(item);
		
		if ( fuelstation  &&  fuelstation.HasFuelToGive() )
		{
			return torch.CanReceiveUpgrade();
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		Torch torch = Torch.Cast(action_data.m_MainItem);
		
		if (torch)
		{
			//torch.ConsumeRag();
			torch.Upgrade(null);
		}
	}
};