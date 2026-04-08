class ActionMineBushCB : ActionContinuousBaseCB
{
	protected const float TIME_BETWEEN_MATERIAL_DROPS_DEFAULT = 3;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousMineWood(TIME_BETWEEN_MATERIAL_DROPS_DEFAULT);
	}
};

class ActionMineBush : ActionMineBase
{
	void ActionMineBush()
	{
		m_CallbackClass = ActionMineBushCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_HACKBUSH;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTCursor(UAMaxDistances.SMALL);
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( g_Game.IsMultiplayer() && g_Game.IsServer() )
			return true;
		
		Object targetObject = target.GetObject();
		return targetObject.IsBush() && targetObject.IsCuttable();
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		MineActionData mad = MineActionData.Cast(action_data);
		if (mad.m_EffecterID == -1)
		{
			mad.m_EffecterID = SEffectManager.CreateParticleServer(action_data.m_Target.GetObject().GetPosition(), new TreeEffecterParameters("TreeEffecter", EffecterBase.NOT_DEFINED_LIFESPAN, 0.1));
		}
		else
		{
			SEffectManager.ReactivateParticleServer(mad.m_EffecterID);
		}
	}
	
	override bool HasMultipleExecution(ActionData action_data)
	{
		return true;
	}
};