class ActionDefibrilateSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFIBRILATE);
	}
};

class ActionDefibrilateSelf: ActionDefibrilateBase
{	
	void ActionDefibrilateSelf()
	{
		m_CallbackClass = ActionDefibrilateSelfCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_HIGH;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#defibrilate_myself";
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
	
	override void OnFinishProgressClient( ActionData action_data )
	{	
		Defibrillator defib;
		PlayerBase player = action_data.m_Player;
		Class.CastTo(defib, action_data.m_MainItem);
		
		DefibrillateClient(action_data.m_Player, defib);
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		Defibrillator defib;
		PlayerBase player = action_data.m_Player;
		Class.CastTo(defib, action_data.m_MainItem);
		
		DefibrillateServer(action_data.m_Player, defib);
	}
};
