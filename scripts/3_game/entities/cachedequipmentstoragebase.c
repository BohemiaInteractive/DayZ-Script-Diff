class CachedEquipmentStorageQuery
{
	int m_MaximumDepth = -1;
	ref TIntArray m_ParentPlacement				= new TIntArray();
	ECachedEquipmentItemCategory m_Category 	= ECachedEquipmentItemCategory.NONE;
	ECachedEquipmentPlacement m_Placement 		= ECachedEquipmentPlacement.ANY;
}

class CachedEquipmentItemAttribute
{
	int m_SlotIndex = -1;
	
	int m_InventoryDepth;
	int m_ParentInventoryPlacement	= InventorySlots.INVALID;

	Entity m_Entity;
	Entity m_Parent;

	ECachedEquipmentPlacement m_InventoryPlacement = ECachedEquipmentPlacement.ATTACHMENT;

	int InventoryDepth()
	{
		return m_InventoryDepth;
	}
	
	string Debug()
	{
		if (m_SlotIndex >= 0)
		{
			return string.Format("entity:%1(%2 - %3), parent:%4(%5), inventoryDepth:%6",
				m_Entity,
				EnumTools.EnumToString(ECachedEquipmentPlacement, m_InventoryPlacement),
				InventorySlots.GetSlotName(m_SlotIndex),
				m_Parent,
				EnumTools.EnumToString(InventorySlots, m_ParentInventoryPlacement),
				m_InventoryDepth,
			);
		}
		
		return string.Format("entity:%1(%2), parent:%3(%4) , inventoryDepth:%5",
			m_Entity,
			EnumTools.EnumToString(ECachedEquipmentPlacement, m_InventoryPlacement),
			m_Parent,
			EnumTools.EnumToString(InventorySlots, m_ParentInventoryPlacement),
			m_InventoryDepth,
		);
	}
}

class CachedEquipmentStorageBase
{
	protected ref map<ECachedEquipmentItemCategory, ref array<ref CachedEquipmentItemAttribute>> m_ItemsCacheByCategory;
	
	void CachedEquipmentStorageBase()
	{
		m_ItemsCacheByCategory = new map<ECachedEquipmentItemCategory, ref array<ref CachedEquipmentItemAttribute>>();
	}
	
	void OnItemAttached(notnull Entity entity, int slotId, notnull Entity parent);
	void OnItemDetached(notnull Entity entity, int slotId, notnull Entity parent);
	void OnItemCargoIn(notnull Entity entity);
	void OnItemCargoOut(notnull Entity entity);

	// --------------------------------------------------------------------------------	
	/**
	 * \brief List of entities matching the CachedEquipmentStorageQuery query
	 * @param query Query filter settings
	 */
	array<Entity> GetEntitiesByCategory(CachedEquipmentStorageQuery query)
	{
		TIntArray categoriesAll = m_ItemsCacheByCategory.GetKeyArray();
		TIntArray matchedCategories = new TIntArray();
		
		foreach (int category: categoriesAll)
		{
			if (category & query.m_Category)
				matchedCategories.Insert(category);
		}

		array<Entity> entities = new array<Entity>();

		if (matchedCategories.Count() > 0)
		{
			foreach (int matchedCategory : matchedCategories)
			{
				foreach (CachedEquipmentItemAttribute itemAttributes : m_ItemsCacheByCategory[matchedCategory])
				{
					bool placementMatched 		= false;
					bool parentPlacementMatched = false;
					bool inventoryDepthMatched 	= false;
	
					if (itemAttributes.m_InventoryPlacement == query.m_Placement || query.m_Placement == ECachedEquipmentPlacement.ANY)
						placementMatched = true;
					
					if (query.m_ParentPlacement.Count() == 0 || query.m_ParentPlacement.Find(itemAttributes.m_ParentInventoryPlacement) > INDEX_NOT_FOUND)
						parentPlacementMatched = true;
					
					if (itemAttributes.m_InventoryDepth <= query.m_MaximumDepth || query.m_MaximumDepth == -1)
						inventoryDepthMatched = true;
					
					if (placementMatched && parentPlacementMatched && inventoryDepthMatched)
						entities.Insert(itemAttributes.m_Entity);
				}
			}
		}

		return entities;
	}
	
