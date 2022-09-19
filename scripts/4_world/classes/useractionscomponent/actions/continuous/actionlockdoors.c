class ActionLockDoorsCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.LOCK);
	}
};

class ActionLockDoors: ActionContinuousBase
{
	void ActionLockDoors()
	{
		m_CallbackClass = ActionLockDoorsCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_HIGH;
		m_Text = "#lock_door";
	}
	
	override void CreateConditionComponents()
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( !target ) return false;
		//if( IsDamageDestroyed(action_data.m_Target) ) return false;
		if( !IsBuilding(target) ) return false;
		if( !IsInReach(player, target, UAMaxDistances.DEFAULT) ) return false;

		Building building;
		if( Class.CastTo(building, target.GetObject()) )
		{
			int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
			if ( doorIndex != -1 )
				return (!building.IsDoorOpen(doorIndex) && !building.IsDoorLocked(doorIndex));
		}		
		return false;
	}

	protected void LockDoor(ActionTarget target)
	{
		Building building;

		if ( Class.CastTo(building, target.GetObject()) )
		{
			int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
			if ( doorIndex != -1 )
			{
				building.LockDoor(doorIndex);
			}		
		}
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		LockDoor(action_data.m_Target);
		
		//Damage the Lockpick
		//float dmg = action_data.m_MainItem.GetMaxHealth() * 0.04; //Multiply max health by 'x' amount depending on number of usages wanted (0.04 = 25)
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		
		float skillLevel = action_data.m_Player.GetSoftSkillsManager().GetSpecialtyLevel();
		float appliedDamage = 5 + 2*skillLevel; 
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, appliedDamage);
	}
};