class ActionDeCraftRopeBeltCB : ActionContinuousBaseCB
{
	private const float TIME_TO_CRAFT_CLOTHES = 5.0;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(TIME_TO_CRAFT_CLOTHES);
	}
};

class ActionDeCraftRopeBelt: ActionContinuousBase
{
	void ActionDeCraftRopeBelt()
	{
		m_CallbackClass = ActionDeCraftRopeBeltCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#STR_DeCraftRopeBelt";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{		
		if(item.GetInventory().AttachmentCount() > 0)
		{
			return false;
		}
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{		
		EntityAI item_ingredient = action_data.m_MainItem;
		EntityAI belt;
		
		belt = action_data.m_Player.SpawnEntityOnGroundPos("Rope", action_data.m_Player.GetPosition());
		action_data.m_MainItem.Delete();
		
		MiscGameplayFunctions.TransferItemProperties(item_ingredient, belt);
		
	}
};
