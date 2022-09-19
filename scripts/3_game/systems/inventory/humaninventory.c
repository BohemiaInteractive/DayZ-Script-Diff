/**@class		HumanInventory
 * @brief		inventory for plain man/human
 *
 * HumanInventory has simple synchronous operations only, i.e.
 * no animations are involved while adding/removing/swapping to/from hands.
 *
 * Animations are added on higher level of hierarchy (DayZPlayerInventory for example)
 **/
class HumanInventory : GameInventory
{	
	int m_syncClearUserReservationindex = -1;
	//int m_UserReservationToClear = -1;
	/**@fn			GetEntityInHands
	 * @return		entity in hands
	 **/
	proto native EntityAI GetEntityInHands ();

	/**@fn			CanAddEntityInHands
	 * @brief		alternative for TestAddEntityInHands(e, true, true, true);
	 **/
	proto native bool CanAddEntityInHands (EntityAI e);

	/**@fn			TestAddEntityInHands
	 * @param[in]	e	entity to test for taking in hands
	 * @param[in]	do_item_check		deny if entity is not InventoryItem
	 * @param[in]	do_occupancy_test		deny if there is item in hands already
	 * @param[in]	do_script_check		deny if script conditions fail
	 * @return		true if item passed all tests
	 **/
	proto native bool TestAddEntityInHands (EntityAI e, bool do_resevation_check, bool do_item_check, bool do_lock_check, bool do_occupancy_test, bool do_script_check);

	/**@fn			CanRemoveEntityInHands
	 * @return		true if entity can be removed from hands
	 **/
	proto native bool CanRemoveEntityInHands ();

	proto native bool CanOpenInventory();

	/**@fn			CreateInHands
	 * @brief		creates new entity in hands
	 * @param[in]	typeName type name of the entity to be created
	 * @return		new entity or null otherwise
	 **/
	proto native EntityAI CreateInHands (string typeName);

	proto native int GetUserReservedLocationCount ();
	proto native int FindUserReservedLocationIndex (notnull EntityAI e);
	proto native int FindCollidingUserReservedLocationIndex (notnull EntityAI e, notnull InventoryLocation dst);
	proto native void GetUserReservedLocation (int index, out notnull InventoryLocation dst);
	proto native int FindFirstUserReservedLocationIndexForContainer(notnull EntityAI e);
	
	proto native void SetUserReservedLocation (notnull EntityAI eai, notnull InventoryLocation dst);
	proto native void ClearUserReservedLocation (notnull EntityAI eai);
	proto native bool ClearUserReservedLocationAtIndex (int index);
	proto native void ClearUserReservedLocationForContainer (notnull EntityAI eai);
	proto native bool GetDebugFlag ();

	/**
	 * @fn		CreateInInventory
	 * @brief	creates entity somewhere in inventory
	 *
	 * @param[in]	type	\p		item type to be placed in inventory
	 * @return	created entity
	 **/
	override EntityAI CreateInInventory (string type)
	{
		EntityAI newEntity = super.CreateInInventory(type);
		if (newEntity == null)
			newEntity = CreateInHands(type);
		return newEntity;
	}
	
	void ClearUserReservedLocationSynced (notnull EntityAI eai)
	{
		if( GetGame().IsClient() )
			m_syncClearUserReservationindex = FindUserReservedLocationIndex(eai);
		else if (!GetGame().IsMultiplayer())
		{
			ClearUserReservedLocation(eai);
			eai.GetOnReleaseLock().Invoke(eai);
		}
			
	}
	
	void ClearUserReservedLocationAtIndexSynced(int index)
	{
		if( GetGame().IsClient() )
			m_syncClearUserReservationindex = index;
		else if (!GetGame().IsMultiplayer())
		{
			ClearUserReservedLocationAtIndex(index);
			
			InventoryLocation il = new InventoryLocation;
			
			GetUserReservedLocation(index,il);
			EntityAI item = il.GetItem();
			item.GetOnReleaseLock().Invoke(item);
		}
			
	}

	Man GetManOwner () { return Man.Cast(GetInventoryOwner()); }

	bool HasEntityInHands (EntityAI e) { return e == GetEntityInHands(); }
	
	bool ProcessHandEvent (HandEventBase e) { }

