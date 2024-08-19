class ActionBuryAshesCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BURY_ASHES);
	}
}

class ActionBuryAshes: ActionContinuousBase
{
	void ActionBuryAshes()
	{
		m_CallbackClass		= ActionBuryAshesCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_DIGMANIPULATE;
		m_FullBody			= true;
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight	= UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#bury";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget	= new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem		= new CCINonRuined();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.IsPlacingLocal())
		{
			return false;
		}
		
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			return false;
		}
		
		Fireplace fireplaceTarget = Fireplace.Cast(target.GetObject());
		if (fireplaceTarget)
		{
			if (fireplaceTarget.HasAshes() && !fireplaceTarget.IsBurning() && fireplaceTarget.IsEmpty())
			{
				int liquidType;
				string surfaceType;
				GetGame().SurfaceUnderObject(fireplaceTarget, surfaceType, liquidType);
				if (GetGame().IsSurfaceDigable(surfaceType))
				{
					return true;
				}
			}
		}
		
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		//destroy fireplace with ashes
		GetGame().ObjectDelete(action_data.m_Target.GetObject());

		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, 4);
		
		//add soft skill specialty
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty(UASoftSkillsWeight.ROUGH_LOW);
	}
}