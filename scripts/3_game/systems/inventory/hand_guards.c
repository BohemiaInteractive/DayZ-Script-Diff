///@{ guards

/**@class	HandGuardBase
 * @brief	represents guard on a transition from state to state
 **/
class HandGuardBase
{
	/**@fn			GuardCondition
	 * @brief		enable or disable transition based on condition
	 * @note		Item is guaranteed to exist on remote client due to replication relationships
	 * @note		The guard is a boolean operation executed first and which can prevent the transition from firing by returning false
	 * @return		true if transition is allowed
	 **/
	bool GuardCondition(HandEventBase e) { return true; }
};


class HandGuardAnd extends HandGuardBase
{
	ref HandGuardBase m_arg0;
	ref HandGuardBase m_arg1;

	void HandGuardAnd(HandGuardBase arg0 = null, HandGuardBase arg1 = null) { m_arg0 = arg0; m_arg1 = arg1; }

	override bool GuardCondition(HandEventBase e)
	{
		bool result = m_arg0.GuardCondition(e) && m_arg1.GuardCondition(e);
		
		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result + " - " + m_arg0.Type() + " && " + m_arg1.Type(), "HandGuardAnd" , "n/a", "GuardCondition", e.m_Player.ToString() );
		}
		#endif
		
		return result;
	}
};

class HandGuardNot extends HandGuardBase
{
	ref HandGuardBase m_arg0;

	void HandGuardNot(HandGuardBase arg0 = null) { m_arg0 = arg0; }

	override bool GuardCondition(HandEventBase e)
	{
		bool result = !m_arg0.GuardCondition(e);
		
		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result + " - " + m_arg0.Type(), "HandGuardNot" , "n/a", "GuardCondition", e.m_Player.ToString() );
		}
		#endif
		return result;
	}
};

class HandGuardOr extends HandGuardBase
{
	ref HandGuardBase m_arg0;
	ref HandGuardBase m_arg1;

	void HandGuardOr(HandGuardBase arg0 = null, HandGuardBase arg1 = null) { m_arg0 = arg0; m_arg1 = arg1; }

	override bool GuardCondition(HandEventBase e)
	{
		bool result = m_arg0.GuardCondition(e) || m_arg1.GuardCondition(e);
		
		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result + " - " + m_arg0.Type() + " || " + m_arg1.Type(), "HandGuardOr" , "n/a", "GuardCondition", e.m_Player.ToString() );
		}
		#endif
		return result;
	}
};

class HandGuardHasItemInEvent extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardHasItemInEvent(Man p = null) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		EntityAI eai = e.GetSrcEntity();
		if (eai != NULL /* && CanTakeInHands*/)
		{
			#ifdef ENABLE_LOGGING
			if ( LogManager.IsInventoryHFSMLogEnable() )
			{	
				Debug.InventoryHFSMLog("GuardCondition result: true - " + eai, "HandGuardHasItemInEvent" , "n/a", "GuardCondition", m_Player.ToString() );
			}
			#endif
			return true;
		}
		
		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: false - " + eai, "HandGuardHasItemInEvent" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif
		
		return false;
	}
};

class HandGuardHasWeaponInEvent extends HandGuardHasItemInEvent
{
	void HandGuardHasWeapoonInEvent (Man p = null) { }

	override bool GuardCondition(HandEventBase e)
	{
		EntityAI eai = e.GetSrcEntity();
		bool result = false;
		if (eai)
		{
			if (eai.IsWeapon())
			{
				result = true;
			}
		}
		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result,"HandGuardHasWeaponInEvent", "n/a", "GuardCondition", e.m_Player.ToString() );
		}
		#endif
		return result;
	}
};

class HandGuardIsSameItemInHands extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardIsSameItemInHands(Man p = null) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		bool result = false;
		if (e.GetSrcEntity() == m_Player.GetHumanInventory().GetEntityInHands())
		{
			result = true;
		}
		
		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result + " - srcItem = " + e.GetSrcEntity() + " hnd= " + m_Player.GetHumanInventory().GetEntityInHands(), "HandGuardIsSameItemInHands" , "n/a", "GuardCondition", e.m_Player.ToString() );
		}
		#endif
		return result;
	}
};

class HandGuardHasDestroyedItemInHands extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardHasDestroyedItemInHands(Man p = null) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		EntityAI hnd = m_Player.GetHumanInventory().GetEntityInHands();
		if (e.GetSrcEntity())
		{
			if (e.GetSrcEntity() == hnd)
			{
				#ifdef ENABLE_LOGGING
				if ( LogManager.IsInventoryHFSMLogEnable() )
				{	
					Debug.InventoryHFSMLog("GuardCondition result: true - has same entity in hands " + hnd, "HandGuardHasDestroyedItemInHands" , "n/a", "GuardCondition", m_Player.ToString() );
				}
				#endif
				return true;
			}

			if (hnd == null)
			{
				#ifdef ENABLE_LOGGING
				if ( LogManager.IsInventoryHFSMLogEnable() )
				{	
					Debug.InventoryHFSMLog("GuardCondition result: true - hands already empty", "HandGuardHasDestroyedItemInHands" , "n/a", "GuardCondition", m_Player.ToString() );
				}
				#endif
				return true;
			}
		}
		else
		{
			#ifdef ENABLE_LOGGING
			if ( LogManager.IsInventoryHFSMLogEnable() )
			{	
				Debug.InventoryHFSMLog("GuardCondition result: true - hands already empty and item destroyed", "HandGuardHasDestroyedItemInHands" , "n/a", "GuardCondition", m_Player.ToString() );
			}
			#endif
			return true;
		}
		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: false -  destroyed entity not in hands", "HandGuardHasDestroyedItemInHands" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif
		return false;
	}
};

