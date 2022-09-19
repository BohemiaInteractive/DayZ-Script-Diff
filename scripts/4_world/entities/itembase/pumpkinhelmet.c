// Have a spooky Halloween everyone!

class PumpkinHelmet : Clothing
{
	void PumpkinHelmet()
	{
		SetEventMask(EntityEvent.INIT); // Enable EOnInit event
	}
	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing eyewear = Clothing.Cast(parent.FindAttachmentBySlotName("Eyewear"));
		if ( eyewear && eyewear.ConfigGetBool("isStrap") )
		{
			return false;
		}
		
		Clothing mask = Clothing.Cast(parent.FindAttachmentBySlotName("Mask"));
		if ( mask && (mask.ConfigGetBool("noHelmet") && !HockeyMask.Cast(mask) && !SantasBeard.Cast(mask)) ) //TODO
		{
			return false;
		}
		
		return true;
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
		if ( !GetGame().IsDedicatedServer() ) //SP only
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
};

// boo!