	// --------------------------------------------------------------------------------
	/**
	 * \brief List of entities matching the category and inventory placement
	 * @param category Matching category
	 * @param placement Inventory placement (see ECachedEquipmentPlacement)
	 */
	array<Entity> GetEntitiesByCategory(ECachedEquipmentItemCategory category, ECachedEquipmentPlacement placement = ECachedEquipmentPlacement.ANY)
	{
		CachedEquipmentStorageQuery query = new CachedEquipmentStorageQuery();
		query.m_Category = category;
		query.m_Placement = placement;
		query.m_MaximumDepth = 2;
		array<Entity> entities = GetEntitiesByCategory(query);

		return entities;
	}
	
	// --------------------------------------------------------------------------------
	
	protected bool IsStored(Entity entity, ECachedEquipmentItemCategory category)
	{
		if (!m_ItemsCacheByCategory.Contains(category))
			return false;

		foreach (CachedEquipmentItemAttribute itemAttributes : m_ItemsCacheByCategory[category])
		{
			if (itemAttributes.m_Entity == entity)
				return true;
		}
		
		return false;
	}
	
	// --------------------------------------------------------------------------------
	
	protected void Insert(ECachedEquipmentItemCategory category, CachedEquipmentItemAttribute attributes)
	{
		array<ref CachedEquipmentItemAttribute> attributesArray = new array<ref CachedEquipmentItemAttribute>();
		if (!m_ItemsCacheByCategory.Contains(category))
			m_ItemsCacheByCategory.Set(category, attributesArray);

		m_ItemsCacheByCategory[category].Insert(attributes);
	}
	
	// --------------------------------------------------------------------------------
	
	protected void Remove(ECachedEquipmentItemCategory category, Entity entity)
	{
		if (!m_ItemsCacheByCategory.Contains(category))
			return;
		
		array<CachedEquipmentItemAttribute> toRemove = new array<CachedEquipmentItemAttribute>();
		foreach (CachedEquipmentItemAttribute attributeToRemove : m_ItemsCacheByCategory[category])
		{
		    if (attributeToRemove.m_Entity == entity)
		        toRemove.Insert(attributeToRemove);
		}

		foreach (CachedEquipmentItemAttribute attribute : toRemove)
		    m_ItemsCacheByCategory[category].RemoveItem(attribute);
	}
	
	// --------------------------------------------------------------------------------
	
	void UpdateParents()
	{
		array<ECachedEquipmentItemCategory> categories = m_ItemsCacheByCategory.GetKeyArray();
		if (!categories)
		    return;
		
		InventoryLocation parentInventoryLocation = new InventoryLocation();
		
		foreach (ECachedEquipmentItemCategory category : categories)
		{
		    if (!m_ItemsCacheByCategory[category])
		        continue;
		        
		    foreach (int index, CachedEquipmentItemAttribute itemAttributes : m_ItemsCacheByCategory[category])
		    {
		        if (!itemAttributes || !itemAttributes.m_Parent)
		            continue;
		        
		        EntityAI parent;
		        if (!Class.CastTo(parent, itemAttributes.m_Parent))
		            continue;
		            
		        GameInventory parentInventory = parent.GetInventory();
		        if (!parentInventory)
		            continue;
		        
		        parentInventory.GetCurrentInventoryLocation(parentInventoryLocation);
		        itemAttributes.m_ParentInventoryPlacement = parentInventoryLocation.GetSlot();
		    }
		}
	}
	
	// --------------------------------------------------------------------------------
	
	#ifdef DIAG_DEVELOPER
	void PrintCachedEntitiesByCategory(ECachedEquipmentItemCategory category)
	{
		if (m_ItemsCacheByCategory[category])
		{
			foreach (CachedEquipmentItemAttribute itemAttributes : m_ItemsCacheByCategory[category])
			{
				Print(itemAttributes.Debug());
			}
		}
	}
	
	// --------------------------------------------------------------------------------
	
	void DumpCache()
	{
		Print("--------------------------");
		Print("Equipped Items Cache Stats");
		
		array<ECachedEquipmentItemCategory> categories = m_ItemsCacheByCategory.GetKeyArray();
		foreach (ECachedEquipmentItemCategory category : categories)
		{
			Print(string.Format("[%1]", EnumTools.EnumFlagsToString(ECachedEquipmentItemCategory, category)));
			PrintCachedEntitiesByCategory(category);
		}
		Print("--------------------------");
	}
	#endif
}
