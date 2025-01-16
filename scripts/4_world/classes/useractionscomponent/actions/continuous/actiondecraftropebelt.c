class ActionDeCraftRopeBeltCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_DECRAFT);
	}

	//! DEPRECATED
	private const float TIME_TO_CRAFT_CLOTHES = 5.0;
}

class ActionDeCraftRopeBelt : ActionContinuousBase
{
	void ActionDeCraftRopeBelt()
	{
		m_CallbackClass 	= ActionDeCraftRopeBeltCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_HIGH;

		m_Text = "#STR_DeCraftRopeBelt";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNone();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return item.GetInventory().AttachmentCount() == 0;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{		
		EntityAI ropebelt = action_data.m_MainItem;
		EntityAI rope = action_data.m_Player.SpawnEntityOnGroundRaycastDispersed("Rope");
		action_data.m_MainItem.Delete();
		
		MiscGameplayFunctions.TransferItemProperties(ropebelt, rope);
	}
}
