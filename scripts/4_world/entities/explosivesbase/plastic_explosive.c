class Plastic_Explosive : ExplosivesBase
{
	protected const string SLOT_TRIGGER 					= "TriggerRemoteDetonator_Receiver";
	protected const string ANIM_PHASE_TRIGGER_REMOTE 		= "TriggerRemote";
	
	protected bool m_UsedAsCharge;
	
	protected ref RemotelyActivatedItemBehaviour m_RAIB;

	void Plastic_Explosive()
	{
		m_RAIB = new RemotelyActivatedItemBehaviour(this);
		
		SetAmmoType("Plastic_Explosive_Ammo");
		SetParticleExplosion(ParticleList.PLASTIC_EXPLOSION);

		RegisterNetSyncVariableInt("m_RAIB.m_PairDeviceNetIdLow");
		RegisterNetSyncVariableInt("m_RAIB.m_PairDeviceNetIdHigh");
	}
	
	override void EOnInit(IEntity other, int extra)
	{
		if (!g_Game.IsMultiplayer())
			LockTriggerSlots();
	}
	
	//! special behaviour - do not call super from ExplosivesBase
	override void EEKilled(Object killer)	
	{
		//analytics (behaviour from EntityAI)
		GetGame().GetAnalyticsServer().OnEntityKilled(killer, this);
	}

	override bool HasLockedTriggerSlots()
	{
		return GetInventory().GetSlotLock(InventorySlots.GetSlotIdFromString(SLOT_TRIGGER));
	}
	
	override void LockTriggerSlots()
	{
		GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(SLOT_TRIGGER), true);
	}
	
	override void UnlockTriggerSlots()
	{
		GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(SLOT_TRIGGER), false);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (version <= 134) // up to 1.21
		{
			int slotId = InventorySlots.GetSlotIdFromString(SLOT_TRIGGER);
			bool locked = GetInventory().GetSlotLock(slotId);
			while (locked)
			{
				GetInventory().SetSlotLock(slotId, false);
				locked = GetInventory().GetSlotLock(slotId);
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
		
		int slotId = InventorySlots.GetSlotIdFromString(SLOT_TRIGGER);
		UpdateVisuals(GetInventory().FindAttachment(slotId));
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);

		if (m_RAIB)
		{
			m_RAIB.Pair();
		}
	}
	
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		InventoryLocation il = new InventoryLocation();
		GetInventory().GetCurrentInventoryLocation(il);
		if (il.GetType() != InventoryLocationType.GROUND)
		{
			return false;
		}
	
		ClockBase timer = ClockBase.Cast(attachment);
		if (timer && !timer.IsAlarmOn())
		{
			return false;
		}

		return !GetArmed();
	}
	
	override bool CanDisplayAttachmentSlot(int slot_id)
	{
		string slotName = InventorySlots.GetSlotName(slot_id);

		switch (slotName)
		{
		case SLOT_TRIGGER:
			return FindAttachmentBySlotName(slotName) != null;
		break;
		}

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
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		m_UsedAsCharge = false;

		if (parent && parent.IsInherited(ExplosivesBase))
		{
			m_UsedAsCharge = true;
		}
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);

		if (parent && !parent.IsInherited(ExplosivesBase))
		{
			m_UsedAsCharge = false;
		}
	}
	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		//! ignores explosion damage from other Plastic_Explosive that are used as charge(like 2 PE fired via IED)
		if (damageType == DamageType.EXPLOSION)
		{
			return !m_UsedAsCharge;
		}
		
		return true;
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
		if (!super.CanBeArmed())
		{
			return false;
		}

		return HasLockedTriggerSlots() && !GetArmed();
	}
	
	override bool CanBeDisarmed()
	{
		return GetArmed();
	}
	
	override void OnActivatedByItem(notnull ItemBase item)
	{
		if (GetGame().IsServer())
		{
			if (GetHealthLevel("") == GameConstants.STATE_RUINED)
			{
				return;
			}
			
			if (item == this)
			{
				SetHealth("", "", 0.0);
				InitiateExplosion();
				return;
			}
			
			if (m_RAIB.IsPaired() && GetArmed())
			{
				if (GetPairDevice() == item)
				{
					SetHealth("", "", 0.0);
					InitiateExplosion();
				}
			}
		}
	}
	
	override void OnDisarmed(bool pWithTool)
	{
		super.OnDisarmed(pWithTool);
		
		UnpairRemote();
		UpdateVisuals(null);
		
		for (int att = 0; att < GetInventory().AttachmentCount(); att++)
		{
			ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(att));
			if (attachment)
			{
				attachment.UnlockFromParent();
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
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		if (slot_name == SLOT_TRIGGER)
			OnTriggerAttached(item);
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		if (slot_name == SLOT_TRIGGER)
			OnTriggerDetached(item);
	}
	
	override void UpdateLED(int pState)
	{
		RemoteDetonatorReceiver receiver = RemoteDetonatorReceiver.Cast(FindAttachmentBySlotName(SLOT_TRIGGER));
		if (receiver)
		{
			receiver.UpdateLED(pState, true);
		}
	}

	protected void OnTriggerAttached(EntityAI entity)
	{
		Arm();
		UpdateVisuals(entity);
		UpdateLED(ERemoteDetonatorLEDState.LIT);
		
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
			SetAnimationPhase(ANIM_PHASE_TRIGGER_REMOTE, 0.0);
		}
		else
		{
			SetAnimationPhase(ANIM_PHASE_TRIGGER_REMOTE, 1.0);
		}
	}
		
	override string GetDeploySoundset()
	{
		return "placePlasticExplosive_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "plasticexplosive_deploy_SoundSet";
	}
	
	override string GetArmSoundset()
	{
		return "plasticexplosive_deploy_SoundSet";
	}
	
	override string GetDisarmSoundset()
	{
		return "ImprovisedExplosive_disarm_SoundSet";
	}
	
}

class Plastic_Explosive_Placing : Plastic_Explosive {}
