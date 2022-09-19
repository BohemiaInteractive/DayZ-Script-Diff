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
		
		if (bodyEAI && !bodyEAI.IsAlive() && (bodyEAI.IsInherited(DayZCreature) || bodyEAI.IsInherited(Man) || bodyEAI.IsInherited(DeadChicken_ColorBase) ) && !bodyEAI.GetParent())
		{
			int liquidType;
			string surfaceType;
			GetGame().SurfaceUnderObject(bodyEAI, surfaceType, liquidType);
			
			if (GetGame().IsSurfaceDigable(surfaceType))
			{
				return true;
			}
		}
		
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{	
		Object targetObject = action_data.m_Target.GetObject();
		g_Game.ObjectDelete(targetObject);

		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, 4);

		action_data.m_Player.GetSoftSkillsManager().AddSpecialty(m_SpecialtyWeight);
	}
};
