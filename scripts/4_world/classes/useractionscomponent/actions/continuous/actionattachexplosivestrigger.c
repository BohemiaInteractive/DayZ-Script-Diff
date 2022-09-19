class ActionAttachExplosivesTriggerCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.EXPLOSIVE_ARM);
	}
}

class ActionAttachExplosivesTrigger : ActionContinuousBase
{
	void ActionAttachExplosivesTrigger()
	{
		m_CallbackClass		= ActionAttachExplosivesTriggerCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody			= true;
		m_SpecialtyWeight	= UASoftSkillsWeight.PRECISE_LOW;
		m_Text 				= "#STR_ArmExplosive";
	}

	override void CreateConditionComponents() 
	{
		m_ConditionTarget 	= new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem 	= new CCINonRuined();
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		return true;
	}
	
	override bool CanBeSetFromInventory()
	{
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (target.GetParent())
		{
			return false;
		}
		
		if (ClaymoreMine.Cast(target.GetObject()))
		{
			return false;
		}
		
		ExplosivesBase explosive = ExplosivesBase.Cast(target.GetObject());
		if (explosive)
		{
			ClockBase timedTrigger;
			if (ClockBase.CastTo(timedTrigger, item))
			{
				return timedTrigger.IsAlarmOn() && explosive.IsTimerDetonable() && explosive.CanBeArmed();
			}
			
			return explosive.CanBeArmed();		
		}
		

		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);
		
		ExplosivesBase explosive = ExplosivesBase.Cast(action_data.m_Target.GetObject());
		if (explosive)
		{
			explosive.UnlockTriggerSlots();
			explosive.SetTakeable(false);  //Ensures the item is not picked by server or remotes before arming
		}
		
		if (action_data.m_MainItem.IsInherited(RemoteDetonator))
		{
			CreateRemoteDetonatorReceiverAsAttachment(action_data);
			
			return;
		}
		else
		{
			AttachItem(action_data);
		}
	}
	
	override void OnFinishProgressClient(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);

		ExplosivesBase explosive = ExplosivesBase.Cast(action_data.m_Target.GetObject());		
		if (explosive)
		{	
			explosive.UnlockTriggerSlots();
			explosive.SetTakeable(false); //Ensures the item is not picked by client before arming
		}
		
		if (!action_data.m_MainItem.IsInherited(RemoteDetonator))
		{
			AttachItem(action_data);
		}
	}
	
	override void OnEndServer(ActionData action_data)
	{
		ExplosivesBase explosive = ExplosivesBase.Cast(action_data.m_Target.GetObject());

		RemoteDetonatorTrigger trigger = RemoteDetonatorTrigger.Cast(action_data.m_Player.GetItemInHands());
		if (trigger)
		{
			if (explosive)
			{
				explosive.PairRemote(trigger);
 				trigger.SetControlledDevice(explosive);
				explosive.Arm();
			}
		}
	}
	
	protected void CreateRemoteDetonatorReceiverAsAttachment(ActionData action_data)
	{
		ExplosivesBase explosive;
		if (action_data.m_Target.IsProxy())
		{
			explosive = ExplosivesBase.Cast(action_data.m_Target.GetParent());
		}
		else
		{
			explosive = ExplosivesBase.Cast(action_data.m_Target.GetObject());
		}
		
		if (explosive && action_data.m_MainItem)
		{		
			ItemBase receiver = ItemBase.Cast(explosive.GetInventory().CreateAttachment("RemoteDetonatorReceiver"));
			if (receiver)
			{
				receiver.LockToParent();
				RemoteDetonatorTrigger.SpawnInPlayerHands(action_data.m_Player);
			}
		}
	}
	
	protected void AttachItem(ActionData action_data)
	{
		EntityAI target_EAI;	
		if (action_data.m_Target.IsProxy())
		{
			target_EAI = EntityAI.Cast(action_data.m_Target.GetParent());
		}
		else
		{
			target_EAI = EntityAI.Cast(action_data.m_Target.GetObject());
		}
		
		if (target_EAI && action_data.m_MainItem)
		{
			int slot_id = InventorySlots.GetSlotIdFromString(action_data.m_MainItem.GetExplosiveTriggerSlotName());
			bool test = action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(target_EAI, action_data.m_MainItem, slot_id);
		}
	}
}