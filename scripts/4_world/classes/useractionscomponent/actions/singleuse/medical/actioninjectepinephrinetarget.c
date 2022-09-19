class ActionInjectEpinephrineTarget: ActionInjectTarget
{
	void ActionInjectEpinephrineTarget()
	{
	}
		
	override void ApplyModifiers( ActionData action_data )
	{
		action_data.m_MainItem.OnApply( PlayerBase.Cast(action_data.m_Target.GetObject()));
		//PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		//ntarget.GiveShock(100);
	}
};