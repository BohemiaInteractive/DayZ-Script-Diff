class ActionDisinfectTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT);
	}

};

class ActionDisinfectTarget: ActionDisinfectBase
{	
	void ActionDisinfectTarget()
	{
		m_CallbackClass = ActionDisinfectSelfCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#disinfect_target";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( target)
		{
			PlayerBase target_player = PlayerBase.Cast(target.GetObject());
			if(target_player)
				return (target_player.IsBleeding() || (player.m_SyncedModifiers & eModifierSyncIDs.MODIFIER_SYNC_WOUND_INFECT_1) || (player.m_SyncedModifiers & eModifierSyncIDs.MODIFIER_SYNC_WOUND_INFECT_2));
			
		}
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlayerBase target = PlayerBase.Cast(action_data.m_Target.GetObject());
		
		if( target )
		{
			target.GetModifiersManager().ActivateModifier(eModifiers.MDF_DISINFECTION);
			Apply(action_data);
		}
	}
};