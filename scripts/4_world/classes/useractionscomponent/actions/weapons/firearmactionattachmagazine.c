class AttachMagazineActionReciveData : ActionReciveData
{
	ref InventoryLocation  m_ilOldMagazine;
}
class AttachMagazineActionData : ActionData
{
	ref InventoryLocation  m_ilOldMagazine;
	Magazine m_oldMagazine;
}

class FirearmActionAttachMagazine : FirearmActionBase
{	
	//-----------------------------------------------------
	// 	Action events and methods
	//-----------------------------------------------------
	void FirearmActionAttachMagazine() 
	{
		m_Text = "#attach";
	}	
	
	override int GetActionCategory()
	{
		return AC_SINGLE_USE;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item ) //condition for action
	{
		if (!super.ActionCondition( player, target, item ))
			return false;
		
		HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();
		Magazine mag = Magazine.Cast(target.GetObject());	
		Weapon_Base wpn = Weapon_Base.Cast(item);	
		return mag && (player.GetWeaponManager().CanAttachMagazine(wpn,mag) || player.GetWeaponManager().CanSwapMagazine(wpn,mag)) && (!hcw || hcw.GetRunningAction() != WeaponActions.RELOAD);
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		return true;
	}
	
	override ActionData CreateActionData()
	{
		AttachMagazineActionData action_data = new AttachMagazineActionData;
		return action_data;
	}
	
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		AttachMagazineActionData action_data_am = AttachMagazineActionData.Cast(action_data);
		
		action_data_am.m_ilOldMagazine.WriteToContext(ctx);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if (!action_recive_data)
		{
			action_recive_data = new AttachMagazineActionReciveData;
		}
		
		super.ReadFromContext(ctx, action_recive_data);
		
		InventoryLocation il = new InventoryLocation;		
		if (!il.ReadFromContext(ctx))
			return false;
		
		AttachMagazineActionReciveData recive_data_am = AttachMagazineActionReciveData.Cast(action_recive_data);
		recive_data_am.m_ilOldMagazine = il;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		AttachMagazineActionReciveData recive_data_am = AttachMagazineActionReciveData.Cast(action_recive_data);
		AttachMagazineActionData action_data_am = AttachMagazineActionData.Cast(action_data);
		
		action_data.m_MainItem = action_recive_data.m_MainItem;
		if (!action_recive_data.m_Target)
		{
			action_data.m_Target = new ActionTarget(NULL, NULL, -1, vector.Zero, 0); 
		}
		else
		{
			action_data.m_Target = action_recive_data.m_Target;
		}
		action_data_am.m_ilOldMagazine = recive_data_am.m_ilOldMagazine;
	}
	
	override bool Post_SetupAction( ActionData action_data )
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
			int muzzle_index = wpn.GetCurrentMuzzle();
				
			AttachMagazineActionData am_action_data = AttachMagazineActionData.Cast(action_data);
			am_action_data.m_oldMagazine = wpn.GetMagazine(muzzle_index);
				
			InventoryLocation new_il = new InventoryLocation;

			if (!action_data.m_Player.GetWeaponManager().PrepareInventoryLocationForMagazineSwap(wpn, Magazine.Cast(action_data.m_Target.GetObject()), new_il))
			{
				return false;
			}
			am_action_data.m_ilOldMagazine = new_il;
		}
		return true;
	}
	
	override bool InventoryReservation( ActionData action_data)
	{
		if( (IsLocal() || !UseAcknowledgment()) && IsInstant() )
			return true;
		
		bool success = true;
		InventoryLocation oldMagTargetInventoryLocation = NULL;
		InventoryLocation targetInventoryLocation = NULL;
		InventoryLocation handInventoryLocation = NULL;
		
		PlayerBase player = action_data.m_Player;
		AttachMagazineActionData am_action_data = AttachMagazineActionData.Cast(action_data);
		
		Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
		int muzzle_index = wpn.GetCurrentMuzzle();
		
		Magazine new_mag = Magazine.Cast(action_data.m_Target.GetObject());
		
		if (am_action_data.m_oldMagazine)
		{
			oldMagTargetInventoryLocation = new InventoryLocation;
			if ( action_data.m_Player.GetInventory().HasInventoryReservation( am_action_data.m_oldMagazine, am_action_data.m_ilOldMagazine) )
			{
				success = false;
			}
			else
			{
				player.GetInventory().AddInventoryReservationEx(am_action_data.m_oldMagazine,am_action_data.m_ilOldMagazine,GameInventory.c_InventoryReservationTimeoutMS);
			}
		}
		
		if (success)
		{
			targetInventoryLocation = new InventoryLocation;
			targetInventoryLocation.SetAttachment( wpn, new_mag, InventorySlots.MAGAZINE);
			if ( action_data.m_Player.GetInventory().HasInventoryReservation( new_mag, targetInventoryLocation) )
			{
				success = false;
				if (am_action_data.m_oldMagazine)
				{
					player.GetInventory().ClearInventoryReservation(am_action_data.m_oldMagazine,am_action_data.m_ilOldMagazine);
				}
			}
			else
			{
				action_data.m_Player.GetInventory().AddInventoryReservationEx( new_mag, targetInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS);
			}
		}
	
		if (success)
		{
			handInventoryLocation = new InventoryLocation;
			handInventoryLocation.SetHands(action_data.m_Player, wpn);
		
			if ( action_data.m_Player.GetInventory().HasInventoryReservation( wpn, handInventoryLocation) )
			{
				if (am_action_data.m_oldMagazine)
				{
					player.GetInventory().ClearInventoryReservation(am_action_data.m_oldMagazine,am_action_data.m_ilOldMagazine);
				}
				player.GetInventory().ClearInventoryReservation(new_mag, targetInventoryLocation);
				success = false;
			}
			else
			{
				action_data.m_Player.GetInventory().AddInventoryReservationEx( wpn, handInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS);
			}				
		}
		
		if ( success )
		{
			if (am_action_data.m_ilOldMagazine)
				action_data.m_ReservedInventoryLocations.Insert(am_action_data.m_ilOldMagazine);
			
			if (targetInventoryLocation)
				action_data.m_ReservedInventoryLocations.Insert(targetInventoryLocation);
			
			if (handInventoryLocation)
				action_data.m_ReservedInventoryLocations.Insert(handInventoryLocation);
		}
		
		return success;
	}

	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		AttachMagazineActionData action_data_am = AttachMagazineActionData.Cast(action_data);
		Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
		Magazine mag = Magazine.Cast(action_data.m_Target.GetObject());	
		ClearInventoryReservationEx(action_data);
		if ( action_data.m_Player.GetWeaponManager().CanAttachMagazine(wpn,mag,false) )
			action_data.m_Player.GetWeaponManager().AttachMagazine(mag, this);
		else
			action_data.m_Player.GetWeaponManager().SwapMagazineEx(mag, action_data_am.m_ilOldMagazine, this);
		 InventoryReservation(action_data);
	}
	
	override bool CanBePerformedFromInventory()
	{
		return true;
	}
	
	override bool CanBePerformedFromQuickbar()
	{
		return true;
	}
};


