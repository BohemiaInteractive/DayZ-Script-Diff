class ActionBuryBodyCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BURY_BODY);
	}
};

class ActionBuryBody: ActionContinuousBase
{	
	void ActionBuryBody()
	{
		m_CallbackClass		= ActionBuryBodyCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_DIGMANIPULATE;
		m_FullBody			= true;
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight	= UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#bury";
	}
	
	override void CreateConditionComponents()  
	{	
		
		m_ConditionTarget	= new CCTDummy();
		m_ConditionItem		= new CCINonRuined();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.IsPlacingLocal())
		{
			return false;
		}
		
		EntityAI bodyEAI;
		Class.CastTo(bodyEAI, target.GetObject());
		
		if (bodyEAI && (bodyEAI.IsCorpse() || (!bodyEAI.IsAlive() && (bodyEAI.IsInherited(DayZCreature) || bodyEAI.IsInherited(Man)))) && !bodyEAI.GetParent())
		{
			int liquidType;
			string surfaceType;
			g_Game.SurfaceUnderObject(bodyEAI, surfaceType, liquidType);
			
			if (g_Game.IsSurfaceDigable(surfaceType))
			{
				return true;
			}
		}
		
		return false;
	}
	
	override bool Can(PlayerBase player, ActionTarget target, ItemBase item, int condition_mask)
	{
		if (!super.Can(player, target, item, condition_mask))
			return false;
		
		return player.CheckFreeSpace(vector.Forward, 1.0, false);
	}


	override void OnFinishProgressServer(ActionData action_data)
	{	
		Object targetObject = action_data.m_Target.GetObject();
		g_Game.ObjectDelete(targetObject);

		MiscGameplayFunctions.DealEvinronmentAdjustedDmg(action_data.m_MainItem, action_data.m_Player, 4);
	}
};
