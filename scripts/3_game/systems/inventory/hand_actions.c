///@{ actions

/**@class	HandActionBase
 * @brief	represents action executed on transition just between OnExit from old state and OnEntry to new state
 **/
class HandActionBase
{
	/**@fn			Action
	 * @brief		executed when transition occurs
	 **/
	void Action (HandEventBase e) { }
};


class HandActionCreated extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionCreated", e.m_Player.ToString() );
		}
		#endif

		e.m_Player.OnItemInHandsChanged();
	}
};

class HandActionTake extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionTake", e.m_Player.ToString() );
		}
		#endif

		GameInventory.LocationSyncMoveEntity(e.GetSrc(), e.GetDst());
		e.m_Player.OnItemInHandsChanged();
	}
};

class HandActionDrop extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionDrop", e.m_Player.ToString() );
		}
		#endif

		GameInventory.LocationSyncMoveEntity(e.GetSrc(), e.GetDst());
 		e.m_Player.OnItemInHandsChanged();
	}
};

class HandActionThrow extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionThrow", e.m_Player.ToString() );
		}
		#endif
		HandEventThrow throwEvent = HandEventThrow.Cast(e);

		GameInventory.LocationSyncMoveEntity(e.GetSrc(), e.GetDst());

		DayZPlayer player = DayZPlayer.Cast(e.m_Player);
		if ( player.GetInstanceType() != DayZPlayerInstanceType.INSTANCETYPE_REMOTE )
		{
			InventoryItem item = InventoryItem.Cast(e.GetSrcEntity());
			if( item )
				item.ThrowPhysically(player, throwEvent.GetForce());
			else
				Error("[hndfsm] HandActionThrow - src entity null!");
		}

		player.OnItemInHandsChanged();
	}
};

class HandActionMoveTo extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionMoveTo", e.m_Player.ToString() );
		}
		#endif

		HandEventMoveTo es = HandEventMoveTo.Cast(e);
		if (es)
		{
			GameInventory.LocationSyncMoveEntity(e.GetSrc(), es.m_Dst);
			e.m_Player.OnItemInHandsChanged();
		}
		else
			Error("[hndfsm] HandActionMoveTo - this is no HandEventMoveTo");
	}
};

class HandActionDestroy extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionDestroy", e.m_Player.ToString() );
		}
		#endif

		GetGame().ObjectDelete(e.GetSrcEntity());
		e.m_Player.OnItemInHandsChanged();
	}
};

class HandActionDestroyed extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionDestroyed", e.m_Player.ToString() );
		}
		#endif
		e.m_Player.OnItemInHandsChanged();
	}
};

class HandActionDestroyAndReplaceWithNew extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionDestroyAndReplaceWithNew", e.m_Player.ToString() );
		}
		#endif
		Man player = e.m_Player;
		EntityAI itemInHands = player.GetHumanInventory().GetEntityInHands();

		InventoryLocation src = new InventoryLocation;
		if (itemInHands.GetInventory().GetCurrentInventoryLocation(src))
		{
			HandEventDestroyAndReplaceWithNew edr = HandEventDestroyAndReplaceWithNew.Cast(e);
			if (edr)
			{
				edr.m_Lambda.Execute();
				return;
			}
			else
				Error("[hndfsm] HandActionDestroyAndReplaceWithNew - not a HandEventDestroyAndReplaceWithNew event");
		}
		else
			Error("[hndfsm] HandActionDestroyAndReplaceWithNew - itemInHands has no InventoryLocation");
	}
};

class HandActionDestroyAndReplaceWithNewElsewhere extends HandActionDestroyAndReplaceWithNew
{
	override void Action (HandEventBase e)
	{
		super.Action(e);
	}
};

class HandActionReplaced extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionReplaced", e.m_Player.ToString() );
		}
		#endif
		Man player = e.m_Player;

		player.OnItemInHandsChanged();
	}
};

class HandActionSwap extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionSwap", e.m_Player.ToString() );
		}
		#endif
		HandEventSwap es = HandEventSwap.Cast(e);
		if (es)
		{
			GameInventory.LocationSwap(es.GetSrc(), es.m_Src2, es.GetDst(), es.m_Dst2);
			e.m_Player.OnItemInHandsChanged();
		}
		else
			Error("[hndfsm] HandActionSwap - this is no HandEventSwap");
	}
};

class HandActionForceSwap extends HandActionBase
{
	override void Action (HandEventBase e)
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryHFSMLogEnable() )
		{	
			Debug.InventoryHFSMLog("Action - STS = " + e.m_Player.GetSimulationTimeStamp(), e.ToString() , "n/a", "HandActionForceSwap", e.m_Player.ToString() );
		}
		#endif
		HandEventForceSwap es = HandEventForceSwap.Cast(e);
		if (es)
		{
			GameInventory.LocationSwap(es.GetSrc(), es.m_Src2, es.GetDst(), es.m_Dst2);
			e.m_Player.OnItemInHandsChanged();
		}
		else
			Error("[hndfsm] HandActionForceSwap - this is no HandEventForceSwap");
	}
};

///@} actions

