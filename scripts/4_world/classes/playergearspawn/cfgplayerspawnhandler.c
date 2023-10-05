class PlayerSpawnHandler
{
	private static string m_Path = "$mission:cfgPlayerSpawnGear.json";
	private static bool m_Initialized = false;
	static ref PlayerSpawnJsonData m_Data;
	
	static bool LoadData()
	{
		if (!FileExist(m_Path))
		{
			return false;
		}

		bool cfgPlayerSpawnGearFileUsed = CfgGameplayHandler.GetUsePlayerSpawnGearFile();

#ifdef DIAG_DEVELOPER
		if (!GetGame().IsDedicatedServer())
		{
			cfgPlayerSpawnGearFileUsed = true;
		}
#endif
		if (cfgPlayerSpawnGearFileUsed)
		{
			JsonFileLoader<PlayerSpawnJsonData>.JsonLoadFile(m_Path, m_Data);
			m_Initialized = true;
		}
		
		return cfgPlayerSpawnGearFileUsed;
	}
	
	static bool IsInitialized()
	{
		return m_Initialized;
	}
	
	static PlayerSpawnPreset GetRandomCharacterPreset()
	{
		array<int> weightedPresetIndexes = new array<int>();
		int count = m_Data.presets.Count();
		PlayerSpawnPreset p;
		for (int i = 0; i < count; i++)
		{
			p = m_Data.presets[i];
			if (p.IsValid())
			{
				for (int j = 0; j < p.spawnWeight; j++)
				{
					weightedPresetIndexes.Insert(i);
				}
			}
		}
		
		return m_Data.presets.Get(weightedPresetIndexes.GetRandomElement());
	} 
	
	//! equips character with the chosen preset
	static bool ProcessEquipmentData(PlayerBase player, PlayerSpawnPreset data)
	{
		if (data.IsValid())
		{
			ProcessSlotsEquipment(player, data);
			ProcessCargoEquipment(player, data);
		}
		
		return true;
	}
	
	//! iterates over each object and spawns alternatives
	private static void ProcessSlotsEquipment(PlayerBase player, PlayerSpawnPreset data)
	{
		if (!data.HasAttachmentSlotSetsDefined())
		{
			Debug.Log("No non-empty 'attachmentSlotItemSets' array found. Skipping slot spawns","n/a","n/a","ProcessSlotsEquipment");
			return;
		}
		
		foreach (PlayerSpawnPresetSlotData slotData : data.attachmentSlotItemSets)
		{
			SelectAndSpawnSlotEquipment(player,slotData);
		}
	}
	
	//! selects weighted slot equipment variant
	private static bool SelectAndSpawnSlotEquipment(PlayerBase player, PlayerSpawnPresetSlotData slotData)
	{
		int slotID;
		if (!slotData.TranslateAndValidateSlot(player,slotID))
			return false;
		
		if (!slotData.IsValid())
			return false;
		
		array<int> weightedDiscreteSetIndexes = new array<int>();
		int count = slotData.discreteItemSets.Count();
		PlayerSpawnPresetDiscreteItemSetSlotData dis;
		for (int i = 0; i < count; i++)
		{
			dis = slotData.discreteItemSets[i];
			
			if (dis.IsValid()) //only when the type exists and spawnWeight is set
			{
				for (int j = 0; j < dis.spawnWeight; j++)
				{
					weightedDiscreteSetIndexes.Insert(i);
				}
			}
		}
		
		dis = null;
		if (weightedDiscreteSetIndexes.Count() > 0)
			dis = slotData.discreteItemSets.Get(weightedDiscreteSetIndexes.GetRandomElement());
		return SpawnDiscreteSlotItemSet(player,dis,slotID);
	}
	
	//! chooses one object from the array
	private static void ProcessCargoEquipment(PlayerBase player, PlayerSpawnPreset data)
	{
		if (!data.HasDiscreteUnsortedItemSetsDefined())
		{
			Debug.Log("No non-empty 'discreteUnsortedItemSets' array found. Skipping cargo spawns","n/a","n/a","ProcessCargoEquipment");
			return;
		}
		
		SelectAndSpawnCargoSet(player,data);
	}
	
	private static bool SelectAndSpawnCargoSet(PlayerBase player, PlayerSpawnPreset data)
	{
		array<int> weightedDiscreteSetIndexes = new array<int>();
		int count = data.discreteUnsortedItemSets.Count();
		PlayerSpawnPresetDiscreteCargoSetData csd;
		for (int i = 0; i < count; i++)
		{
			csd = data.discreteUnsortedItemSets[i];
			
			if (csd.IsValid()) //only when the spawnWeight is set
			{
				for (int j = 0; j < csd.spawnWeight; j++)
				{
					weightedDiscreteSetIndexes.Insert(i);
				}
			}
		}
		
		csd = null;
		if (weightedDiscreteSetIndexes.Count() > 0)
			csd = data.discreteUnsortedItemSets.Get(weightedDiscreteSetIndexes.GetRandomElement());
		return SpawnDiscreteCargoItemSet(player,csd);
	}
	
	private static bool SpawnDiscreteCargoItemSet(PlayerBase player, PlayerSpawnPresetDiscreteCargoSetData csd)
	{
		SpawnComplexChildrenItems(player,csd);
		SpawnSimpleChildrenItems(player,csd);
		return true;
	}
	
	private static bool SpawnDiscreteSlotItemSet(EntityAI parent, PlayerSpawnPresetDiscreteItemSetSlotData dis, int slotID)
	{
		if (!dis)
		{
			Debug.Log("No PlayerSpawnPresetDiscreteItemSet found. Skipping spawn for slot: " + InventorySlots.GetSlotName(slotID),"n/a","n/a","SpawnDiscreteSlotItemSet");
			return false;
		}
		
		ItemBase item = ItemBase.Cast(parent.GetInventory().CreateAttachmentEx(dis.itemType,slotID));
		if (item)
		{
			HandleNewItem(item,dis);
		}
		else if (dis.itemType != string.Empty)
		{
			Debug.Log("FAILED spawning item type: " + dis.itemType + " into slot: " + InventorySlots.GetSlotName(slotID) + " of parent: " + parent,"n/a","n/a","SpawnDiscreteSlotItemSet");
			return false;
		}
		
		return item != null;
	}
	
	//! could spawn other items recursively. Parent item is guaranteed here.
	private static bool SpawnComplexChildrenItems(EntityAI parent, notnull PlayerSpawnPresetItemSetBase data)
	{
		if (!data.complexChildrenTypes || data.complexChildrenTypes.Count() < 1) //no children defined, still valid!
		{
			return false;
		}
		
		foreach (PlayerSpawnPresetComplexChildrenType cct : data.complexChildrenTypes)
		{
			//todo: slotID option over nyah?
			if (cct.itemType == string.Empty)
			{
				Debug.Log("Empty item type found in 'complexChildrenTypes' of parent : " + parent,"n/a","n/a","SpawnSimpleChildrenItems");
				continue;
			}
			
			ItemBase item;
			Class.CastTo(item,CreateChildItem(parent,cct.itemType));
			
			if (item)
			{
				HandleNewItem(item,cct);
			}
			else
			{
				Debug.Log("FAILED spawning item: " + cct.itemType + " of parent: " + parent,"n/a","n/a","SpawnComplexChildrenItems");
			}
		}
		
		return true;
	}
	
	private static bool SpawnSimpleChildrenItems(EntityAI parent, PlayerSpawnPresetItemSetBase data)
	{
		if (!data || !data.simpleChildrenTypes || data.simpleChildrenTypes.Count() < 1) //no children defined, still valid!
		{
			return false;
		}
		
		int count = data.simpleChildrenTypes.Count();
		string itemType;
		for (int i = 0; i < count; i++)
		{
			itemType = data.simpleChildrenTypes[i];
			if (itemType == string.Empty)
			{
				Debug.Log("Empty item type found at idx: " + i.ToString() + " of 'simpleChildrenTypes' array. Skipping","n/a","n/a","SpawnSimpleChildrenItems");
				continue;
			}
			
			ItemBase item;
			Class.CastTo(item,CreateChildItem(parent,itemType));
			
			if (item)
			{
				if (!data.simpleChildrenUseDefaultAttributes)
					ApplyAttributes(item,data.attributes);
			}
			else
			{
				Debug.Log("FAILED spawning item type: " + itemType + " to parent: " + parent,"n/a","n/a","SpawnSimpleChildrenItems");
			}
		}
		return true;
	}
	
	private static void HandleNewItem(notnull ItemBase item, PlayerSpawnPresetItemSetBase data)
	{
		ApplyAttributes(item,data.attributes);
		
		PlayerBase player;
		if (Class.CastTo(player,item.GetHierarchyRootPlayer()) && data.GetQuickbarIdx() > -1)
			player.SetQuickBarEntityShortcut(item,data.GetQuickbarIdx());
		
		SpawnComplexChildrenItems(item,data);
		SpawnSimpleChildrenItems(item,data);
	}
	
	private static EntityAI CreateChildItem(EntityAI parent, string type)
	{
		PlayerBase player;
		ItemBase newItem;
		if (Class.CastTo(player,parent)) //special behavior
		{
			int count = player.GetInventory().AttachmentCount();
			ItemBase attachment;
			for (int i = 0; i < count; i++)
			{
				attachment = ItemBase.Cast(player.GetInventory().GetAttachmentFromIndex(i));
				if (Class.CastTo(newItem,attachment.GetInventory().CreateInInventory(type)))
					return newItem;
			}
			
			Debug.Log("FAILED spawning item: " + type + ", it fits in no cargo or attachment on any worn item","n/a","n/a","CreateChildItem");
			return null;
		}
		
		//weapon magazine exception
		if (GetGame().ConfigIsExisting(CFG_MAGAZINESPATH + " " + type) && parent.IsWeapon())
		{
			Weapon_Base wep
			if (Class.CastTo(wep,parent))
				return wep.SpawnAttachedMagazine(type);
		}
		
		return parent.GetInventory().CreateInInventory(type);
	}
	
	private static void ApplyAttributes(ItemBase item, PlayerSpawnAttributesData attributes)
	{
		if (!attributes)
			return;
		
		float health01 = Math.RandomFloatInclusive(attributes.healthMin,attributes.healthMax);
		item.SetHealth01("","Health",health01);
		
		float quantity01 = Math.RandomFloatInclusive(attributes.quantityMin,attributes.quantityMax);
		if (item.IsMagazine())
		{
			Magazine mag = Magazine.Cast(item);
			//todo: magazine bullet customization?
			/*if (attributes.magazineAmmoOrdered && attributes.magazineAmmoOrdered.Count() > 0)
			{
				mag.ServerSetAmmoCount(0);
				foreach (string bulletType : attributes.magazineAmmoOrdered)
				{
					mag.ServerStoreCartridge(health01,bulletType);
				}
				mag.SetSynchDirty();
			}
			else*/
			{
				int ammoQuantity = (int)Math.Lerp(0,mag.GetAmmoMax(),quantity01);
				mag.ServerSetAmmoCount(ammoQuantity);
			}
		}
		else //'varQuantityDestroyOnMin' quantity safeguard
		{
			float quantityAbsolute = Math.Lerp(item.GetQuantityMin(),item.GetQuantityMax(),quantity01);
			quantityAbsolute = Math.Round(quantityAbsolute); //avoids weird floats
			if (quantityAbsolute <= item.GetQuantityMin() && item.ConfigGetBool("varQuantityDestroyOnMin"))
				quantityAbsolute++;
			item.SetQuantity(quantityAbsolute);
		}
	}
}
