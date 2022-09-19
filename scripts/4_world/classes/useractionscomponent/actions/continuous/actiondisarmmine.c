class ActionDisarmMineCB: ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DISARM_MINE);
	}
}

class ActionDisarmMine: ActionContinuousBase
{
	void ActionDisarmMine()
	{
		m_CallbackClass	= ActionDisarmMineCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody 		= true;
		
		m_Text 			= "#disarm";
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINonRuined;
		m_ConditionTarget 	= new CCTCursor;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (!target)
		{
			return false;
		}

		LandMineTrap targetMine;
		if (Class.CastTo(targetMine, target.GetObject()) && item && IsInReach(player, target, UAMaxDistances.DEFAULT))
		{		
			return targetMine.IsActive() && targetMine.CanBeDisarmed();
		}

		return false;
	}
	
	override void OnStartAnimationLoop(ActionData action_data)
	{
		if (!GetGame().IsMultiplayer() || GetGame().IsServer())
		{
			LandMineTrap targetMine = LandMineTrap.Cast(action_data.m_Target.GetObject());
			Param1<bool> play = new Param1<bool>(true);
			GetGame().RPCSingleParam(targetMine, SoundTypeMine.DISARMING, play, true);
		}
	}
	
	override void OnAnimationEvent(ActionData action_data)
	{
		if (!GetGame().IsMultiplayer() || GetGame().IsServer())
		{
			LandMineTrap targetMine = LandMineTrap.Cast(action_data.m_Target.GetObject());
			Param1<bool> play = new Param1<bool>(true);
			GetGame().RPCSingleParam(targetMine, SoundTypeMine.DISARMING, play, true);
		}
	}
	
	override void OnEndAnimationLoop( ActionData action_data )
	{
		if (!GetGame().IsMultiplayer() || GetGame().IsServer())
		{
			LandMineTrap targetMine = LandMineTrap.Cast(action_data.m_Target.GetObject());
			Param1<bool> play = new Param1<bool>(false);
			GetGame().RPCSingleParam(targetMine, SoundTypeMine.DISARMING, play, true);
		}
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		LandMineTrap targetMine;
		Class.CastTo(targetMine, action_data.m_Target.GetObject());
		
		ToolBase tool;
		Class.CastTo(tool, action_data.m_MainItem);
		
		if (Math.RandomIntInclusive(0, 100) < tool.GetDisarmRate())
		{
			targetMine.Disarm();
		}
		else
		{
			targetMine.OnSteppedOn(action_data.m_Player);
		}
		
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, 2);
	}
};