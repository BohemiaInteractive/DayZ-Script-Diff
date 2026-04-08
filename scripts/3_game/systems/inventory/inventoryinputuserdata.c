
class InventoryInputUserData
{
	///@{ move
	static void SerializeMove(ParamsWriteContext ctx, int type, notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		ctx.Write(INPUT_UDT_INVENTORY);
		ctx.Write(type);
		src.WriteToContext(ctx);
		dst.WriteToContext(ctx);
	}

	static void SendInputUserDataMove(int type, notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		if (g_Game.IsClient())
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] t=" + g_Game.GetTime() + "ms sending cmd=" + typename.EnumToString(InventoryCommandType, type) + " src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));
			ScriptInputUserData ctx = new ScriptInputUserData;
			SerializeMove(ctx, type, src, dst);
			ctx.Send();
		}
	}
	
	static void SerializeInventoryCheckForItem(ParamsWriteContext ctx, EntityAI item)
	{
		InventoryLocation il = new InventoryLocation;
		item.GetInventory().GetCurrentInventoryLocation(il);
		il.WriteToContext(ctx);
	}
	
	static void SendClientInventoryCheck(array<EntityAI> items)
	{
		ScriptInputUserData ctx = new ScriptInputUserData;
		int nItems = items.Count();
		ctx.Write(INPUT_UDT_INVENTORY_CHECK);
		ctx.Write(nItems);
		for (int i = 0; i < nItems; ++i)
		{
			SerializeInventoryCheckForItem(ctx, items[i]);
		}
		ctx.Send();
	}
	
	static void SendServerInventoryCheck(array<EntityAI> items, DayZPlayer player)
	{
		ScriptRPC ctx = new ScriptRPC();
		int nItems = items.Count();
		ctx.Write(nItems);
		for (int i = 0; i < nItems; i++)
		{
			SerializeInventoryCheckForItem(ctx, items[i]);
		}

		ctx.Send(player, ERPCs.RPC_ITEM_REPAIR, false, player.GetIdentity());
	}

	static void SendServerMove(Man player, int type, notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		if (g_Game.IsServer())
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] server sending cmd=" + typename.EnumToString(InventoryCommandType, type) + " src=" + InventoryLocation.DumpToStringNullSafe(src) + " dst=" + InventoryLocation.DumpToStringNullSafe(dst));
			ScriptInputUserData ctx = new ScriptInputUserData;
			SerializeMove(ctx, type, src, dst);
			GameInventory.ServerLocationSyncMoveEntity(player, src.GetItem(), ctx);
		}
	}
	///@} move

	///@{ swap
	static void SerializeSwap(ParamsWriteContext ctx, notnull InventoryLocation src1, notnull InventoryLocation src2, notnull InventoryLocation dst1, notnull InventoryLocation dst2, bool skippedSwap)
	{
		ctx.Write(INPUT_UDT_INVENTORY);
		ctx.Write(InventoryCommandType.SWAP);
		src1.WriteToContext(ctx);
		src2.WriteToContext(ctx);
		dst1.WriteToContext(ctx);
		dst2.WriteToContext(ctx);
		ctx.Write(skippedSwap);
	}

	static void SendInputUserDataSwap(notnull InventoryLocation src1, notnull InventoryLocation src2, notnull InventoryLocation dst1, notnull InventoryLocation dst2, bool skippedSwap = false)
	{
		if (g_Game.IsClient())
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] t=" + g_Game.GetTime() + "ms sending cmd=SWAP src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));
			ScriptInputUserData ctx = new ScriptInputUserData;
			SerializeSwap(ctx, src1, src2, dst1, dst2, skippedSwap);
			ctx.Send();
		}
	}

	static void SendServerSwap(notnull InventoryLocation src1, notnull InventoryLocation src2, notnull InventoryLocation dst1, notnull InventoryLocation dst2, bool skippedSwap = false)
	{
		if (g_Game.IsServer())
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] server sending cmd=SWAP src1=" + InventoryLocation.DumpToStringNullSafe(src1) + " src2=" + InventoryLocation.DumpToStringNullSafe(src2) +  " dst1=" + InventoryLocation.DumpToStringNullSafe(dst1) + " dst2=" + InventoryLocation.DumpToStringNullSafe(dst2));
			ScriptInputUserData ctx = new ScriptInputUserData;
			SerializeSwap(ctx, src1, src2, dst1, dst2, skippedSwap);
			GameInventory.ServerLocationSwap(src1, src2, dst1, dst2, ctx);
		}
	}
	///@} swap

	///@{ hand
	static void SerializeHandEvent(ParamsWriteContext ctx, HandEventBase e)
	{
		ctx.Write(INPUT_UDT_INVENTORY);
		ctx.Write(InventoryCommandType.HAND_EVENT);
		e.WriteToContext(ctx);
	}

	static void SendInputUserDataHandEvent(HandEventBase e)
	{
		if (g_Game.IsClient())
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] t=" + g_Game.GetTime() + "ms SendInputUserDataHandEvent e=" + e.DumpToString());
			ScriptInputUserData ctx = new ScriptInputUserData;
			SerializeHandEvent(ctx, e);
			ctx.Send();
		}
	}

	static void SendServerHandEventViaInventoryCommand(notnull Man player, HandEventBase e)
	{
		// Warning: this uses NetworkMessageInventoryCommand
		if (g_Game.IsServer())
		{
			if (e.IsServerSideOnly())
				Error("[syncinv] SendServerHandEvent - called on server side event only, e=" + e.DumpToString());
			if (player.IsAlive())
				Error("[syncinv] SendServerHandEvent - called on living thing.. server hand command is only for dead people, e=" + e.DumpToString());
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] SendServerHandEventViaInventoryCommand SendInputUserDataHandEvent e=" + e.DumpToString());
			ScriptInputUserData ctx = new ScriptInputUserData;
			SerializeHandEvent(ctx, e);
			GameInventory.ServerHandEvent(player, e.GetSrcEntity(), ctx);
		}
	}
	///@} hand
}

