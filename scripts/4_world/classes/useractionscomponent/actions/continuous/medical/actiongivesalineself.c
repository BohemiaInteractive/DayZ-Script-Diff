class ActionGiveSalineSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.SALINE);
	}
};

class ActionGiveSalineSelf: ActionContinuousBase
{	
	void ActionGiveSalineSelf()
	{
		m_CallbackClass = ActionGiveSalineSelfCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAG;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#give_saline";
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

	override void OnFinishProgressServer( ActionData action_data )
	{		
		action_data.m_MainItem.TransferModifiers(action_data.m_Player);
		Param1<float> nacdata = Param1<float>.Cast( action_data.m_ActionComponent.GetACData() );		
		float delta = nacdata.param1;
		//action_data.m_Player.SetHealth("GlobalHealth", "Blood", action_data.m_Player.GetHealth("GlobalHealth", "Blood") + delta );
		//action_data.m_Player.AddHealth("", "Blood", delta);
		action_data.m_Player.GetModifiersManager().ActivateModifier(eModifiers.MDF_SALINE);
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );

		action_data.m_MainItem.Delete();
	}
};