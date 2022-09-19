int SlotToAnimType (notnull Man player, notnull InventoryLocation src, InventoryLocation dst = null)
{
	//Print("src.GetType() " + src.GetType());
	InventoryLocation invloc1 = new InventoryLocation;
	//InventoryLocation invloc2 = new InventoryLocation;
	
	if ( (dst && dst.GetParent() && !dst.GetParent().GetHierarchyRootPlayer()) || (src && src.GetParent() && !src.GetParent().GetHierarchyRootPlayer()) )
		return -1;
	
	if (dst && (dst.GetType() == InventoryLocationType.ATTACHMENT || dst.GetType() == InventoryLocationType.CARGO))
	{
		invloc1.Copy(dst);
		//invloc2.Copy(src);
	}
	else if (src.GetType() == InventoryLocationType.ATTACHMENT || src.GetType() == InventoryLocationType.CARGO)
	{
		invloc1.Copy(src);
		//invloc2.Copy(dst);
	}
	else
	{
		return -1;
	}
	
	int val = -1;
	if ( invloc1.GetItem() && invloc1.GetItem().GetInventoryHandAnimation(invloc1,val) )
	{
		return val;
	}
	
	if (invloc1.GetType() == InventoryLocationType.ATTACHMENT /*|| src.GetType() == InventoryLocationType.HANDS*/)
	{
		//return WeaponHideShowTypes.HIDESHOW_SLOT_KNIFEBACK;
		switch (invloc1.GetSlot())
		{
			case InventorySlots.SHOULDER:
			{
				if (invloc1.GetItem() && invloc1.GetItem().IsWeapon())
				{
					return WeaponHideShowTypes.HIDESHOW_SLOT_RFLLEFTBACK;
				}
				else if (invloc1.GetItem() && invloc1.GetItem().IsOneHandedBehaviour())
				{
					return WeaponHideShowTypes.HIDESHOW_SLOT_1HDLEFTBACK;
				}
				return WeaponHideShowTypes.HIDESHOW_SLOT_2HDLEFTBACK;
			}
			case InventorySlots.MELEE:
			{
				if (invloc1.GetItem() && invloc1.GetItem().IsWeapon())
				{
					return WeaponHideShowTypes.HIDESHOW_SLOT_RFLRIGHTBACK;
				}
				else if (invloc1.GetItem() && invloc1.GetItem().IsOneHandedBehaviour())
				{
					return WeaponHideShowTypes.HIDESHOW_SLOT_1HDRIGHTBACK;
				}
				return WeaponHideShowTypes.HIDESHOW_SLOT_2HDRIGHTBACK;
			}
			case InventorySlots.PISTOL:
			{
				EntityAI parent_item = invloc1.GetParent(); 		// belt
				Man owner;
				if (parent_item)
					owner = parent_item.GetHierarchyRootPlayer(); 		// player
				if (!owner)
					return -1;
				
				EntityAI item1 = owner.GetInventory().FindAttachment(InventorySlots.HIPS);
				EntityAI item2 = parent_item.GetHierarchyParent();
				if (owner && item1 == item2) // is the pistol in a belt holster?
				{
					return WeaponHideShowTypes.HIDESHOW_SLOT_PISTOLBELT;
				}
				return WeaponHideShowTypes.HIDESHOW_SLOT_PISTOLCHEST;
			}
			case InventorySlots.KNIFE:
				return WeaponHideShowTypes.HIDESHOW_SLOT_KNIFEBACK;
			
			/*case InventorySlots.VEST:
			case InventorySlots.FEET:
			case InventorySlots.BODY:
			case InventorySlots.LEGS:
			case InventorySlots.BACK:
			case InventorySlots.HIPS:
			case InventorySlots.HEADGEAR:
				return WeaponHideShowTypes.HIDESHOW_SLOT_INVENTORY;*/
			
			default:
				return WeaponHideShowTypes.HIDESHOW_SLOT_INVENTORY;
				//Print("[hndfsm] SlotToAnimType -  not animated slot in src_loc=" + InventoryLocation.DumpToStringNullSafe(invloc1));
		};
		//
		//if (InventorySlots.GetSlotIdFromString("Pistol"))
	}
	else if (invloc1.GetType() == InventoryLocationType.CARGO)
	{
		if ( invloc1.GetItem() && (invloc1.GetItem().GetInventory().HasInventorySlot(InventorySlots.SHOULDER) || invloc1.GetItem().GetInventory().HasInventorySlot(InventorySlots.MELEE)) )
		{
			//Print("Special inventory anim");
			if (invloc1.GetItem() && invloc1.GetItem().IsWeapon())
			{
				return WeaponHideShowTypes.HIDESHOW_SLOT_RFLRIGHTBACK;
			}
			else if (invloc1.GetItem() && invloc1.GetItem().IsOneHandedBehaviour())
			{
				return WeaponHideShowTypes.HIDESHOW_SLOT_1HDRIGHTBACK;
			}
			return WeaponHideShowTypes.HIDESHOW_SLOT_2HDRIGHTBACK;
		}
		//Print("Default inventory anim");
		return WeaponHideShowTypes.HIDESHOW_SLOT_INVENTORY; //default item animation
	}
	return -1;
}

