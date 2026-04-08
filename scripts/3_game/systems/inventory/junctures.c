
bool TryAcquireInventoryJunctureFromServer (notnull Man player, notnull InventoryLocation src, notnull InventoryLocation dst)
{
	if (player.NeedInventoryJunctureFromServer(src.GetItem(), src.GetParent(), dst.GetParent()))
	{
		if ( ( src.GetItem() && src.GetItem().IsSetForDeletion() ) || ( src.GetParent() && src.GetParent().IsSetForDeletion() ) || ( dst.GetParent() && dst.GetParent().IsSetForDeletion() ) )
		{
			return JunctureRequestResult.JUNCTURE_DENIED;
		}
		
		if (src.GetItem() && !src.GetItem().CanPutIntoHands(player))
		{
			return JunctureRequestResult.JUNCTURE_DENIED;
		}
		
		bool test_dst_occupancy = true;
		if (g_Game.AddInventoryJunctureEx(player, src.GetItem(), dst, test_dst_occupancy, GameInventory.c_InventoryReservationTimeoutMS))
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] juncture needed and acquired, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));
			return JunctureRequestResult.JUNCTURE_ACQUIRED; // ok
		}
		else
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] juncture request DENIED, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));
			return JunctureRequestResult.JUNCTURE_DENIED; // permission to perform juncture denied
		}
	}
	else
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] juncture not required, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));
		return JunctureRequestResult.JUNCTURE_NOT_REQUIRED; // juncture not necessary
	}
}

bool TryAcquireTwoInventoryJuncturesFromServer (notnull Man player, notnull InventoryLocation src1, notnull InventoryLocation src2, notnull InventoryLocation dst1, notnull InventoryLocation dst2)
{
	#ifdef ENABLE_LOGGING
	if ( LogManager.IsInventoryReservationLogEnable() )
	{
		Debug.InventoryReservationLog("STS = " + player.GetSimulationTimeStamp() + " src1:" + src1.DumpToString() + " dst1: " + dst1.DumpToString()+ " src2:" + src2.DumpToString() + " dst2: " + dst2.DumpToString(), "InventoryJuncture" , "n/a", "TryAcquireTwoInventoryJuncturesFromServer",player.ToString() );	
	}
	#endif
	
	//if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] t=" + g_Game.GetTime() + "ms TryAcquireTwoInventoryJuncturesFromServer src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));

	EntityAI srcItem1 = src1.GetItem();
	EntityAI srcItem2 = src2.GetItem();
	EntityAI srcParent1 = src1.GetParent();
	EntityAI srcParent2 = src2.GetParent();
	EntityAI dstParent1 = dst1.GetParent();
	EntityAI dstParent2 = dst2.GetParent();
	
	
	bool need_j1 = player.NeedInventoryJunctureFromServer(srcItem1, srcParent1, dstParent1);
	bool need_j2 = player.NeedInventoryJunctureFromServer(srcItem2, srcParent2, dstParent2);
	if (need_j1 || need_j2)
	{
		if (need_j1)
		{
			if ( ( srcItem1 && srcItem1.IsSetForDeletion() ) || ( srcParent1 && srcParent1.IsSetForDeletion() ) || ( dstParent1 && dstParent1.IsSetForDeletion() ) )
			{
				return JunctureRequestResult.JUNCTURE_DENIED;
			}
			
			
			if (srcItem1 && !srcItem1.CanPutIntoHands(player))
			{
				return JunctureRequestResult.JUNCTURE_DENIED;
			}
			
			if (!g_Game.AddInventoryJunctureEx(player, srcItem1, dst1, false, GameInventory.c_InventoryReservationTimeoutMS))
			{
				/*#ifdef ENABLE_LOGGING
				if ( LogManager.IsInventoryReservationLogEnable() )
				{
					Debug.InventoryMoveLog("", "SWAP" , "n/a", "TryAcquireTwoInventoryJuncturesFromServer", player.ToString() );
				}
				#endif*/
				return JunctureRequestResult.JUNCTURE_DENIED; // permission to perform juncture denied
			}
		}
//Need add log and change chanel to print
		if (need_j2)
		{
			if ( ( srcItem2 && srcItem2.IsSetForDeletion() ) || ( srcParent2 && srcParent2.IsSetForDeletion() ) || ( dstParent2 && dstParent2.IsSetForDeletion() ) )
			{
				if (need_j1)
				{
					g_Game.ClearJunctureEx(player, srcItem1); // release already acquired juncture for item1
				}
				return JunctureRequestResult.JUNCTURE_DENIED;
			}
			
			if (srcItem2 && !srcItem2.CanPutIntoHands(player))
			{
				return JunctureRequestResult.JUNCTURE_DENIED;
			}
			
			if (!g_Game.AddInventoryJunctureEx(player, srcItem2, dst2, false, GameInventory.c_InventoryReservationTimeoutMS))
			{
				//if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] item2 juncture request DENIED, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));
				if (need_j1)
				{
					g_Game.ClearJunctureEx(player, srcItem1); // release already acquired juncture for item1
					//if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] item2 juncture request DENIED, cleaning acquired juncture for item1, , player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));
				}
				return JunctureRequestResult.JUNCTURE_DENIED; // permission to perform juncture denied
			}
		}

		return JunctureRequestResult.JUNCTURE_ACQUIRED; // ok, both junctures acquired
	}
	else
	{
		/*#ifdef ENABLE_LOGGING
		if ( LogManager.IsInventoryReservationLogEnable() )
		{
			Debug.InventoryReservationLog("Remote - skipped", "SWAP" , "n/a", "ProcessInputData", player.ToString() );
		}
		#endif*/
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] junctures not required, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));
		return JunctureRequestResult.JUNCTURE_NOT_REQUIRED; // juncture not necessary
	}
}

