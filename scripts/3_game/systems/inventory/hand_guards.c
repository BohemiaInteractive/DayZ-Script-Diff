///@{ guards

/**@class	HandGuardBase
 * @brief	represents guard on a transition from state to state
 **/
class HandGuardBase
{
	/**@fn			GuardCondition
	 * @brief		enable or disable transition based on condition
	 * the guard is a boolean operation executed first and which can prevent the transition from firing by returning false
	 * @return		true if transition is allowed
	 **/
	bool GuardCondition (HandEventBase e) { return true; }
};


class HandGuardAnd extends HandGuardBase
{
	ref HandGuardBase m_arg0;
	ref HandGuardBase m_arg1;

	void HandGuardAnd (HandGuardBase arg0 = NULL, HandGuardBase arg1 = NULL) { m_arg0 = arg0; m_arg1 = arg1; }

	override bool GuardCondition (HandEventBase e)
	{
		bool result = m_arg0.GuardCondition(e) && m_arg1.GuardCondition(e);
		
		#ifdef DEVELOPER
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

	void HandGuardNot (HandGuardBase arg0 = NULL) { m_arg0 = arg0; }

	override bool GuardCondition (HandEventBase e)
	{
		bool result = !m_arg0.GuardCondition(e);
		
		#ifdef DEVELOPER
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

	void HandGuardOr (HandGuardBase arg0 = NULL, HandGuardBase arg1 = NULL) { m_arg0 = arg0; m_arg1 = arg1; }

	override bool GuardCondition (HandEventBase e)
	{
		bool result = m_arg0.GuardCondition(e) || m_arg1.GuardCondition(e);
		
		#ifdef DEVELOPER
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
	void HandGuardHasItemInEvent (Man p = NULL) { m_Player = p; }

	override bool GuardCondition (HandEventBase e)
	{
		EntityAI eai = e.GetSrcEntity();
		if (eai != NULL /* && CanTakeInHands*/)
		{
			#ifdef DEVELOPER
			if ( LogManager.IsInventoryHFSMLogEnable() )
			{	
				Debug.InventoryHFSMLog("GuardCondition result: true - " + eai, "HandGuardHasItemInEvent" , "n/a", "GuardCondition", m_Player.ToString() );
			}
			#endif
			return true;
		}
		
		#ifdef DEVELOPER
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
	void HandGuardHasWeapoonInEvent (Man p = NULL) { }

	override bool GuardCondition (HandEventBase e)
	{
		EntityAI eai = e.GetSrcEntity();
		bool result = false;
		if (eai != NULL)
		{
			if (eai.IsWeapon())
			{
				result = true;
			}
		}
		#ifdef DEVELOPER
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
	void HandGuardIsSameItemInHands (Man p = NULL) { m_Player = p; }

	override bool GuardCondition (HandEventBase e)
	{
		bool result = false;
		if (e.GetSrcEntity() == m_Player.GetHumanInventory().GetEntityInHands())
		{
			result = true;
		}
		
		#ifdef DEVELOPER
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
	void HandGuardHasDestroyedItemInHands (Man p = NULL) { m_Player = p; }

	override bool GuardCondition (HandEventBase e)
	{
		EntityAI hnd = m_Player.GetHumanInventory().GetEntityInHands();
		if (e.GetSrcEntity())
		{
			if (e.GetSrcEntity() == hnd)
			{
				#ifdef DEVELOPER
				if ( LogManager.IsInventoryHFSMLogEnable() )
				{	
					Debug.InventoryHFSMLog("GuardCondition result: true - has same entity in hands " + hnd, "HandGuardHasDestroyedItemInHands" , "n/a", "GuardCondition", m_Player.ToString() );
				}
				#endif
				return true;
			}

			if (hnd == null)
			{
				#ifdef DEVELOPER
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
			#ifdef DEVELOPER
			if ( LogManager.IsInventoryHFSMLogEnable() )
			{	
				Debug.InventoryHFSMLog("GuardCondition result: true - hands already empty and item destroyed", "HandGuardHasDestroyedItemInHands" , "n/a", "GuardCondition", m_Player.ToString() );
			}
			#endif
			return true;
		}
		#ifdef DEVELOPER
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
	void HandGuardHasItemInHands (Man p = NULL) { m_Player = p; }

	override bool GuardCondition (HandEventBase e)
	{
		bool result = false;
		if (m_Player.GetHumanInventory().GetEntityInHands())
		{
			result = true;
		}

		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result + " - " + m_Player.GetHumanInventory().GetEntityInHands(), "HandGuardHasItemInHands" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif
		return result;
	}
};

class HandGuardHasRoomForItem extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardHasRoomForItem (Man p = NULL) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		if (e.GetDst() && e.GetDst().IsValid())
		{
			if( !GetGame().IsDedicatedServer())
			{
				if(m_Player)
					m_Player.GetHumanInventory().ClearInventoryReservationEx(e.GetDst().GetItem(),e.GetDst());
			}

			if (!GameInventory.LocationTestAddEntity(e.GetDst(), false, true, true, true, true, false))
			{
				#ifdef DEVELOPER
				if ( LogManager.IsInventoryHFSMLogEnable() )
				{	
					Debug.InventoryHFSMLog("GuardCondition result: false - no room at dst=" + InventoryLocation.DumpToStringNullSafe(e.GetDst()), "HandGuardHasRoomForItem" , "n/a", "GuardCondition", m_Player.ToString() );
				}
				#endif
				//hndDebugPrint("[hndfsm] HandGuardHasRoomForItem - no room at dst=" + InventoryLocation.DumpToStringNullSafe(e.GetDst()));
				//Error("[hndfsm] HandGuardHasRoomForItem - no room at dst=" + InventoryLocation.DumpToStringNullSafe(e.GetDst()));
				return false;
			}
				
			
			if( !GetGame().IsDedicatedServer())
			{
				if(m_Player)
					m_Player.GetHumanInventory().AddInventoryReservationEx(e.GetDst().GetItem(), e.GetDst(), GameInventory.c_InventoryReservationTimeoutShortMS);
			}
			
			#ifdef DEVELOPER
			if ( LogManager.IsInventoryHFSMLogEnable() )
			{	
				Debug.InventoryHFSMLog("GuardCondition result: true", "HandGuardHasRoomForItem" , "n/a", "GuardCondition", m_Player.ToString() );
			}
			#endif
			return true;
		}
		
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: false - e.m_Dst is null", "HandGuardHasRoomForItem" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif

		return false;
	}
};

class HandGuardCanSwap extends HandGuardBase
{
	protected Man m_Player;
	void HandGuardCanSwap(Man p = NULL) { m_Player = p; }

	override bool GuardCondition(HandEventBase e)
	{
		HandEventSwap es = HandEventSwap.Cast(e);
		bool result = GameInventory.CanSwapEntitiesEx(es.GetSrc().GetItem(), es.m_Src2.GetItem());

		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result, "HandGuardCanSwap" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif
		//hndDebugPrint("[hndfsm] HandGuardCanSwap guard - cannot swap");
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
		
		bool result = GameInventory.CanSwapEntitiesEx(es.GetSrc().GetItem(), es.m_Src2.GetItem());
		if (!result && es.m_Dst2)
		 	result = GameInventory.CanForceSwapEntitiesEx(es.GetSrc().GetItem(), null, es.m_Src2.GetItem(), es.m_Dst2);
		
		#ifdef DEVELOPER
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

		#ifdef DEVELOPER
		if (LogManager.IsInventoryHFSMLogEnable())
		{	
			Debug.InventoryHFSMLog("GuardCondition result: " + result, "HandGuardInstantForceSwap" , "n/a", "GuardCondition", m_Player.ToString() );
		}
		#endif

		return result;
	}
};

///@} guards

