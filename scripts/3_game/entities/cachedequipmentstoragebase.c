class CachedEquipmentStorageQuery
{
	int m_MaximumDepth = -1;
	ECachedEquipmentItemCategory m_Category = ECachedEquipmentItemCategory.NONE;
	ECachedEquipmentPlacement m_Placement 	= ECachedEquipmentPlacement.ANY;
}

class CachedEquipmentItemAttribute
{
	int m_SlotIndex = -1;
	
	int m_InventoryDepth;

	Entity m_Entity;
	Entity m_Parent;

	ECachedEquipmentPlacement m_InventoryPlacement = ECachedEquipmentPlacement.ATTACHMENT;
	
	bool HasParent()
	{
		return m_Parent != null;
	}
	
	int InventoryDepth()
	{
		return m_InventoryDepth;
	}
	
	string Debug()
	{
		if (m_SlotIndex >= 0)
		{
			return string.Format("entity:%1(%2 - %3), parent:%4, inventoryDepth:%5",
				m_Entity,
				EnumTools.EnumToString(ECachedEquipmentPlacement, m_InventoryPlacement),
				InventorySlots.GetSlotName(m_SlotIndex),
				m_Parent,
				m_InventoryDepth,
			);
		}
		
		return string.Format("entity:%1(%2), parent:%3 , inventoryDepth:%4",
			m_Entity,
			EnumTools.EnumToString(ECachedEquipmentPlacement, m_InventoryPlacement),
			m_Parent,
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
		array<Entity> entities = new array<Entity>();
		if (m_ItemsCacheByCategory[query.m_Category])
		{
			foreach (CachedEquipmentItemAttribute itemAttributes : m_ItemsCacheByCategory[query.m_Category])
			{
				bool placementMatched = false;
				bool inventoryDepthMatched = false;

				if (itemAttributes.m_InventoryPlacement == query.m_Placement || query.m_Placement == ECachedEquipmentPlacement.ANY)
					placementMatched = true;
				
				if (itemAttributes.m_InventoryDepth <= query.m_MaximumDepth || query.m_MaximumDepth == -1)
					inventoryDepthMatched = true;
				
				if (placementMatched && inventoryDepthMatched)
					entities.Insert(itemAttributes.m_Entity);
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
		array<Entity> entities = new array<Entity>();
		if (m_ItemsCacheByCategory[category])
		{
			foreach (CachedEquipmentItemAttribute itemAttributes : m_ItemsCacheByCategory[category])
			{
				if (itemAttributes.m_InventoryPlacement == placement || placement == ECachedEquipmentPlacement.ANY)
					entities.Insert(itemAttributes.m_Entity);
			}
		}

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
			Print(string.Format("[%1]", EnumTools.EnumToString(ECachedEquipmentItemCategory, category)));
			PrintCachedEntitiesByCategory(category);
		}
		Print("--------------------------");
	}
	#endif
}