	void OnHandsStateChanged (HandStateBase src, HandStateBase dst) { }
	void OnHandsExitedStableState (HandStateBase src, HandStateBase dst) { }
	void OnHandsEnteredStableState (HandStateBase src, HandStateBase dst) { }
	
	void OnEntityInHandsCreated (InventoryLocation src)
	{
		InventoryLocation cpy = new InventoryLocation;
		cpy.Copy(src);
		hndDebugPrint("[inv] OnEntityInHandsCreated src=" + InventoryLocation.DumpToStringNullSafe(cpy));
		ProcessHandEvent(new HandEventCreated(GetManOwner(), src));
	}

	void OnEntityInHandsDestroyed (InventoryLocation src)
	{
		InventoryLocation cpy = new InventoryLocation;
		cpy.Copy(src);
		hndDebugPrint("[inv] OnEntityInHandsDestroyed src=" + InventoryLocation.DumpToStringNullSafe(cpy));
		ProcessHandEvent(new HandEventDestroyed(GetManOwner(), cpy));
	}

	bool HandEvent (InventoryMode mode, HandEventBase e)
	{
		return true;
	}
	
	override bool DropEntity (InventoryMode mode, EntityAI owner, notnull EntityAI item)
	{
		InventoryLocation src = new InventoryLocation;
		if (item && item.GetInventory() && item.GetInventory().GetCurrentInventoryLocation(src))
		{
			switch (src.GetType())
			{
				case InventoryLocationType.HANDS:
					hndDebugPrint("[inv] HumanInventory::DropEntity(" + typename.EnumToString(InventoryMode, mode) + ") item=" + item);
					HandEvent(mode, new HandEventDrop(GetManOwner(), src));
					return true;

				default: return super.DropEntity(mode, owner, item);
			}
		}
		Error("No inventory location");
		return false;
	}
	
	bool ThrowEntity(EntityAI item, vector dir, float force)
	{
		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
			return false;
		
		DayZPlayer player = DayZPlayer.Cast(item.GetHierarchyRootPlayer());
		InventoryMode invMode = InventoryMode.PREDICTIVE;
				
		if ( player.NeedInventoryJunctureFromServer( item, item.GetHierarchyParent(), null) )
			invMode = InventoryMode.JUNCTURE;
		
		InventoryLocation src = new InventoryLocation;
		if (item && item.GetInventory() && item.GetInventory().GetCurrentInventoryLocation(src))
		{
			switch (src.GetType())
			{
				case InventoryLocationType.HANDS:
					hndDebugPrint("[inv] HumanInventory::ThrowEntity item=" + item);
					HandEventThrow throwEvent = new HandEventThrow(GetManOwner(), src);
					throwEvent.SetForce(dir * force);
					HandEvent(invMode, throwEvent);
					return true;

				//default: return super.DropEntity(InventoryMode.JUNCTURE, src.GetParent(), item);
				default:
					DropEntity(invMode, player, item);
					return true;
			}
		}
		Error("No inventory location");
		return false;
	}
		
	bool RedirectToHandEvent (InventoryMode mode, notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		if (src.GetType() == InventoryLocationType.HANDS)
		{
			Man man_src = Man.Cast(src.GetParent());
			hndDebugPrint("[inv] HI::RedirectToHandEvent - source location == HANDS, player has to handle this");
			
			EntityAI item = src.GetItem();
			
			int r_index = FindUserReservedLocationIndex(item);

			if(r_index >= 0)
			{
				InventoryLocation r_il = new InventoryLocation;
				GetUserReservedLocation(r_index,r_il);

				ClearUserReservedLocationAtIndex(r_index);
				int r_type = r_il.GetType();
				if( r_type == InventoryLocationType.CARGO || r_type == InventoryLocationType.PROXYCARGO )
				{
					r_il.GetParent().GetOnReleaseLock().Invoke( item );
				}
				else if( r_type == InventoryLocationType.ATTACHMENT )
				{
					r_il.GetParent().GetOnAttachmentReleaseLock().Invoke( item, r_il.GetSlot() );
				}
			}
	
			man_src.GetHumanInventory().HandEvent(mode, new HandEventMoveTo(man_src, src, dst));
			return true;
		}
		
		if (dst.GetType() == InventoryLocationType.HANDS)
		{
			hndDebugPrint("[inv] HI::RedirectToHandEvent - dst location == HANDS, player has to handle this");
						
			Man man_dst = Man.Cast(dst.GetParent());
			man_dst.GetHumanInventory().HandEvent(mode, new HandEventTake(man_dst, src));
			return true;
		}
		return false;
	}

