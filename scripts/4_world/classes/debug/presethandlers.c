#ifdef DEVELOPER
//----------------------------------------------------------
//Class names need to match the preset name + _Preset suffix
//----------------------------------------------------------


//TEMPLATE:
/*

class Farmer_Preset extends PresetSpawnBase
{
	override void OnPresetSpawn(PlayerBase player)
	{
		//m_ItemsInventory -> contains all items in player inventory
	}
}

*/

//---------------------------------------------
class FreshSpawn_Preset extends PresetSpawnBase
{
	override void OnPresetSpawn(PlayerBase player)
	{
		FindAndTakeToHandsFromInventory("Plum");
	}
}

//---------------------------------------------
class Farmer_Preset extends PresetSpawnBase
{
	override void OnPresetSpawn(PlayerBase player)
	{
		EntityAI mask = FindItem("AirborneMask");
	}
}

//---------------------------------------------
class Fisherman_Preset extends PresetSpawnBase
{
	override void OnPresetSpawn(PlayerBase player)
	{
		EntityAI entAI;

		entAI = FindItem("FishingRod");
		player.SetQuickBarEntityShortcut(entAI,0);

		entAI = FindItem("ImprovisedFishingRod");
		player.SetQuickBarEntityShortcut(entAI,1);

		entAI = FindItem("Rag");
		player.SetQuickBarEntityShortcut(entAI,2);

		entAI = FindItem("FieldShovel");
		player.SetQuickBarEntityShortcut(entAI,3);
		
		entAI = FindItem("HuntingKnife");
		player.SetQuickBarEntityShortcut(entAI,4);
	}
}

//---------------------------------------------
class AntiHazard_Preset extends PresetSpawnBase
{
	override void OnPresetSpawn(PlayerBase player)
	{
		EntityAI mask = FindItem("AirborneMask");
		
		if (mask)
		{
			auto debugParams = DebugSpawnParams.WithPlayer(player);
			mask.OnDebugSpawnEx(debugParams);
		}
	}
}

//---------------------------------------------
class Update117_Preset extends PresetSpawnBase
{	
	vector randompos;
	override void OnPresetSpawn(PlayerBase player)
	{
		randompos = player.GetPosition();
		foreach (EntityAI itm : m_ItemsPreset)
		{
			if (itm)
			{
				vector itempos;
				itempos[0] = Math.RandomFloat(-1,1);
				itempos[2] = Math.RandomFloat(-1,1);
				
				ItemBase itmBase = ItemBase.Cast(itm);
				
				itmBase.SetPosition(randompos + itempos);
				itmBase.SetQuantity(100);
			}
		}
	}
}

//---------------------------------------------
class Update118_Preset extends PresetSpawnBase
{	
	vector randompos;
	override void OnPresetSpawn(PlayerBase player)
	{
		randompos = player.GetPosition();
		foreach (EntityAI itm : m_ItemsPreset)
		{
			if (itm)
			{
				vector itempos;
				itempos[0] = Math.RandomFloat(-1,1);
				itempos[2] = Math.RandomFloat(-1,1);
				
				ItemBase itmBase = ItemBase.Cast(itm);
				
				itmBase.SetPosition(randompos + itempos);
				itmBase.SetQuantity(100);
			}
		}
	}
}

//---------------------------------------------
class BBMaterials_Preset extends PresetSpawnBase
{	
	vector randompos;
	override void OnPresetSpawn(PlayerBase player)
	{
		randompos = player.GetPosition();
		foreach (EntityAI itm : m_ItemsPreset)
		{
			if (itm)
			{
				vector itempos;
				itempos[0] = Math.RandomFloat(-1,1);
				itempos[2] = Math.RandomFloat(-1,1);
				itm.SetPosition(randompos + itempos);
			}
		}
	}
}

//---------------------------------------------
class Military_Light_Preset extends PresetSpawnBase
{
	override void OnPresetSpawn(PlayerBase player)
	{
		EntityAI weapon = FindItem("AK101");

		auto debugParams = DebugSpawnParams.WithPlayer(player);
		weapon.OnDebugSpawnEx(debugParams);

		FindAndTakeToHandsFromInventory("AK101");
	}
}




//---------------------------------------------
//------------------ BASE ---------------------
//---------------------------------------------

class PresetSpawnBase
{
	ref array<EntityAI> m_ItemsInventory = new array<EntityAI>;//inventory items
	HumanInventory m_Inventory;
	PlayerBase m_Player;
	array<EntityAI> m_ItemsPreset;//all items spawned from this preset
	//---------------------------------------------
	
	void Init(PlayerBase player)
	{
		m_Player = player;
		m_Inventory = player.GetHumanInventory();
		m_ItemsPreset = player.m_PresetItems;
		m_Inventory.EnumerateInventory(InventoryTraversalType.INORDER, m_ItemsInventory);
		OnPresetSpawn(player);

		foreach (auto item:m_ItemsPreset)
		{
			Print(item);
		}
	}
	
	//---------------------------------------------
	
	void OnPresetSpawn(PlayerBase player)
	{

	}
	//---------------------------------------------
	
	void TakeToHands(EntityAI item)
	{
		if (GetGame().IsMultiplayer() && m_Player.GetInstanceType() != DayZPlayerInstanceType.INSTANCETYPE_SERVER)//throws assert otherwise
			return;
		if (GetGame().IsDedicatedServer())
		{
			m_Player.TakeEntityToHandsImpl(InventoryMode.SERVER, item);
		}
		else
		{
			m_Player.TakeEntityToHandsImpl(InventoryMode.PREDICTIVE, item);
		}
		
	}
	
	void FindAndTakeToHandsFromInventory(string classname)
	{
		m_Inventory.EnumerateInventory(InventoryTraversalType.INORDER, m_ItemsInventory);
		foreach(EntityAI item:m_ItemsInventory)
		{
			if (item.ClassName() == classname)
			{
				TakeToHands(item);
				return;
			}
		}
	}
	
	EntityAI FindItem(string classname)
	{
		m_Inventory.EnumerateInventory(InventoryTraversalType.INORDER, m_ItemsInventory);
		foreach(EntityAI item:m_ItemsInventory)
		{
			if (item.ClassName() == classname)
			{
				return item;
			}
		}
		return null;
	}
	
}

#endif