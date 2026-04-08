class ActionOpenDoors: ActionInteractBase
{
	ref NoiseParams m_NoisePar;
	
	void ActionOpenDoors()
	{
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text 			= "#open";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTCursor();
	}

	protected bool CheckIfDoorIsLocked()
	{
		return true;
	}
	
	protected bool CreatesNoise()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target) 
			return false;

		if (!IsBuilding(target)) 
			return false;

		Building building;
		if (Class.CastTo(building, target.GetObject()))
		{
			int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
			if (doorIndex != -1)
			{
				if (!IsInReach(player, target, UAMaxDistances.DEFAULT)) 
					return false;
				
				return building.CanDoorBeOpened(doorIndex, CheckIfDoorIsLocked());

			}
		}

		return false;
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		Building building;
		if (Class.CastTo(building, action_data.m_Target.GetObject()))
		{
			int doorIndex = building.GetDoorIndex(action_data.m_Target.GetComponentIndex());
			if (doorIndex != -1)
			{
				if (building.CanDoorBeOpened(doorIndex, CheckIfDoorIsLocked()))
				{
					building.OpenDoor(doorIndex);
				}
			}
		}
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		m_NoisePar = new NoiseParams();
		m_NoisePar.LoadFromPath("CfgVehicles SurvivorBase NoiseActionDefault");
		NoiseSystem noise = g_Game.GetNoiseSystem();
		if (noise && CreatesNoise())
		{
			if (action_data.m_Player)
				noise.AddNoisePos(action_data.m_Player, action_data.m_Target.GetObject().GetPosition(), m_NoisePar, NoiseAIEvaluate.GetNoiseReduction(g_Game.GetWeather()));
		}
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
}

class ActionLockedDoors : ActionOpenDoors
{
	override bool CheckIfDoorIsLocked()
	{
		return false;
	}
	
	override protected bool CreatesNoise()
	{
		return false;
	}
};
