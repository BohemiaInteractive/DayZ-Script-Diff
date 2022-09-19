class ActionCraftImprovisedFeetCoverCB : ActionContinuousBaseCB
{
	private const float TIME_TO_CRAFT_CLOTHES = 5.0;
	
	override void CreateActionComponent()
	{
		//float adjusted_time = m_ActionData.m_Player.GetSoftSkillsManager().AdjustCraftingTime(TIME_TO_CRAFT_CLOTHES,UASoftSkillsWeight.ROUGH_HIGH);	
		m_ActionData.m_ActionComponent = new CAContinuousTime(TIME_TO_CRAFT_CLOTHES);
	}
};

class ActionCraftImprovisedFeetCover: ActionContinuousBase
{
	void ActionCraftImprovisedFeetCover()
	{
		m_CallbackClass = ActionCraftImprovisedFeetCoverCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#STR_CraftImprovisedFeetCover";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( item.GetQuantity() >= 2 )
		{
			return true;
		}
		return false;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		EntityAI item_ingredient = action_data.m_MainItem;
		EntityAI cover;
		
		cover = action_data.m_Player.SpawnEntityOnGroundPos("FeetCover_Improvised", action_data.m_Player.GetPosition());
		action_data.m_MainItem.AddQuantity(-2);
		
		MiscGameplayFunctions.TransferItemProperties(item_ingredient, cover);
	}
};
