// *************************************************************************************
// ! PluginInventoryRepair
// *************************************************************************************
class PluginInventoryRepair extends PluginBase
{
	protected static float PIR_REQUEST_COOLDOWN = 5;
	protected static float PIR_MAX_ITEMS_TO_SEND = 5;
	
	protected ref array<EntityAI> m_ItemsToRepair;
	protected ref array<ref InventoryLocation> m_CorrectILs;
	protected float m_TimeFromLastRequest = 0;
	
	void PluginInventoryRepair()
	{
		m_ItemsToRepair = new array<EntityAI>();
		m_CorrectILs = new array<ref InventoryLocation>();
	}

	bool RepairItem(EntityAI itemToRepair)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		InventoryLocation repairIl = new InventoryLocation;
		InventoryLocation currentIl = new InventoryLocation;
		
		repairIl.SetTemporary(player, itemToRepair);
		itemToRepair.GetInventory().GetCurrentInventoryLocation(currentIl);
		
		if(GameInventory.LocationSyncMoveEntity(currentIl, repairIl))
		{
			if(currentIl.GetType() == InventoryLocationType.HANDS)
			{
				player.OnItemInHandsChanged();
			}
			return true;
		}
		return false;
	}
	
	//Receive from server
	void OnRPC(ParamsReadContext ctx)
	{
		int itemCount;
		
		if (!ctx.Read(itemCount))
			return;
		
		for (int i = 0; i < itemCount; i++)
		{
			InventoryLocation il = new InventoryLocation();
			if (!il.ReadFromContext(ctx) )
				return;
			
			m_CorrectILs.Insert(il);
		}
	}

	bool MoveItemToCorrectPosition(InventoryLocation ilCorrect)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		InventoryLocation ilTemp = new InventoryLocation;
			
		EntityAI item = ilCorrect.GetItem();
		EntityAI parent = ilCorrect.GetParent();
		item.GetInventory().GetCurrentInventoryLocation(ilTemp);
			
		switch(ilCorrect.GetType())
		{
			case InventoryLocationType.CARGO:
				InventoryLocation ilCargoItem = new InventoryLocation;
				CargoBase cargo = parent.GetInventory().GetCargo();
				for(int i = cargo.GetItemCount() - 1; i >= 0 ; --i)
				{
					EntityAI itemInCargo = cargo.GetItem(i);
					if (itemInCargo != item)
					{
						itemInCargo.GetInventory().GetCurrentInventoryLocation(ilCargoItem);
						if (ilCorrect.CollidesWith(ilCargoItem))
						{
							if (!RepairItem(itemInCargo))
							{
								return false;
							}
						}
					}
				}
				break;
			case InventoryLocationType.ATTACHMENT:
				EntityAI entityInAttachment = parent.GetInventory().FindAttachment(ilCorrect.GetSlot());

				if(entityInAttachment)
				{
					if (!RepairItem(entityInAttachment))
					{
						return false;
					}
				}
			
				break;
			case InventoryLocationType.HANDS:
				EntityAI entityInHands = player.GetItemInHands();
				if(entityInHands)
				{
					if (!RepairItem(entityInHands))
					{
						return false;
					}
				}

				break;
			default:
				break;
		}

		if (!GameInventory.LocationSyncMoveEntity(ilTemp, ilCorrect))
		{
			if (ilTemp.GetType() != InventoryLocationType.TEMP)
				RepairItem(item);
			return false;
		}
		
		return true;
	}

	override void OnUpdate(float delta_time)
	{
		if (m_CorrectILs.Count())
		{
			for (int j = m_CorrectILs.Count() - 1; j >= 0 ; j--)
			{
				if (MoveItemToCorrectPosition(m_CorrectILs[j]))
					m_CorrectILs.Remove(j);
			}
		}
	
		m_TimeFromLastRequest += delta_time;
		if(m_ItemsToRepair.Count())
		{
			array<EntityAI> itemsToSend = new array<EntityAI>;
			if (m_TimeFromLastRequest > PIR_REQUEST_COOLDOWN)
			{
				for (int i = 0; i < m_ItemsToRepair.Count() && i < PIR_MAX_ITEMS_TO_SEND; i++ )
				{
					itemsToSend.Insert(m_ItemsToRepair[i]);
				}

				InventoryInputUserData.SendClientInventoryCheck(itemsToSend);
				m_TimeFromLastRequest = 0;
			}
		}
	}
	
	void Remove(EntityAI item)
	{
		m_ItemsToRepair.RemoveItem(item);
	}
	
	void Add(EntityAI item)
	{
		if (m_ItemsToRepair.Find(item) < 0)
		{
			m_ItemsToRepair.Insert(item);
		}
	}
}