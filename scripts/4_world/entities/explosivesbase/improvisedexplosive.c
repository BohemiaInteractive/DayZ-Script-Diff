class ImprovisedExplosive : ExplosivesBase
{
	protected const float TIME_TRIGGER_INITIAL_DELAY_SECS 			= 0.1;
	protected const float TIME_TRIGGER_TIMER_BASED_DELAY_SECS 		= 1.0;
	protected const float TIME_TRIGGER_DELAY_SECS 					= 0.3;
	
	protected static const string SLOT_TRIGGER_ALARM_CLOCK	 		= "TriggerAlarmClock";
	protected static const string SLOT_TRIGGER_KITCHEN_TIMER		= "TriggerKitchenTimer";
	protected static const string SLOT_TRIGGER_REMOTE				= "TriggerRemoteDetonator_Receiver";
	
	protected static const string SLOT_EXPLOSIVE_A					= "IEDExplosiveA";
	protected static const string SLOT_EXPLOSIVE_B					= "IEDExplosiveB";
	
	protected const int SLOT_EXPLOSIVE_COUNT						= 2;
	protected const string SLOT_EXPLOSIVES[SLOT_EXPLOSIVE_COUNT] 	= {
		SLOT_EXPLOSIVE_A,
		SLOT_EXPLOSIVE_B
	};
	
	protected const int SLOT_TRIGGERS_COUNT							= 3;
	protected const string SLOT_TRIGGERS[SLOT_TRIGGERS_COUNT]		= {
		SLOT_TRIGGER_ALARM_CLOCK,
		SLOT_TRIGGER_KITCHEN_TIMER,
		SLOT_TRIGGER_REMOTE
	};
	
	protected const string ANIM_PHASE_TRIGGER_EMPTY					= "TriggerEmpty";
	protected const string ANIM_PHASE_TRIGGER_TIMER					= "TriggerTimer";
	protected const string ANIM_PHASE_TRIGGER_CLOCK					= "TriggerClock";
	protected const string ANIM_PHASE_TRIGGER_REMOTE				= "TriggerRemote";
	
	protected ref RemotelyActivatedItemBehaviour m_RAIB;
	
	void ImprovisedExplosive()
	{
		m_RAIB = new RemotelyActivatedItemBehaviour(this);

		RegisterNetSyncVariableInt("m_RAIB.m_PairDeviceNetIdLow");
		RegisterNetSyncVariableInt("m_RAIB.m_PairDeviceNetIdHigh");
	}

	override void EOnInit(IEntity other, int extra)
	{
		LockTriggerSlots();
	}
	
	override bool HasLockedTriggerSlots()
	{
		for (int i = 0; i < SLOT_TRIGGERS_COUNT; i++)
		{
			return GetInventory().GetSlotLock(InventorySlots.GetSlotIdFromString(SLOT_TRIGGERS[i]));
		}
		
		return false;
	}
	
	override void LockTriggerSlots()
	{
		for (int i = 0; i < SLOT_TRIGGERS_COUNT; i++)
		{
			GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(SLOT_TRIGGERS[i]), true);
		}
	}
	
	override void UnlockTriggerSlots()
	{
		for (int i = 0; i < SLOT_TRIGGERS_COUNT; i++)
		{
			GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(SLOT_TRIGGERS[i]), false);
		}
	}
	
 	override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
		
		if (m_RAIB)
		{
			m_RAIB.OnVariableSynchronized();
		}

		for (int i = 0; i < SLOT_TRIGGERS_COUNT; i++)
		{
			EntityAI trigger = GetInventory().FindAttachmentByName(SLOT_TRIGGERS[i]);
			if (trigger)
			{
				UpdateVisuals(trigger);
				break;
			}
		}
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);

		if (m_RAIB)
		{
			m_RAIB.Pair();
		}
	}
	
	override RemotelyActivatedItemBehaviour GetRemotelyActivatedItemBehaviour()
	{
		return m_RAIB;
	}
	
	override void PairRemote(notnull EntityAI trigger)
	{
		m_RAIB.Pair(trigger);
	}
	
	override void UnpairRemote()
	{
		if (GetPairDevice())
		{
			GetPairDevice().UnpairRemote();
		}

		m_RAIB.Unpair();
	}

	override EntityAI GetPairDevice()
	{
		return m_RAIB.GetPairDevice();
	}
	
	override bool CanBeArmed()
	{
		if (GetArmed())
		{
			return false;
		}
		
		if (!HasLockedTriggerSlots())
		{
			return false;
		}

		for (int i = 0; i < SLOT_EXPLOSIVE_COUNT; i++)
		{
			EntityAI explosive = GetInventory().FindAttachmentByName(SLOT_EXPLOSIVES[i]);
			if (explosive)
			{
				return true;
			}
		}
		
		return false;
	}

	override bool CanBeDisarmed()
	{
		return true;
	}
	
	override bool CanPutIntoHands(EntityAI parent)
	{
		return IsTakeable();
	}

	override bool CanRemoveFromHands(EntityAI parent)
	{
		return IsTakeable();
	}
	
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		InventoryLocation il = new InventoryLocation();
		GetInventory().GetCurrentInventoryLocation(il);
		for (int i = 0; i < SLOT_TRIGGERS_COUNT; i++)
		{
			if (slotId == InventorySlots.GetSlotIdFromString(SLOT_TRIGGERS[i]))
			{
				if (il.GetType() == InventoryLocationType.HANDS)
				{
					return false;
				}
			}
		}

		return !GetArmed();
	}
	
	override bool CanDisplayAttachmentSlot(int slot_id)
	{
		string slotName = InventorySlots.GetSlotName(slot_id);

		switch (slotName)
		{
		case SLOT_TRIGGER_ALARM_CLOCK:
		case SLOT_TRIGGER_KITCHEN_TIMER:
		case SLOT_TRIGGER_REMOTE:
			return FindAttachmentBySlotName(slotName) != null;
		break;
		}

		return true;
	}
	
	override bool IsTimerDetonable()
	{
		return true;
	}
	
	override bool IsTakeable()
	{
		return !GetArmed() && super.IsTakeable();
	}
	
	override bool IsDeployable()
	{
		return !GetArmed();
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);

		if (GetGame().IsServer())
		{
			if (newLevel == GameConstants.STATE_RUINED)
			{
				for (int attachmentIdx = 0; attachmentIdx < GetInventory().AttachmentCount(); attachmentIdx++)
				{
					ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(attachmentIdx));
					if (attachment)
					{
						attachment.UnlockFromParent();
						attachment.SetHealth("", "", 0.0);
					}
				}
				
				SetArmed(false);
				SetTakeable(true);
			}
		}
	}
	
	override void OnActivatedByItem(notnull ItemBase item)
	{
		if (GetGame().IsServer() && GetArmed())
		{
			ItemBase attachment;
			bool isTimeTriggered = false;
			array<ItemBase> attachmentsCache = new array<ItemBase>();
			
			//! cache attachments
			int attachmentIdx;
			for (attachmentIdx = 0; attachmentIdx < GetInventory().AttachmentCount(); attachmentIdx++)
			{
				attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(attachmentIdx));
				if (attachment)
				{
					attachmentsCache.Insert(attachment);
				}
			}

			//! unlock attachments first
			for (attachmentIdx = 0; attachmentIdx < attachmentsCache.Count(); attachmentIdx++)
			{
				attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(attachmentIdx));
				if (attachmentsCache[attachmentIdx])
				{
					attachmentsCache[attachmentIdx].UnlockFromParent();
				}	
			}
			
			//! triggers specific handling
			for (attachmentIdx = 0; attachmentIdx < attachmentsCache.Count(); attachmentIdx++)
			{
				attachment = ItemBase.Cast(attachmentsCache[attachmentIdx]);
				if (attachment.IsInherited(ClockBase))
				{
					isTimeTriggered = true;
				}
			}
			
			//! go through attached explosives
			for (attachmentIdx = 0; attachmentIdx < attachmentsCache.Count(); attachmentIdx++)
			{
				attachment = ItemBase.Cast(attachmentsCache[attachmentIdx]);
				if (attachment && !attachment.IsAnyInherited({RemoteDetonator, ClockBase}))
				{
					GetInventory().DropEntityInBounds(InventoryMode.SERVER, this, attachment, "0.5 0 0.5", 0, 0, 0);
					attachment.SetAnimationPhase(ANIM_PHASE_VISIBILITY, 1.0);
					attachment.SetTakeable(false);
				}
			}
			
			float delayFor = TIME_TRIGGER_INITIAL_DELAY_SECS;
			if (isTimeTriggered)
			{
				delayFor = TIME_TRIGGER_INITIAL_DELAY_SECS + TIME_TRIGGER_TIMER_BASED_DELAY_SECS;
				//! defer delete to allow ringing
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, delayFor * 1000, false);
			}
			else
			{
				DeleteSafe();
			}

			//! final traverse - attached explosives activation
			for (attachmentIdx = 0; attachmentIdx < attachmentsCache.Count(); attachmentIdx++)
			{
				attachment = ItemBase.Cast(attachmentsCache[attachmentIdx]);
				if (attachment.IsAnyInherited({RemoteDetonator,ClockBase}))
				{
					//! defer damage to trigger attachments to allow ringing
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(attachment.SetHealth, delayFor * 1000, false, "", "", 0.0);
				}

				if (attachment && !attachment.IsAnyInherited({RemoteDetonator, ClockBase}))
				{
					Param1<ItemBase> params = new Param1<ItemBase>(attachment);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(attachment, "OnActivatedByItem", delayFor * 1000, false, params);
					delayFor += TIME_TRIGGER_DELAY_SECS;					
				}
			}
		}
	}
	
	//! not exploding itself, rely on attached explosives	
	override protected void InitiateExplosion();
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		switch (slot_name)
		{
		case SLOT_TRIGGER_ALARM_CLOCK:
		case SLOT_TRIGGER_KITCHEN_TIMER:
		case SLOT_TRIGGER_REMOTE:
			OnTriggerAttached(FindAttachmentBySlotName(slot_name));
		break;
		}
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		switch (slot_name)
		{
		case SLOT_TRIGGER_ALARM_CLOCK:
		case SLOT_TRIGGER_KITCHEN_TIMER:
		case SLOT_TRIGGER_REMOTE:
			OnTriggerDetached(FindAttachmentBySlotName(slot_name));
		break;
		}
	}
	
	override void OnDisarmed(bool pWithTool)
	{
		super.OnDisarmed(pWithTool);
		
		UnpairRemote();

		for (int att = 0; att < GetInventory().AttachmentCount(); att++)
		{
			ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(att));
			if (attachment)
			{
				attachment.UnlockFromParent();

				if (attachment.IsInherited(ClockBase))
				{
					if (pWithTool)
					{
						GetInventory().DropEntity(InventoryMode.SERVER, this, attachment);
					}
				}
				
				if (attachment.IsInherited(RemoteDetonator))
				{
					if (pWithTool)
					{
						GetInventory().DropEntity(InventoryMode.SERVER, this, attachment);
						attachment.SetHealth("", "", 0.0);
					}
					else
					{
						attachment.Delete();
					}
				}
			}
		}
		
		LockTriggerSlots();
		SetTakeable(true);
	}
	
	override void UpdateLED(int pState)
	{
		RemoteDetonatorReceiver receiver = RemoteDetonatorReceiver.Cast(FindAttachmentBySlotName(SLOT_TRIGGER_REMOTE));
		if (receiver)
		{
			receiver.UpdateLED(pState, true);
		}
	}
	
	protected void OnTriggerAttached(EntityAI entity)
	{
		UpdateVisuals(entity);
		UpdateLED(ERemoteDetonatorLEDState.LIT);
		
		if (entity.IsInherited(ClockBase))
		{
			Arm();
		}
		
		for (int att = 0; att < GetInventory().AttachmentCount(); att++)
		{
			ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(att));
			if (attachment)
			{
				attachment.LockToParent();
			}
		}
	}
	
	protected void OnTriggerDetached(EntityAI entity)
	{
		UpdateVisuals(null);
		UpdateLED(ERemoteDetonatorLEDState.OFF);
	}
	
	protected void UpdateVisuals(EntityAI entity)
	{
		if (entity)
		{
			if (entity.IsInherited(RemoteDetonator))
			{
				SetAnimationPhase(ANIM_PHASE_TRIGGER_EMPTY, 1.0);
				SetAnimationPhase(ANIM_PHASE_TRIGGER_TIMER, 1.0);
				SetAnimationPhase(ANIM_PHASE_TRIGGER_CLOCK, 1.0);
				SetAnimationPhase(ANIM_PHASE_TRIGGER_REMOTE, 0.0);
			}
			else if (entity.IsInherited(AlarmClock_ColorBase))
			{
				SetAnimationPhase(ANIM_PHASE_TRIGGER_EMPTY, 1.0);
				SetAnimationPhase(ANIM_PHASE_TRIGGER_TIMER, 1.0);
				SetAnimationPhase(ANIM_PHASE_TRIGGER_CLOCK, 0.0);
				SetAnimationPhase(ANIM_PHASE_TRIGGER_REMOTE, 1.0);
			}
			else if (entity.IsInherited(KitchenTimer))
			{
				SetAnimationPhase(ANIM_PHASE_TRIGGER_EMPTY, 1.0);
				SetAnimationPhase(ANIM_PHASE_TRIGGER_TIMER, 0.0);
				SetAnimationPhase(ANIM_PHASE_TRIGGER_CLOCK, 1.0);
				SetAnimationPhase(ANIM_PHASE_TRIGGER_REMOTE, 1.0);
			}
		}
		else
		{
			SetAnimationPhase(ANIM_PHASE_TRIGGER_EMPTY, 0.0);
			SetAnimationPhase(ANIM_PHASE_TRIGGER_TIMER, 1.0);
			SetAnimationPhase(ANIM_PHASE_TRIGGER_CLOCK, 1.0);
			SetAnimationPhase(ANIM_PHASE_TRIGGER_REMOTE, 1.0);
		}
	}
	
#ifdef DEVELOPER
	override protected string GetDebugText()
	{
		string debug_output;
		debug_output += string.Format("low net id: %1\n", m_RAIB.GetPairDeviceNetIdLow());
		debug_output += string.Format("high net id: %1\n", m_RAIB.GetPairDeviceNetIdHigh());
		debug_output += string.Format("pair device: %1\n", m_RAIB.GetPairDevice());

		return debug_output;
	}
#endif
}

class ImprovisedExplosivePlacing : ImprovisedExplosive {}