	override bool TakeToDst (InventoryMode mode, notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		hndDebugPrint("[inv] Take2Dst(" + typename.EnumToString(InventoryMode, mode) + ") src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));

		if (GetManOwner().IsAlive() && RedirectToHandEvent(mode, src, dst))
			return true;

		return super.TakeToDst(mode, src, dst);
	}

	override bool TakeEntityToInventory (InventoryMode mode, FindInventoryLocationType flags, notnull EntityAI item)
	{
		InventoryLocation src = new InventoryLocation;
		if (item.GetInventory().GetCurrentInventoryLocation(src))
		{
			InventoryLocation dst = new InventoryLocation;
			if (FindFreeLocationFor(item, flags, dst))
			{
				if (RedirectToHandEvent(mode, src, dst))
					return true;

				hndDebugPrint("[inv] HumanInventory::Take2Inv(" + typename.EnumToString(InventoryMode, mode) + ") item=" + item);
				return super.TakeEntityToInventory(mode, flags, item);
			}
			else
			{
				// no room
				return false;
			}
		}
		Error("HumanInventory::TakeEntityToInventory: No inventory location");
		return false;
	}

	/**
	\brief Put item into into cargo on specific cargo location
	*/
	override bool TakeEntityToCargoEx (InventoryMode mode, notnull EntityAI item, int idx, int row, int col)
	{
		InventoryLocation src = new InventoryLocation;
		if (item.GetInventory().GetCurrentInventoryLocation(src))
		{
			switch (src.GetType())
			{
				case InventoryLocationType.HANDS:
				{
					if (GetInventoryOwner().IsAlive())
					{
						hndDebugPrint("[inv] HumanInventory::Take2Cgo(" + typename.EnumToString(InventoryMode, mode) + ") item=" + item + " row=" + row + " col=" + col);
						InventoryLocation dst = new InventoryLocation;
						dst.SetCargo(GetInventoryOwner(), item, idx, row, col, item.GetInventory().GetFlipCargo());

						HandEvent(mode, new HandEventMoveTo(GetManOwner(), src, dst));
						return true;
					}
					return super.TakeEntityToCargoEx(mode, item, idx, row, col);
				}
				default: return super.TakeEntityToCargoEx(mode, item, idx, row, col);
			}
		}
		Error("HumanInventory::TakeEntityToCargoEx: No inventory location");
		return false;
	}

	override bool TakeEntityAsAttachmentEx (InventoryMode mode, notnull EntityAI item, int slot)
	{
		InventoryLocation src = new InventoryLocation;
		if (item.GetInventory().GetCurrentInventoryLocation(src))
		{
			switch (src.GetType())
			{
				case InventoryLocationType.HANDS:
				{
					if (GetInventoryOwner().IsAlive())
					{
						hndDebugPrint("[inv] HumanInventory::Take2Att(" + typename.EnumToString(InventoryMode, mode) + ") item=" + item + " slot=" + slot);
						InventoryLocation dst = new InventoryLocation;
						EntityAI src_entity = GetInventoryOwner();
						dst.SetAttachment(src_entity, item, slot);

						HandEvent(mode, new HandEventMoveTo(GetManOwner(), src, dst));
						return true;
					}
					return super.TakeEntityAsAttachmentEx(mode, item, slot);
				}
				default: return super.TakeEntityAsAttachmentEx(mode, item, slot);
			}
		}
		Error("HumanInventory::TakeEntityAsAttachmentEx: No inventory location");
		return false;
	}

	override bool SwapEntities (InventoryMode mode, notnull EntityAI item1, notnull EntityAI item2)
	{
		InventoryLocation src1, src2, dst1, dst2;
		if (GameInventory.MakeSrcAndDstForSwap(item1, item2, src1, src2, dst1, dst2))
		{
			bool handled = false;
			switch (src1.GetType())
			{
				case InventoryLocationType.HANDS:
				{
					//! returns item to previous location, if available
					InventoryLocation fswap_dst2 = new InventoryLocation;
					if ( SwappingToPreviousLocation(item1, item2, fswap_dst2) )
					{
						hndDebugPrint("[inv] HumanInventory::Swap HandEventForceSwap(" + typename.EnumToString(InventoryMode, mode) + ") IH=src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2));
						HandEvent(mode, new HandEventForceSwap(GetManOwner(), src1, src2, dst1, fswap_dst2));
					}
					else
					{
						hndDebugPrint("[inv] HumanInventory::Swap HandEventSwap(" + typename.EnumToString(InventoryMode, mode) + ") src1=" + InventoryLocation.DumpToStringNullSafe(src1) + "src2=" + InventoryLocation.DumpToStringNullSafe(src2));
						HandEvent(mode, new HandEventSwap(GetManOwner(), src1, src2, dst1, dst2));
					}
					handled = true;
					break;
				}
			}

			switch (src2.GetType())
			{
				case InventoryLocationType.HANDS:
				{
					hndDebugPrint("[inv] HumanInventory::Swap HandEventSwap2(" + typename.EnumToString(InventoryMode, mode) + ") src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " IH=src2=" + InventoryLocation.DumpToStringNullSafe(src2));
					HandEvent(mode, new HandEventSwap(GetManOwner(), src1, src2, dst1, dst2));
					handled = true;
					break;
				}
			}

			if (!handled)
				return super.SwapEntities(mode, item1, item2);
			return true;
		}
		Error("HumanInventory::SwapEntities: cannot create src1, src2, dst1, dst2");
		return false;
	}

	override bool ForceSwapEntities(InventoryMode mode, notnull EntityAI item1, notnull EntityAI item2, notnull InventoryLocation item2_dst)
	{
		InventoryLocation src1, src2, dst1;
		if (GameInventory.MakeSrcAndDstForForceSwap(item1, item2, src1, src2, dst1, item2_dst))
		{
			GameInventory manOwnerInventory = GetManOwner().GetInventory();
			
			hndDebugPrint("[inv] HumanInventory::FSwap(" + typename.EnumToString(InventoryMode, mode) + ") dst1=" + InventoryLocation.DumpToStringNullSafe(dst1)+ " dst2=" + InventoryLocation.DumpToStringNullSafe(item2_dst));
			bool handled = false;
			switch (src1.GetType())
			{
				case InventoryLocationType.HANDS:
				{
					hndDebugPrint("[inv] HumanInventory::FSwap-HND@1(" + typename.EnumToString(InventoryMode, mode) + ") src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " item2_dst=" + InventoryLocation.DumpToStringNullSafe(item2_dst));
					HandEventBase e = new HandEventForceSwap(GetManOwner(), src1, src2, dst1, item2_dst);
					e.ReserveInventory();
					HandEvent(mode, e);
					handled = true;
					break;
				}
			}

			switch (src2.GetType())
			{
				case InventoryLocationType.HANDS:
				{
					//! returns item to previous location, if available
					InventoryLocation fswap_dst2 = new InventoryLocation;
					if ( SwappingToPreviousLocation(item2, item1, fswap_dst2))
					{
						hndDebugPrint("[inv] HumanInventory::FSwap-PREV(" + typename.EnumToString(InventoryMode, mode) + ") src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " fswap_dst2=" + InventoryLocation.DumpToStringNullSafe(fswap_dst2));
						HandEvent(mode, new HandEventForceSwap(GetManOwner(), src1, src2, dst1, fswap_dst2));
					}
					else
					{
						hndDebugPrint("[inv] HumanInventory::FSwap-HND@2(" + typename.EnumToString(InventoryMode, mode) + ") src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " item2_dst=" + InventoryLocation.DumpToStringNullSafe(item2_dst));
						HandEvent(mode, new HandEventForceSwap(GetManOwner(), src1, src2, dst1, item2_dst));
					}
					handled = true;	
					break;
				}
			}
			
			bool returnValue = true;

			if (!handled)
				returnValue = super.ForceSwapEntities(mode, item1, item2, item2_dst);
			
			return returnValue;
		}
		Error("HumanInventory::ForceSwapEntities: No inventory location");
		return false;
	}

	override bool LocalDestroyEntity (notnull EntityAI item)
	{
		InventoryLocation src = new InventoryLocation;
		if (item.GetInventory().GetCurrentInventoryLocation(src))
		{
			switch (src.GetType())
			{
				case InventoryLocationType.HANDS:
				{
					if (GetInventoryOwner().IsAlive())
					{
						hndDebugPrint("[inv] HumanInventory::LocalDestroy inv item=" + item);
						HandEvent(InventoryMode.LOCAL, new HandEventDestroy(GetManOwner(), src));
						return true;
					}
					return super.LocalDestroyEntity(item);
				}
				default: return super.LocalDestroyEntity(item);
			}
		}
		Error("LocalDestroyEntity: No inventory location");
		return false;
	}

	override bool ReplaceItemWithNew (InventoryMode mode, ReplaceItemWithNewLambdaBase lambda)
	{
		EntityAI itemInHands = GetEntityInHands();
		if (itemInHands == lambda.m_OldItem)
			return ReplaceItemInHandsWithNew(mode, lambda);
		return super.ReplaceItemWithNew(mode, lambda);
	}
	
	bool ReplaceItemElsewhereWithNewInHands (InventoryMode mode, ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemInElsewhereWithNewinHandsImpl(mode, new HandEventDestroyElsewhereAndReplaceWithNewInHands(GetManOwner(), null, lambda));
	}
	
	protected bool ReplaceItemInElsewhereWithNewinHandsImpl (InventoryMode mode, HandEventBase e)
	{
		if (GetEntityInHands())
		{
			Error("[inv] HumanInventory::ReplaceItemInElsewhereWithNewinHandsImpl Item in hands, event=" + e.DumpToString());
			return false;
		}

		InventoryLocation dst = e.GetDst();
		if (dst)
		{
			switch (dst.GetType())
			{
				case InventoryLocationType.HANDS:
				{
					if (GetInventoryOwner().IsAlive())
					{
						hndDebugPrint("[inv] HumanInventory::ReplaceItemInElsewhereWithNewinHandsImpl event=" + e);
						HandEvent(mode, e);
						return true;
					}

					hndDebugPrint("[inv] HumanInventory::ReplaceItemInElsewhereWithNewinHandsImpl DEAD_owner=" + GetInventoryOwner().GetName() +"="+ GetInventoryOwner());
					Error("HumanInventory::ReplaceItemInElsewhereWithNewinHandsImpl TODO"); // replace-with-new in corpse's hands, not implemented
					return false;
				}
				default:
					Error("[inv] HumanInventory::ReplaceItemInElsewhereWithNewinHandsImpl src has to be hands");
					return false;
			}
		}
		Error("[inv] HumanInventory::ReplaceItemInElsewhereWithNewinHandsImpl no dst in event, event=" + e.DumpToString());
		return false;
	}

	protected bool ReplaceItemInHandsWithNewImpl (InventoryMode mode, HandEventBase e)
	{
		EntityAI itemInHands = GetEntityInHands();
		InventoryLocation src = new InventoryLocation;
		if (itemInHands && itemInHands.GetInventory().GetCurrentInventoryLocation(src))
		{
			switch (src.GetType())
			{
				case InventoryLocationType.HANDS:
				{
					if ((mode != InventoryMode.SERVER) && GetInventoryOwner().IsAlive())
					{
						hndDebugPrint("[inv] HumanInventory::ReplaceItemInHandsWithNewImpl event=" + e);
						HandEvent(mode, e);
						return true;
					}

					hndDebugPrint("[inv] HumanInventory::ReplaceItemInHandsWithNewImpl DEAD_owner=" + GetInventoryOwner().GetName() +"="+ GetInventoryOwner());
					//Error("HumanInventory::ReplaceItemInHandsWithNewImpl TODO"); // replace-with-new in corpse's hands, not implemented
					HandEvent(mode, e);
					return true;
				}
				default:
					Error("[inv] HumanInventory::ReplaceItemInHandsWithNewImpl src has to be hands");
					return false;
			}
		}
		Error("[inv] HumanInventory::ReplaceItemInHandsWithNewImpl No item in hands, event=" + e.DumpToString());
		return false;
	}

	bool ReplaceItemInHandsWithNew (InventoryMode mode, ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemInHandsWithNewImpl(mode, new HandEventDestroyAndReplaceWithNew(GetManOwner(), null, lambda));
	}

	bool ReplaceItemInHandsWithNewElsewhere (InventoryMode mode, ReplaceItemWithNewLambdaBase lambda)
	{
		return ReplaceItemInHandsWithNewImpl(mode, new HandEventDestroyAndReplaceWithNewElsewhere(GetManOwner(), null, lambda));
	}
	
	bool SwappingToPreviousLocation (EntityAI item1, EntityAI item2, out InventoryLocation dst)
	{
		bool SwapToPrevious = true;
		InventoryLocation src1 = new InventoryLocation;
		InventoryLocation src2 = new InventoryLocation;
		dst = new InventoryLocation;
		if (item1.GetInventory().GetCurrentInventoryLocation(src1) && item2.GetInventory().GetCurrentInventoryLocation(src2))
		{
			if (item1.m_OldLocation && item1.m_OldLocation.IsValid() && !item1.m_OldLocation.CollidesWith(src2) && item1.m_OldLocation.GetParent() && item1.m_OldLocation.GetParent().GetHierarchyRootPlayer())	
			{
				dst.Copy(item1.m_OldLocation);
				
				int count = item1.GetInventory().GetSlotIdCount();
				if ( count > 0 && src2.GetType() == InventoryLocationType.ATTACHMENT ) //item2 is currently attached somewhere
				{
					//Print("src2.GetSlot() " + src2.GetSlot());
					for (int i = 0; i < count; i++)
					{
						int slotID = item1.GetInventory().GetSlotId(i);
						//Print("slotID " + slotID);
						if (src2.GetSlot() == slotID) //can be attached into the same slot. And will be.
							return false;
					}
				}
				
				//if ( item1.m_OldLocation.GetType() == InventoryLocationType.CARGO/* || InventoryLocationType.PROXYCARGO*/ ) //should return to cargo, verify free space
				//{
					if ( !item1.m_OldLocation.GetParent().GetInventory().LocationCanAddEntity(item1.m_OldLocation) ) //TODO: is this correct usage?
					{
						//FindFreeLocationFor(item1,FindInventoryLocationType.ANY,dst);
						SwapToPrevious = false;
					}
					if ( CanSwapEntitiesEx(item1,item2) )
					{
						SwapToPrevious = false;
					}
				//}
				
				if ( SwapToPrevious/*dst.CompareLocationOnly(item1.m_OldLocation)*/ )
					return true;
			}
		}
		
		return false;
	}
	
	void HandleInventoryManipulation()
	{}
	
	void Update(float delta_time)
	{
		HandleInventoryManipulation();
		if ( m_syncClearUserReservationindex != -1 && ScriptInputUserData.CanStoreInputUserData())
		{
			ScriptInputUserData ctx = new ScriptInputUserData;
			ctx.Write(INPUT_UDT_INVENTORY);
			ctx.Write(InventoryCommandType.USER_RESERVATION_CANCEL);
			ctx.Write(m_syncClearUserReservationindex);
			ctx.Send();
			
			InventoryLocation il = new InventoryLocation;
			
			GetUserReservedLocation(m_syncClearUserReservationindex,il);
			ClearUserReservedLocationAtIndex(m_syncClearUserReservationindex);
			EntityAI item = il.GetItem();
			item.GetOnReleaseLock().Invoke(item);
			m_syncClearUserReservationindex = -1;
		}
		
		/*if( m_UserReservationToClear != -1 )
		{
			ClearUserReservedLocationAtIndex(m_UserReservationToClear);
			m_UserReservationToClear = -1;
		}*/
	}
	
	
	
	bool ProccessInputData(int type, bool handling_juncture, bool remote, ParamsReadContext ctx)
	{
		InventoryLocation correct_il;
		ScriptJunctureData ctx_repair;

		switch (type)
		{
			case InventoryCommandType.USER_RESERVATION_CANCEL:
			{
				int index = -1;
				
				if(!ctx.Read(index))
					return true;
				
				ClearUserReservedLocationAtIndex(index);
				//m_UserReservationToClear = index;
				
				return true;
			}
		}
		return false;
	}
}

