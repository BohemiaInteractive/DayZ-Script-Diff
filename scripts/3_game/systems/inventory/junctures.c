
bool TryAcquireInventoryJunctureFromServer (notnull Man player, notnull InventoryLocation src, notnull InventoryLocation dst)
{
	if (player.NeedInventoryJunctureFromServer(src.GetItem(), src.GetParent(), dst.GetParent()))
	{
		if ( ( src.GetItem() && src.GetItem().IsSetForDeletion() ) || ( src.GetParent() && src.GetParent().IsSetForDeletion() ) || ( dst.GetParent() && dst.GetParent().IsSetForDeletion() ) )
		{
			return JunctureRequestResult.JUNCTURE_DENIED;
		}
		bool test_dst_occupancy = true;
		if (GetGame().AddInventoryJunctureEx(player, src.GetItem(), dst, test_dst_occupancy, GameInventory.c_InventoryReservationTimeoutMS))
		{
			syncDebugPrint("[syncinv] juncture needed and acquired, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));
			return JunctureRequestResult.JUNCTURE_ACQUIRED; // ok
		}
		else
		{
			syncDebugPrint("[syncinv] juncture request DENIED, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));
			return JunctureRequestResult.JUNCTURE_DENIED; // permission to perform juncture denied
		}
	}
	else
	{
		syncDebugPrint("[syncinv] juncture not required, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));
		return JunctureRequestResult.JUNCTURE_NOT_REQUIRED; // juncture not necessary
	}
}

bool TryAcquireTwoInventoryJuncturesFromServer (notnull Man player, notnull InventoryLocation src1, notnull InventoryLocation src2, notnull InventoryLocation dst1, notnull InventoryLocation dst2)
{
	#ifdef DEVELOPER
	if ( LogManager.IsInventoryReservationLogEnable() )
	{
		Debug.InventoryReservationLog("STS = " + player.GetSimulationTimeStamp() + " src1:" + src1.DumpToString() + " dst1: " + dst1.DumpToString()+ " src2:" + src2.DumpToString() + " dst2: " + dst2.DumpToString(), "InventoryJuncture" , "n/a", "TryAcquireTwoInventoryJuncturesFromServer",player.ToString() );	
	}
	#endif
	
	//syncDebugPrint("[syncinv] t=" + GetGame().GetTime() + "ms TryAcquireTwoInventoryJuncturesFromServer src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));

	bool need_j1 = player.NeedInventoryJunctureFromServer(src1.GetItem(), src1.GetParent(), dst1.GetParent());
	bool need_j2 = player.NeedInventoryJunctureFromServer(src2.GetItem(), src2.GetParent(), dst2.GetParent());
	if (need_j1 || need_j2)
	{
		if (need_j1)
		{
			if ( ( src1.GetItem() && src1.GetItem().IsSetForDeletion() ) || ( src1.GetParent() && src1.GetParent().IsSetForDeletion() ) || ( dst1.GetParent() && dst1.GetParent().IsSetForDeletion() ) )
			{
				return JunctureRequestResult.JUNCTURE_DENIED;
			}
			if (!GetGame().AddInventoryJunctureEx(player, src1.GetItem(), dst1, false, GameInventory.c_InventoryReservationTimeoutMS))
			{
				/*#ifdef DEVELOPER
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
			if ( ( src2.GetItem() && src2.GetItem().IsSetForDeletion() ) || ( src2.GetParent() && src2.GetParent().IsSetForDeletion() ) || ( dst2.GetParent() && dst2.GetParent().IsSetForDeletion() ) )
			{
				if (need_j1)
				{
					GetGame().ClearJunctureEx(player, src1.GetItem()); // release already acquired juncture for item1
				}
				return JunctureRequestResult.JUNCTURE_DENIED;
			}
			if (!GetGame().AddInventoryJunctureEx(player, src2.GetItem(), dst2, false, GameInventory.c_InventoryReservationTimeoutMS))
			{
				//syncDebugPrint("[syncinv] item2 juncture request DENIED, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));
				if (need_j1)
				{
					GetGame().ClearJunctureEx(player, src1.GetItem()); // release already acquired juncture for item1
					//syncDebugPrint("[syncinv] item2 juncture request DENIED, cleaning acquired juncture for item1, , player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));
				}
				return JunctureRequestResult.JUNCTURE_DENIED; // permission to perform juncture denied
			}
		}

		return JunctureRequestResult.JUNCTURE_ACQUIRED; // ok, both junctures acquired
	}
	else
	{
		#ifdef DEVELOPER
		if ( LogManager.IsInventoryReservationLogEnable() )
		{
			Debug.InventoryMoveLog("Remote - skipped", "SWAP" , "n/a", "ProccessInputData", player.ToString() );
		}
		#endif
		syncDebugPrint("[syncinv] junctures not required, player=" + Object.GetDebugName(player) + " STS = " + player.GetSimulationTimeStamp() + " src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));
		return JunctureRequestResult.JUNCTURE_NOT_REQUIRED; // juncture not necessary
	}
}

