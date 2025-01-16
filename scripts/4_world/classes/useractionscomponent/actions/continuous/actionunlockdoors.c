class ActionUnlockDoorsCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNLOCK);
	}
};

class ActionUnlockDoors: ActionContinuousBase
{
	protected const float APPLIED_DMG = 6;
	
	void ActionUnlockDoors()
	{
		m_CallbackClass = ActionUnlockDoorsCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#unlock";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( !IsBuilding(target) ) return false;
		if( !IsInReach(player, target, UAMaxDistances.DEFAULT) ) return false;
		
		Building building;
		ToolBase tool;
		if (Class.CastTo(building, target.GetObject())&& Class.CastTo(tool, item))
		{
			int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
			if (doorIndex != -1 && tool.GetKeyCompatibilityType() & building.GetLockCompatibilityType(doorIndex))
				return building.IsDoorLocked(doorIndex);
		}
		return false;
	}

	protected void UnlockDoor(ActionTarget target)
	{
		Building building;

		if ( Class.CastTo(building, target.GetObject()) )
		{
			int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
			if ( doorIndex != -1 )
			{
				building.UnlockDoor(doorIndex);
			}
		}
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		UnlockDoor(action_data.m_Target);
		
		//Damage the Lockpick
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, APPLIED_DMG);
		
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
};