class ActionTurnOnWeaponFlashlight: ActionSingleUseBase
{
	ItemBase m_flashlight;
	
	void ActionTurnOnWeaponFlashlight()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;//CMD_ACTIONMOD_INTERACTONCE
		m_Text = "#switch_on";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( item.IsInherited(Rifle_Base) )
		{
			//m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_LIGHTFLARE;
			m_flashlight = ItemBase.Cast(item.FindAttachmentBySlotName("weaponFlashlight"));
		}
		else if (item.IsInherited(Pistol_Base))
		{
			//m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_LITCHEMLIGHT;
			m_flashlight = ItemBase.Cast(item.FindAttachmentBySlotName("pistolFlashlight"));
		}
		else //is not valid item
			return false;
		
		if ( m_flashlight && m_flashlight.HasEnergyManager() && m_flashlight.GetCompEM().CanSwitchOn() && m_flashlight.GetCompEM().CanWork() ) //TODO review conditions for turning off
		{
			return true;
		}
		
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		if ( m_flashlight.HasEnergyManager() )
		{
			if ( m_flashlight.GetCompEM().CanWork() )
			{
				m_flashlight.GetCompEM().SwitchOn();
			}
			Weapon_Base.Cast(action_data.m_MainItem).FlashlightOn();  //currently seems to be doing nothing
		}
	}
};
