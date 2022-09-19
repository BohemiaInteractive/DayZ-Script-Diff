class ActionMineRockCB : ActionContinuousBaseCB
{
	private const float TIME_BETWEEN_MATERIAL_DROPS = 8;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousMineRock(TIME_BETWEEN_MATERIAL_DROPS);
	}
};

class ActionMineRock: ActionMineBase
{
	void ActionMineRock()
	{
		m_CallbackClass = ActionMineRockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_MINEROCK;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		//m_Text = "#mine";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		Object targetObject = target.GetObject();
		if ( targetObject.IsRock() )
		{
			return true;
		}
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};

class ActionMineRock1H : ActionMineRock
{
	void ActionMineRock1H()
	{
		m_CallbackClass = ActionMineRockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
	}
}