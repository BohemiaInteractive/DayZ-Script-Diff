// Have a spooky Halloween everyone!

class PumpkinHelmet : HelmetBase
{
	void PumpkinHelmet()
	{
		SetEventMask(EntityEvent.INIT); // Enable EOnInit event
	}
	
	override void OnMovedInsideCargo(EntityAI container)
	{
		UpdateGlowState();
	}
	
	override void OnMovedWithinCargo(EntityAI container)
	{
		UpdateGlowState();
	}
	
	override void OnRemovedFromCargo(EntityAI container)
	{
		UpdateGlowState();
	}
	
	override void EOnInit( IEntity other, int extra)
	{
		UpdateGlowState();
	}
	
	override void OnItemLocationChanged( EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged( old_owner, new_owner);
		
		UpdateGlowState();
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		UpdateGlowState();
	}
	
	void UpdateGlowState()
	{
		// Makes sure PumpkinHelmet doesn't glow when it's attached on head, or it's inside cargo.
		
		bool do_glow = true;

		if ( IsDamageDestroyed() )
		{
			do_glow = false;
		}
		else
		{
			int id = InventorySlots.HEADGEAR;
			InventoryLocation IL = new InventoryLocation();
			GetInventory().GetCurrentInventoryLocation( IL );
			
			int id_2 = IL.GetSlot();
			int id_cargo = IL.GetIdx();
			
			if ( id == id_2) // Pumpkin is attached on head
				do_glow = false;
			
			if ( id_cargo != -1 ) // Pumpkin is in cargo
				do_glow = false;
		}

		SetPilotLight(do_glow);
	}
	
	override void UpdateNVGStatus(PlayerBase player, bool attaching = false, bool force_disable = false)
	{
		if ( !g_Game.IsDedicatedServer() ) //SP only
		{
			if (force_disable)
			{
				player.RemoveActiveNV(NVTypes.NV_PUMPKIN);
			}
			else
			{
				if ( attaching && (!player.IsNVGWorking() || player.GetNVType() != NVTypes.NV_PUMPKIN) )
				{
					player.AddActiveNV(NVTypes.NV_PUMPKIN);
				}
				else if ( !attaching && player.IsNVGWorking() )
				{
					player.RemoveActiveNV(NVTypes.NV_PUMPKIN);
				}
			}
		}
	}
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		if (slotId == InventorySlots.HEADGEAR)
		{
			ret.Insert(EAttExclusions.EXCLUSION_HEADGEAR_HELMET_0);
			
			ret.Insert(EAttExclusions.EXCLUSION_MASK_1);
			ret.Insert(EAttExclusions.EXCLUSION_MASK_2);
			ret.Insert(EAttExclusions.EXCLUSION_HEADSTRAP_0);
			
			ret.Insert(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
		}
		return ret;
	}
};

// boo!