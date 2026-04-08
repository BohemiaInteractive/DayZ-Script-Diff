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
		if (!g_Game.IsMultiplayer())
			LockTriggerSlots();
	}

	override bool HasLockedTriggerSlots()
	{
		foreach (string triggerSlot : SLOT_TRIGGERS)
			return GetInventory().GetSlotLock(InventorySlots.GetSlotIdFromString(triggerSlot));
		
		return false;
	}
	
	override void LockTriggerSlots()
	{
		foreach (string triggerSlotName : SLOT_TRIGGERS)
			GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(triggerSlotName), true);
	}
	
	override void UnlockTriggerSlots()
	{
		foreach (string triggerSlotName : SLOT_TRIGGERS)
			GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(triggerSlotName), false);
	}
	
	override void LockExplosivesSlots()
	{	
		foreach (string explosiveSlotName : SLOT_EXPLOSIVES)
			GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(explosiveSlotName), true);
	}
	
	override void UnlockExplosivesSlots()
	{	
		foreach (string explosiveSlotName : SLOT_EXPLOSIVES)
			GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(explosiveSlotName), false);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (version <= 134) // up to 1.21
		{
			foreach (string triggerSlotName : SLOT_TRIGGERS)
			{
				int slotId = InventorySlots.GetSlotIdFromString(triggerSlotName);
				bool locked = GetInventory().GetSlotLock(slotId);
				while (locked)
				{
					GetInventory().SetSlotLock(slotId, false);
					locked = GetInventory().GetSlotLock(slotId);
				}
			}
		}
		
		return true;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		LockTriggerSlots();
	}
	
 	override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
		
		if (m_RAIB)
		{
			m_RAIB.OnVariableSynchronized();
		}

		
		foreach (string slotName : SLOT_TRIGGERS)
		{
			EntityAI trigger = GetInventory().FindAttachmentByName(slotName);
			if (trigger)
			{
				UpdateVisuals(trigger);
				break;
			}
		}
	}
	
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		
		if (g_Game.IsServer())
		{
			SetOrientation(vector.Up);
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
	
	override EntityAI GetPairDevice()
	{
		return m_RAIB.GetPairDevice();
	}
	
	override bool CanBeArmed()
	{
		if (GetArmed())
			return false;
		
		if (!HasLockedTriggerSlots())
			return false;

		foreach (string slotName : SLOT_EXPLOSIVES)
		{
			EntityAI explosive = GetInventory().FindAttachmentByName(slotName);
			if (explosive)
				return true;
		}
		
		return false;
	}

	override bool CanBeDisarmed()
	{
		return true;
	}
	
	
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		InventoryLocation il = new InventoryLocation();
		GetInventory().GetCurrentInventoryLocation(il);
		
		foreach (string slotName : SLOT_TRIGGERS)
		{
			if (slotId == InventorySlots.GetSlotIdFromString(slotName))
			{
				if (il.GetType() == InventoryLocationType.HANDS)
					return false;
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

		if (g_Game.IsServer())
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
		if (g_Game.IsServer() && GetArmed())
		{
			bool isTimeTriggered = false;

			array<ItemBase> attachmentsCache = new array<ItemBase>();
			for (int attachmentIdx = 0; attachmentIdx < GetInventory().AttachmentCount(); attachmentIdx++)
			{
				ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(attachmentIdx));
				if (attachment)
					attachmentsCache.Insert(attachment);
			}
			
			foreach (ItemBase attachment1 : attachmentsCache)
				attachment1.UnlockFromParent();
			
			//! attachment special handling on disarm
			foreach (ItemBase attachment2 : attachmentsCache)
			{
				if (attachment2.IsInherited(ClockBase))
				{
					isTimeTriggered = true;
					break;
				}
			}
			
			//! go through attached explosives
			foreach (ItemBase attachment3 : attachmentsCache)
			{
				if (attachment3)
				{
					vector dropExtents = "0.5 0.0 0.5";
					if (isTimeTriggered)
						dropExtents[1] = 0.15; //! hard-case blocks shells (result of deferred delete)

					GetInventory().DropEntityInBounds(InventoryMode.SERVER, this, attachment3, dropExtents, 0, 0, 0);
					attachment3.SetAnimationPhase(ANIM_PHASE_VISIBILITY, 1.0);
					attachment3.SetTakeable(false);
				}
			}
			
			float delayFor = TIME_TRIGGER_INITIAL_DELAY_SECS;
			if (isTimeTriggered)
			{
				delayFor = TIME_TRIGGER_INITIAL_DELAY_SECS + TIME_TRIGGER_TIMER_BASED_DELAY_SECS;
				//! defer delete to allow ringing
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, delayFor * 1000, false);
			}
			else
			{
				DeleteSafe();
			}

			//! final traverse - attached explosives activation
			foreach (ItemBase attachment4 : attachmentsCache)
			{
				if (attachment4.IsAnyInherited({RemoteDetonator, ClockBase}))
				{
					//! defer damage to trigger attachments to allow ringing
					g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(attachment4.DeleteSafe, delayFor * 1000, false);
				}

				if (attachment4 && !attachment4.IsAnyInherited({RemoteDetonator, ClockBase}))
				{
					Param1<ItemBase> params = new Param1<ItemBase>(attachment4);
					g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(attachment4, "OnActivatedByItem", delayFor * 1000, false, params);
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
	
	override void OnBeforeDisarm()
	{
		UnlockExplosivesSlots();
	}
	
	override void OnDisarmed(bool pWithTool)
	{
		super.OnDisarmed(pWithTool);
		
		UnpairRemote();
		
		array<ItemBase> attachmentsCache = new array<ItemBase>();
		for (int attachmentIdx = 0; attachmentIdx < GetInventory().AttachmentCount(); attachmentIdx++)
		{
			ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(attachmentIdx));
			if (attachment)
			{
				attachmentsCache.Insert(attachment);
			}	
		}
		
		foreach (ItemBase attachment1 : attachmentsCache)
			attachment1.UnlockFromParent();
		
		//! attachment special handling on disarm
		foreach (ItemBase attachment2 : attachmentsCache)
		{
			if (attachment2.IsInherited(ClockBase))
			{
				if (pWithTool)
					GetInventory().DropEntity(InventoryMode.SERVER, this, attachment2);
			}
			
			if (attachment2.IsInherited(RemoteDetonator))
			{
				if (pWithTool)
				{
					GetInventory().DropEntity(InventoryMode.SERVER, this, attachment2);
					attachment2.SetHealth("", "", 0.0);
				}
				else
				{
					attachment2.Delete();
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
			receiver.UpdateLED(pState, true);
	}
	
	protected void OnTriggerAttached(EntityAI entity)
	{
		UpdateVisuals(entity);
		UpdateLED(ERemoteDetonatorLEDState.LIT);
		
		if (entity.IsInherited(ClockBase))
			Arm();
		
		LockTriggerSlots();
		LockExplosivesSlots();
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
	
	override string GetDeploySoundset()
	{
		return "placeImprovisedExplosive_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "improvisedexplosive_deploy_SoundSet";
	}
	
	override string GetArmSoundset()
	{
		return "ImprovisedExplosive_disarm_SoundSet";
	}
	
	override string GetDisarmSoundset()
	{
		return "ImprovisedExplosive_disarm_SoundSet";
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