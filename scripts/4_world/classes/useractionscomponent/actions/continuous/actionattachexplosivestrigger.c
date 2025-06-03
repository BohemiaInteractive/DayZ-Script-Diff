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

		m_Text = "#STR_ArmExplosive";
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
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		ExplosivesBase item = ExplosivesBase.Cast(action_data.m_Target.GetObject());
		if (item.GetArmSoundset() != string.Empty)
			item.StartItemSoundServer(SoundConstants.ITEM_EXPLOSIVE_ARM);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		ExplosivesBase item = ExplosivesBase.Cast(action_data.m_Target.GetObject());
		if (item.GetArmSoundset() != string.Empty)
			item.StopItemSoundServer(SoundConstants.ITEM_EXPLOSIVE_ARM);
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);
		
		ExplosivesBase explosive = ExplosivesBase.Cast(action_data.m_Target.GetObject());
		if (explosive)
		{
			explosive.OnPlacementComplete(action_data.m_Player, explosive.GetPosition(), action_data.m_Player.GetOrientation());
			explosive.UnlockTriggerSlots();
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
		}
		
		if (!action_data.m_MainItem.IsInherited(RemoteDetonator))
		{
			AttachItem(action_data);
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
			explosive.UnlockTriggerSlots();
			RemoteDetonatorTrigger.SpawnInPlayerHands(action_data.m_Player, explosive);
		}
	}
	
	protected void AttachItem(ActionData action_data)
	{
		EntityAI targetEAI;	
		if (action_data.m_Target.IsProxy())
		{
			targetEAI = EntityAI.Cast(action_data.m_Target.GetParent());
		}
		else
		{
			targetEAI = EntityAI.Cast(action_data.m_Target.GetObject());
		}
		
		if (targetEAI && action_data.m_MainItem)
		{
			int slotId = InventorySlots.GetSlotIdFromString(action_data.m_MainItem.GetExplosiveTriggerSlotName());
			action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(targetEAI, action_data.m_MainItem, slotId);
		}
	}
}