class FirearmActionAttachMagazineQuick : FirearmActionBase
{	
	void FirearmActionAttachMagazineQuick() 
	{
	}	
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item ) //condition for action
	{
		if (!super.ActionCondition( player, target, item ))
			return false;
		
		HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();
		if (hcw && hcw.GetRunningAction() == WeaponActions.RELOAD)
			return false;
		
		MagazineStorage mag = MagazineStorage.Cast(player.GetWeaponManager().GetPreparedMagazine());
		
		if (!mag)
			return false;
		
		Weapon weapon = Weapon.Cast(item);
		bool isLoadedMag = false;
		
		for (int i = 0, count = weapon.GetMuzzleCount(); i < count; ++i)
			isLoadedMag |= ( mag == weapon.GetMagazine( i ) );
		
		return !isLoadedMag;
	}
	
	override ActionData CreateActionData()
	{
		AttachMagazineActionData action_data = new AttachMagazineActionData;
		return action_data;
	}
	
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		AttachMagazineActionData action_data_am = AttachMagazineActionData.Cast(action_data);
		
		action_data_am.m_ilOldMagazine.WriteToContext(ctx);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if (!action_recive_data)
		{
			action_recive_data = new AttachMagazineActionReciveData;
		}
		
		super.ReadFromContext(ctx, action_recive_data);
		
		InventoryLocation il = new InventoryLocation;		
		if (!il.ReadFromContext(ctx))
			return false;
		
		AttachMagazineActionReciveData recive_data_am = AttachMagazineActionReciveData.Cast(action_recive_data);
		recive_data_am.m_ilOldMagazine = il;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		AttachMagazineActionReciveData recive_data_am = AttachMagazineActionReciveData.Cast(action_recive_data);
		AttachMagazineActionData action_data_am = AttachMagazineActionData.Cast(action_data);
		
		action_data.m_MainItem = action_recive_data.m_MainItem;
		if (!action_recive_data.m_Target)
		{
			action_data.m_Target = new ActionTarget(NULL, NULL, -1, vector.Zero, 0); 
		}
		else
		{
			action_data.m_Target = action_recive_data.m_Target;
		}
		action_data_am.m_ilOldMagazine = recive_data_am.m_ilOldMagazine;
	}
	
	override bool Post_SetupAction( ActionData action_data )
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
			int muzzle_index = wpn.GetCurrentMuzzle();
				
			AttachMagazineActionData am_action_data = AttachMagazineActionData.Cast(action_data);
			am_action_data.m_oldMagazine = wpn.GetMagazine(muzzle_index);
				
			ActionTarget newTarget = new ActionTarget(action_data.m_Player.GetWeaponManager().GetPreparedMagazine(), null, -1, vector.Zero, -1);
			action_data.m_Target = newTarget;
				
			InventoryLocation new_il = new InventoryLocation;

			if (!action_data.m_Player.GetWeaponManager().PrepareInventoryLocationForMagazineSwap(wpn, Magazine.Cast(action_data.m_Target.GetObject()), new_il))
			{
				return false;
			}
			am_action_data.m_ilOldMagazine = new_il;
		}
		return true;
	}
	
	override bool InventoryReservation( ActionData action_data)
	{
		if( (IsLocal() || !UseAcknowledgment()) && IsInstant() )
			return true;
		
		bool success = true;
		InventoryLocation oldMagTargetInventoryLocation = NULL;
		InventoryLocation targetInventoryLocation = NULL;
		InventoryLocation handInventoryLocation = NULL;
		
		PlayerBase player = action_data.m_Player;
		AttachMagazineActionData am_action_data = AttachMagazineActionData.Cast(action_data);
		
		Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
		int muzzle_index = wpn.GetCurrentMuzzle();
		
		Magazine new_mag = Magazine.Cast(action_data.m_Target.GetObject());
		
		if (am_action_data.m_oldMagazine)
		{
			oldMagTargetInventoryLocation = new InventoryLocation;
			if ( action_data.m_Player.GetInventory().HasInventoryReservation( am_action_data.m_oldMagazine, am_action_data.m_ilOldMagazine) )
			{
				success = false;
			}
			else
			{
				player.GetInventory().AddInventoryReservationEx(am_action_data.m_oldMagazine,am_action_data.m_ilOldMagazine,GameInventory.c_InventoryReservationTimeoutMS);
			}
		}
		
		if (success)
		{
			targetInventoryLocation = new InventoryLocation;
			targetInventoryLocation.SetAttachment( wpn, new_mag, InventorySlots.MAGAZINE);
			if ( action_data.m_Player.GetInventory().HasInventoryReservation( new_mag, targetInventoryLocation) )
			{
				success = false;
				if (am_action_data.m_oldMagazine)
				{
					player.GetInventory().ClearInventoryReservation(am_action_data.m_oldMagazine,am_action_data.m_ilOldMagazine);
				}
			}
			else
			{
				action_data.m_Player.GetInventory().AddInventoryReservationEx( new_mag, targetInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS);
			}
		}
	
		if (success)
		{
			handInventoryLocation = new InventoryLocation;
			handInventoryLocation.SetHands(action_data.m_Player, wpn);
		
			if ( action_data.m_Player.GetInventory().HasInventoryReservation( wpn, handInventoryLocation) )
			{
				if (am_action_data.m_oldMagazine)
				{
					player.GetInventory().ClearInventoryReservation(am_action_data.m_oldMagazine,am_action_data.m_ilOldMagazine);
				}
				player.GetInventory().ClearInventoryReservation(new_mag, targetInventoryLocation);
				success = false;
			}
			else
			{
				action_data.m_Player.GetInventory().AddInventoryReservationEx( wpn, handInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS);
			}				
		}
		
		if ( success )
		{
			if (am_action_data.m_ilOldMagazine)
				action_data.m_ReservedInventoryLocations.Insert(am_action_data.m_ilOldMagazine);
			
			if (targetInventoryLocation)
				action_data.m_ReservedInventoryLocations.Insert(targetInventoryLocation);
			
			if (handInventoryLocation)
				action_data.m_ReservedInventoryLocations.Insert(handInventoryLocation);
		}
		
		return success;
	}

	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		AttachMagazineActionData action_data_am = AttachMagazineActionData.Cast(action_data);
		Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
		Magazine mag = Magazine.Cast(action_data.m_Target.GetObject());	
		ClearInventoryReservationEx(action_data);
		if ( action_data.m_Player.GetWeaponManager().CanAttachMagazine(wpn,mag,false) )
			action_data.m_Player.GetWeaponManager().AttachMagazine(mag, this);
		else
			action_data.m_Player.GetWeaponManager().SwapMagazineEx(mag, action_data_am.m_ilOldMagazine, this);
		 InventoryReservation(action_data);
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool HasProgress()
	{
		return false;
	}
	
	override typename GetInputType()
	{
		return ContinuousWeaponManipulationActionInput;
	} 
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTSelf;
	}
};