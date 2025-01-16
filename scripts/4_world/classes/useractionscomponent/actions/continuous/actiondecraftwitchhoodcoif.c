class ActionDeCraftWitchHoodCoifCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_DECRAFT);
	}
}

class ActionDeCraftWitchHoodCoif : ActionContinuousBase
{
	void ActionDeCraftWitchHoodCoif()
	{
		m_CallbackClass 	= ActionDeCraftWitchHoodCoifCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_HIGH;

		m_Text = "#dismantle";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNone();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		EntityAI mainItem = action_data.m_MainItem;
		
		string color = mainItem.ConfigGetString("color");
		string className = "WitchHood_" + color;
		
		EntityAI result1 = action_data.m_Player.SpawnEntityOnGroundRaycastDispersed(className);
		EntityAI result2 = action_data.m_Player.SpawnEntityOnGroundRaycastDispersed("Chainmail_Coif");
		
		if (!result1 || !result2)
			return;
		
		MiscGameplayFunctions.TransferItemProperties(mainItem, result1);
		MiscGameplayFunctions.TransferItemProperties(mainItem, result2);
		
		action_data.m_MainItem.Delete();
	}
}
