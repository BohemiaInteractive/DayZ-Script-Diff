class ActionCraftArmbandCB: ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BANDAGE);
	}
}

class ActionCraftArmband: ActionContinuousBase
{
	void ActionCraftArmband()
	{
		m_CallbackClass 	= ActionCraftArmbandCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;
		m_Text 				= "#STR_craftarmband0";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget	= new CCTSelf();
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return true;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		EntityAI armband = action_data.m_Player.SpawnEntityOnGroundRaycastDispersed("Armband_White");
		action_data.m_MainItem.AddQuantity(-1);
		
		MiscGameplayFunctions.TransferItemProperties(action_data.m_MainItem, armband);
	}
	
	override string GetSoundCategory(ActionData action_data)
	{
		return "ImprovisedCloth_craft";
	}
}
