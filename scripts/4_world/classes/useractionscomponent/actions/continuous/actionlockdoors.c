class ActionLockDoorsCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.LOCK);
	}
};

class ActionLockDoors: ActionContinuousBase
{
	protected const float APPLIED_DMG = 6;
	
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
		if( !IsBuilding(target) ) return false;
		if( !IsInReach(player, target, UAMaxDistances.DEFAULT) ) return false;

		Building building;
		ToolBase tool;
		if (Class.CastTo(building, target.GetObject())&& Class.CastTo(tool, item))
		{
			int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
			
			// Check door for related lock inventory attachments
			AdditionalDoorInfo doorInfo = building.GetDoorInfo(doorIndex);
			if (doorInfo && doorInfo.m_RelatedInventorySlotNames)
			{
				foreach(string relatedSlotName: doorInfo.m_RelatedInventorySlotNames)
				{
					EntityAI attachmentEntity = building.FindAttachmentBySlotName(relatedSlotName);
					if (!attachmentEntity)
						continue;
					
					ItemBase attachment = ItemBase.Cast(attachmentEntity);
					if (attachment && attachment.IsExternalLockType())
						return false;
				}
			}
			
			if (doorIndex != -1 && tool.GetKeyCompatibilityType() & building.GetLockCompatibilityType(doorIndex))
				return building.CanDoorBeLocked(doorIndex);
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
		
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, APPLIED_DMG);
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
};