class HandGuardHasItemInHands extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardHasItemInHands(Man p = null) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		bool result = false;
		if (m_Player.GetHumanInventory().GetEntityInHands())
		{
			result = true;
		}

		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result + " - " + m_Player.GetHumanInventory().GetEntityInHands(), "HandGuardHasItemInHands" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif
		return result;
	}
};

//! TODO(kumarjac): This guard is unused but it has a fault and doesn't conform with maximimal/minimal checks on "Juncture"/"Remote"
class HandGuardHasRoomForItem extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardHasRoomForItem(Man p = null) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		if (e.GetDst() && e.GetDst().IsValid())
		{
			if ( !GetGame().IsDedicatedServer())
			{
				if (m_Player)
					m_Player.GetHumanInventory().ClearInventoryReservationEx(e.GetDst().GetItem(),e.GetDst());
			}

			if (!GameInventory.LocationTestAddEntity(e.GetDst(), false, true, true, true, true, false))
			{
				#ifdef ENABLE_LOGGING
				if ( LogManager.IsInventoryHFSMLogEnable() )
				{	
					Debug.InventoryHFSMLog("GuardCondition result: false - no room at dst=" + InventoryLocation.DumpToStringNullSafe(e.GetDst()), "HandGuardHasRoomForItem" , "n/a", "GuardCondition", m_Player.ToString() );
				}
				#endif
				//if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("[hndfsm] HandGuardHasRoomForItem - no room at dst=" + InventoryLocation.DumpToStringNullSafe(e.GetDst()));
				//Error("[hndfsm] HandGuardHasRoomForItem - no room at dst=" + InventoryLocation.DumpToStringNullSafe(e.GetDst()));
				return false;
			}
				
			
			if ( !GetGame().IsDedicatedServer())
			{
				if (m_Player)
					m_Player.GetHumanInventory().AddInventoryReservationEx(e.GetDst().GetItem(), e.GetDst(), GameInventory.c_InventoryReservationTimeoutShortMS);
			}
			
			#ifdef ENABLE_LOGGING
			if ( LogManager.IsInventoryHFSMLogEnable() )
			{	
				Debug.InventoryHFSMLog("GuardCondition result: true", "HandGuardHasRoomForItem" , "n/a", "GuardCondition", m_Player.ToString() );
			}
			#endif
			return true;
		}
		
		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: false - e.m_Dst is null", "HandGuardHasRoomForItem" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif

		return false;
	}
};

class HandGuardCanMove extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardCanMove(Man p = null) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		HandEventMoveTo es = HandEventMoveTo.Cast(e);
		
		bool result = e.m_IsJuncture || e.m_IsRemote;
		if (result == false)
		{
			result = GameInventory.LocationCanMoveEntity(es.GetSrc(), es.GetDst());
		}

		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result, "HandGuardCanMove" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif
		
		return result;
	}
};

class HandGuardCanSwap extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardCanSwap(Man p = NULL) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		HandEventSwap es = HandEventSwap.Cast(e);

		bool result = e.m_IsJuncture || e.m_IsRemote;
		if (result == false)
		{
			result = GameInventory.CanSwapEntitiesEx(es.GetSrc().GetItem(), es.m_Src2.GetItem());
		}

		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result, "HandGuardCanSwap" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif
		//if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("[hndfsm] HandGuardCanSwap guard - cannot swap");
		return result;
	}
};

class HandGuardCanForceSwap extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardCanForceSwap(Man p = NULL) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		HandEventForceSwap es = HandEventForceSwap.Cast(e);

		bool result = e.m_IsJuncture || e.m_IsRemote;
		if (result == false)
		{
			result = GameInventory.CanSwapEntitiesEx(es.GetSrc().GetItem(), es.m_Src2.GetItem());

			if (result == false && es.m_Dst2)
			{
				result = GameInventory.CanForceSwapEntitiesEx(es.GetSrc().GetItem(), es.m_Dst, es.m_Src2.GetItem(), es.m_Dst2);
			}
		}
		
		#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result, "HandGuardCanForceSwap" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif

		return result;
	}
};

class HandGuardInstantForceSwap extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardInstantForceSwap(Man p = NULL) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		HandEventForceSwap es = HandEventForceSwap.Cast(e);
		
		InventoryLocation src1 = es.m_Src;
		InventoryLocation dst2 = es.m_Dst2;
		
		bool result = false;
		if (src1.GetType() == InventoryLocationType.CARGO && dst2.GetType() == InventoryLocationType.CARGO)
		{
			if (src1.GetParent() == dst2.GetParent())
			{
				result = true;
			}
		}

		#ifdef ENABLE_LOGGING
		if (LogManager.IsInventoryHFSMLogEnable())
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result, "HandGuardInstantForceSwap" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif

		return result;
	}
};

///@} guards

