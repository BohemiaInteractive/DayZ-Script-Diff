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

		BuildingBase building;
		if (Class.CastTo(building, target.GetObject()))
		{
			DoorManipulationParams doorParams = new DoorManipulationParams();
			doorParams.m_DoorIndex = building.GetDoorIndex(target.GetComponentIndex());
			if (doorParams.m_DoorIndex != -1)
			{
				if (!IsInReach(player, target, UAMaxDistances.DEFAULT)) 
					return false;
				
				if (!building.DoorsDirectionalCheck(player,target.GetComponentIndex()))
					return false;
				
				doorParams.m_CheckIfLocked = CheckIfDoorIsLocked();
				doorParams.m_Caller = player;

				return building.CanDoorBeOpened(doorParams);
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
			DoorManipulationParams doorParams = new DoorManipulationParams();
			doorParams.m_DoorIndex = building.GetDoorIndex(action_data.m_Target.GetComponentIndex());
			if (doorParams.m_DoorIndex != -1)
			{
				doorParams.m_CheckIfLocked = CheckIfDoorIsLocked();
				doorParams.m_Caller = action_data.m_Player;
				if (building.CanDoorBeOpened(doorParams))
					building.OpenDoor(doorParams.m_DoorIndex);
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
}
