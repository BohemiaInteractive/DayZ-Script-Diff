class ActionCraftBoneKnifeCB : ActionContinuousBaseCB
{	
	override void CreateActionComponent()
	{	
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.DEFAULT_CRAFT);
	}
};

class ActionCraftBoneKnife: ActionContinuousBase
{
	void ActionCraftBoneKnife()
	{
		m_CallbackClass = ActionCraftBoneKnifeCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#STR_CraftBoneKnife0";
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
		EntityAI knife = action_data.m_Player.SpawnEntityOnGroundRaycastDispersed("BoneKnife");
		action_data.m_MainItem.AddQuantity(-2);
		
		MiscGameplayFunctions.TransferItemProperties(action_data.m_MainItem, knife);
	}
	
	override string GetSoundCategory(ActionData action_data)
	{
		return "BoneKnife_craft";
	}
};
