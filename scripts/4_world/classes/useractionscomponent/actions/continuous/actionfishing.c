/*class ActionFishingCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousFish;
	}
};

class ActionFishing: ActionContinuousBase
{
	void ActionFishing()
	{
		m_CallbackClass = ActionFishingCB;
		m_Sounds.Insert("FishStruggling_0");
		m_Sounds.Insert("FishStruggling_1");
		m_Sounds.Insert("FishStruggling_2");
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_FISHING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override void CreateConditionComponents()  
	{
		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
		
	override string GetText()
	{
		return "#pull";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		FishingRod_Base nitem = FishingRod_Base.Cast( item );
		if ( nitem.IsFishingActive() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnEndServer( ActionData action_data )
	{
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};*/

	