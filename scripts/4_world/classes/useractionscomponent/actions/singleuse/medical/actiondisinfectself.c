class ActionDisinfectSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT);
	}
};

class ActionDisinfectBase: ActionContinuousBase
{
	void Apply( ActionData action_data )
	{
		ItemBase item = action_data.m_MainItem;
		item.AddQuantity(-item.GetDisinfectQuantity());
		
	}
}


class ActionDisinfectSelf: ActionDisinfectBase
{
	float m_GramsConsumedPerUse;//left for legacy reasons
	
	void ActionDisinfectSelf()
	{
		m_CallbackClass = ActionDisinfectSelfCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_STITCHUPSELF;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#disinfect_self";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return (player.IsBleeding() || (player.m_SyncedModifiers & eModifierSyncIDs.MODIFIER_SYNC_WOUND_INFECT_1) || (player.m_SyncedModifiers & eModifierSyncIDs.MODIFIER_SYNC_WOUND_INFECT_2));
	}
	

	override void OnFinishProgressServer( ActionData action_data )
	{
		
		if( action_data.m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_DISINFECTION))//effectively resets the timer
		{
			action_data.m_Player.GetModifiersManager().DeactivateModifier( eModifiers.MDF_DISINFECTION );
		}
		action_data.m_Player.GetModifiersManager().ActivateModifier( eModifiers.MDF_DISINFECTION );

		Apply(action_data);
	}

	
};