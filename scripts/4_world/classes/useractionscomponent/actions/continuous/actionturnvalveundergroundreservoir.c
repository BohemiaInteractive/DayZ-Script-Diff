class ActionTurnValveUndergroundReservoirCB : ActionTurnValveCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(2);
	}
}

class ActionTurnValveUndergroundReservoir : ActionTurnValve
{
	
	void ActionTurnValveUndergroundReservoir()
	{
		m_CallbackClass = ActionTurnValveUndergroundReservoirCB;
	}
	
	override protected void CreateAndSetupActionCallback( ActionData action_data )
	{
		super.CreateAndSetupActionCallback(action_data);
		
		Land_Underground_WaterReservoir reservoir = Land_Underground_WaterReservoir.Cast(action_data.m_Target.GetObject());
		if (reservoir)
		{
			if (action_data.m_Callback)
			{
				array<vector> data = reservoir.GetValveAligningPointsWS(reservoir.GetTurnableValveIndex(action_data.m_Target.GetComponentIndex()));
				vector position = data[0];
				vector direction = data[1] - data[0];

				action_data.m_Callback.SetAligning(position, direction.Normalized());
			}
		}
	}

	override void OnStartAnimationLoopServer(ActionData action_data)
	{
		super.OnStartAnimationLoopServer(action_data);
	
		Land_Underground_WaterReservoir reservoir = Land_Underground_WaterReservoir.Cast(action_data.m_Target.GetObject());
		if (reservoir)
		{
			reservoir.OnValveManipulationStart(reservoir.GetTurnableValveIndex(action_data.m_Target.GetComponentIndex()));
		}
	}

	override void OnEndAnimationLoop(ActionData action_data)
	{
		super.OnEndAnimationLoop(action_data);

		Land_Underground_WaterReservoir reservoir = Land_Underground_WaterReservoir.Cast(action_data.m_Target.GetObject());
		if (reservoir)
		{
			reservoir.OnValveManipulationCanceled(reservoir.GetTurnableValveIndex(action_data.m_Target.GetComponentIndex()));
		}	
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		Land_Underground_WaterReservoir reservoir = Land_Underground_WaterReservoir.Cast(action_data.m_Target.GetObject());
		if (reservoir)
		{
			reservoir.OnValveManipulationEnd(reservoir.GetTurnableValveIndex(action_data.m_Target.GetComponentIndex()));
		}
	}
}
