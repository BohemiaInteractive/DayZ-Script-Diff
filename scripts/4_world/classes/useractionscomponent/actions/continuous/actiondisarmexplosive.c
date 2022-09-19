class ActionDisarmExplosiveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DISARM_MINE);
	}
}

class ActionDisarmExplosive : ActionContinuousBase
{
	void ActionDisarmExplosive()
	{
		m_CallbackClass	= ActionDisarmExplosiveCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody 		= true;
		
		m_Text 			= "#disarm";
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTCursor();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
		{
			return false;
		}
		
		ExplosivesBase explosive;
		if (Class.CastTo(explosive, target.GetObject()) && item)
		{		
			return explosive.GetArmed() && explosive.CanBeDisarmed();
		}

		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		ExplosivesBase explosive;
		Class.CastTo(explosive, action_data.m_Target.GetObject());
		
		ToolBase tool;
		Class.CastTo(tool, action_data.m_MainItem);
		
		if (Math.RandomIntInclusive(0, 100) < tool.GetDisarmRate())
		{
			explosive.Disarm(true);
		}
		else
		{
			explosive.SetHealth("", "", 0.0);
		}
		
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, UADamageApplied.DEFUSE_TOOLS);
	}
}
