class ActionDefibrilateTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFIBRILATE);
	}
};

class ActionDefibrilateTarget: ActionDefibrilateBase
{	
	void ActionDefibrilateTarget()
	{
		m_CallbackClass = ActionDefibrilateTargetCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_HIGH;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#defibrilate_target";
	}
	
	override void CreateConditionComponents()  
	{	
		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}

	override void OnFinishProgressClient( ActionData action_data )
	{	
		Defibrillator defib;
		PlayerBase player = action_data.m_Player;
		Class.CastTo(defib, action_data.m_MainItem);
		
		DefibrillateClient(action_data.m_Player, defib);
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		Defibrillator defib = Defibrillator.Cast( action_data.m_MainItem );
		PlayerBase target_player = PlayerBase.Cast( action_data.m_Target.GetObject() );
		
		DefibrillateServer(target_player, defib);
	}
};