bool SelectAnimationOfTakeToHands (notnull Man player, notnull InventoryLocation src, notnull InventoryLocation dst, out int animType)
{
	if (player.IsInTransport())
		return false;
	if (src.GetType() == InventoryLocationType.GROUND)
		return false;

	if (src.GetItem().GetHierarchyRootPlayer() == player)
	{	
		animType = SlotToAnimType(player, src);
		if (animType != -1)
		{
			hndDebugPrint("[hndfsm] SelectAnimationOfTakeToHands - selected animType=" + animType + " for item=" + src.GetItem());
			return true;
		}
	}
	hndDebugPrint("[hndfsm] SelectAnimationOfTakeToHands - no animation");
	return false;
}

bool SelectAnimationOfMoveFromHands (notnull Man player, notnull InventoryLocation src, notnull InventoryLocation dst, out int animType)
{
	if (player.IsInTransport())
		return false;

	if (src.GetItem().GetHierarchyRootPlayer() == player)
	{	
		animType = SlotToAnimType(player, dst);
		if (animType != -1)
		{
			hndDebugPrint("[hndfsm] SelectAnimationOfMoveFromHands guard - selected animType=" + animType + " for item=" + src.GetItem());
			return true;
		}
	}
	hndDebugPrint("[hndfsm] SelectAnimationOfMoveFromHands - no animation");
	return false;
}

bool SelectAnimationOfForceSwapInHands (notnull Man player, notnull InventoryLocation old_src, notnull InventoryLocation new_src, notnull InventoryLocation old_dst, notnull InventoryLocation new_dst, out int animType1, out int animType2)
{
	if (player.IsInTransport())
		return false;
	
	hndDebugPrint("[hndfsm] SlotToAnimType - old_src=" + InventoryLocation.DumpToStringNullSafe(old_src) + " new_src=" + InventoryLocation.DumpToStringNullSafe(new_src) + " old_dst=" + InventoryLocation.DumpToStringNullSafe(old_dst) + " new_dst=" + InventoryLocation.DumpToStringNullSafe(new_dst));

	if (old_src.GetItem().GetHierarchyRootPlayer() == player || new_src.GetItem().GetHierarchyRootPlayer() == player)
	{
		animType1 = SlotToAnimType(player, new_dst, new_src);
		animType2 = SlotToAnimType(player, old_src, old_dst);
		//Print("animType1 = " + animType1);
		//Print("animType2 = " + animType2);
		if (animType1 != -1 && animType2 != -1)
		{
			hndDebugPrint("[hndfsm] SelectAnimationOfForceSwapInHands guard - selected animType1=" + animType1 + " animType2=" + animType2 + " for old_item=" + old_src.GetItem() + " for new_item=" + new_src.GetItem());
			return true;
		}
		/*else if (animType1 != -1 || animType2 != -1) //HACK
		{
			animType1 = -1;
			animType2 = -1;
			return false;
		}*/
	}
	hndDebugPrint("[hndfsm] SelectAnimationOfForceSwapInHands - no animation");
	return false;
}


class HandSelectAnimationOfTakeToHandsEvent extends HandGuardBase
{
	void HandSelectAnimationOfTakeToHandsEvent (Man p = NULL) { }

