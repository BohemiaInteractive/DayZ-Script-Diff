class ActionCloseDoors: ActionInteractBase
{
	ref NoiseParams m_NoisePar;
	
	void ActionCloseDoors()
	{
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text 			= "#close";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTCursor();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
			return false;

		if (!IsBuilding(target))
			return false;
		
		BuildingBase building;
		if (Class.CastTo(building, target.GetObject()))
		{
			int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
			if (doorIndex != -1)
			{
				if (!IsInReach(player, target, UAMaxDistances.DEFAULT))
					return false;
				
				if (!building.DoorsDirectionalCheck(player,target.GetComponentIndex()))
					return false;

				return building.CanDoorBeClosed(doorIndex);
			}
		}		

		return false;
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		BuildingBase building;
		if (Class.CastTo(building, action_data.m_Target.GetObject()))
		{
			int doorIndex = building.GetDoorIndex(action_data.m_Target.GetComponentIndex());
			if (doorIndex != -1)
			{
				if (building.CanDoorBeClosed(doorIndex))
				{
					building.CloseDoor(doorIndex);
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
		if (noise)
		{
			if (action_data.m_Player)
			{
				Object targetObj = action_data.m_Target.GetObject();
				noise.AddNoisePos(action_data.m_Player, action_data.m_Player.GetPosition(), m_NoisePar);
			}
		}
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
}
