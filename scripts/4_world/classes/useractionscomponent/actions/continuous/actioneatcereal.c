class ActionEatCereal: ActionEat
{
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		super.OnFinishProgressServer(action_data);
		
		float compassChance = 0.1;
		if (Math.RandomFloatInclusive(0.0, 1.0) < compassChance)
		{
			action_data.m_Player.SpawnEntityOnGroundRaycastDispersed("OrienteeringCompass");
		}
	}
};