	override bool GuardCondition (HandEventBase e)
	{
		int animType = -1;
		if (SelectAnimationOfTakeToHands(e.m_Player, e.GetSrc(), e.GetDst(), animType))
		{
			e.m_AnimationID = animType;
			return true;
		}
		return false;
	}
};

class HandSelectAnimationOfMoveFromHandsEvent extends HandGuardBase
{
	protected Man m_Player;
	ref HandGuardHasRoomForItem m_HasRoomGuard;

	void HandSelectAnimationOfMoveFromHandsEvent (Man p = NULL) {
		 m_Player = p; 
		m_HasRoomGuard = new HandGuardHasRoomForItem(p);
	}

	override bool GuardCondition (HandEventBase e)
	{
		if (m_HasRoomGuard.GuardCondition(e))
		{
			EntityAI eai = m_Player.GetHumanInventory().GetEntityInHands();
			if (eai)
			{
				InventoryLocation src = new InventoryLocation;
				if (eai.GetInventory().GetCurrentInventoryLocation(src))
				{
					int animType = -1;
					if (SelectAnimationOfMoveFromHands(e.m_Player, src, e.GetDst(), animType))
					{
						e.m_AnimationID = animType;
						return true;
					}
					return false;
				}
			}
		}
		return false;
	}
};

class HandSelectAnimationOfForceSwapInHandsEvent extends HandGuardBase
{
	protected Man m_Player;

	void HandSelectAnimationOfForceSwapInHandsEvent (Man p = NULL) { m_Player = p; }

	bool ProcessSwapEvent (notnull HandEventBase e, out int animType1, out int animType2)
	{
		HandEventSwap es = HandEventSwap.Cast(e);
		if (es)
			return SelectAnimationOfForceSwapInHands(e.m_Player, es.m_Src, es.m_Src2, es.m_Dst, es.m_Dst2, animType1, animType2);
		Error("HandSelectAnimationOfForceSwapInHandsEvent - not an swap event");
		return false;
	}

	override bool GuardCondition (HandEventBase e)
	{
		HandEventForceSwap es = HandEventForceSwap.Cast(e);
		if (es)
		{
			hndDebugPrint("[hndfsm] HandSelectAnimationOfForceSwapInHandsEvent FSwap e=" + e.DumpToString());
			
			if( !es.m_Src2.IsValid() || !es.m_Src.IsValid() )
			{
				Error("[hndfsm] HandSelectAnimationOfForceSwapInHandsEvent - invalid item source");
				return false;
			}
			
			bool allow = false;
			if (GameInventory.CanSwapEntitiesEx(es.GetSrc().GetItem(), es.m_Src2.GetItem()))
				allow = true; // allow if ordinary swap
			else if (es.m_Dst2)
			{
				if (!GameInventory.CanForceSwapEntitiesEx(es.GetSrc().GetItem(), null, es.m_Src2.GetItem(), es.m_Dst2))
				{
					Error("[hndfsm] HandSelectAnimationOfForceSwapInHandsEvent - no room at dst=" + InventoryLocation.DumpToStringNullSafe(es.m_Dst2));
				}
				else
				{
					allow = true;
				}
			}
			
			if (allow)
			{
				int animType1 = -1;
				int animType2 = -1;
				if (ProcessSwapEvent(e, animType1, animType2))
				{
					e.m_AnimationID = animType1;
					es.m_Animation2ID = animType2;
					return true;
				}
			}
			else
				Error("[hndfsm] HandSelectAnimationOfForceSwapInHandsEvent - m_HasRoomGuard.GuardCondition failed");
		}
		else
			Error("[hndfsm] HandSelectAnimationOfForceSwapInHandsEvent - not a swap event");
		return false;
	}
};

class HandSelectAnimationOfSwapInHandsEvent extends HandSelectAnimationOfForceSwapInHandsEvent
{
	override bool GuardCondition (HandEventBase e)
	{
		HandEventSwap es = HandEventSwap.Cast(e);
		if (es)
		{
			int animType1 = -1;
			int animType2 = -1;
			if (ProcessSwapEvent(e, animType1, animType2))
			{
				e.m_AnimationID = animType1;
				es.m_Animation2ID = animType2;
				return true;
			}
		}
		else
			Error("[hndfsm] HandSelectAnimationOfSwapInHandsEvent - not a swap event");
		return false;
	}
};

