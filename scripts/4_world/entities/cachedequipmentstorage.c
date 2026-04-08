//#define CACHED_EQUIPMENT_STORAGE_LOGGING

class CachedEquipmentStorageInventoryEntry
{
	int m_SlotId = -1;
	
	string m_CallerMethod;
	EntityAI m_Entity;
	GameInventory m_GameInventory;
	
	ECachedEquipmentOperationMode m_OperationMode;
}

typedef map<ECachedEquipmentItemCategory, ref array<ref CachedEquipmentItemAttribute>> CachedItemAttributesPerCategory

class CachedEquipmentStorage : CachedEquipmentStorageBase
{
	private DayZPlayerImplement m_Player;

	void CachedEquipmentStorage(DayZPlayerImplement player)
	{
		m_Player = player;
	}
	
	// --------------------------------------------------------------------------------
	
	override void OnItemAttached(notnull Entity entity, int slotId, notnull Entity parent)
	{
		EntityAI item = EntityAI.Cast(entity);
		ECachedEquipmentItemCategory category = item.GetCachedEquipmentCategory();
		if (category != ECachedEquipmentItemCategory.NONE && !IsStored(entity, category))
		{
			CachedEquipmentItemAttribute newItemAttributes 	= new CachedEquipmentItemAttribute();
			newItemAttributes.m_Entity					 	= entity;
			newItemAttributes.m_Parent 						= parent;
			newItemAttributes.m_InventoryDepth 				= item.GetHierarchyLevel();
			newItemAttributes.m_SlotIndex 					= slotId;
			newItemAttributes.m_InventoryPlacement 			= ECachedEquipmentPlacement.ATTACHMENT;
			
			#ifdef ENABLE_LOGGING
			LogMessage(string.Format("parent:%1 entity:%2 (slot:%3)", parent, entity, InventorySlots.GetSlotName(slotId)), "OnItemAttached");
			#endif

			Insert(category, newItemAttributes);
		}
		
		CachedEquipmentStorageInventoryEntry params 	= new CachedEquipmentStorageInventoryEntry();
		params.m_Entity 			= item;
		params.m_GameInventory 		= item.GetInventory();
		params.m_SlotId 			= slotId;
		params.m_CallerMethod 		= "OnItemAttached";
		params.m_OperationMode 		= ECachedEquipmentOperationMode.INSERT;
		
		ProcessItemInventory(params);
	}
	
	// --------------------------------------------------------------------------------

	override void OnItemDetached(notnull Entity entity, int slotId, notnull Entity parent)
	{
		EntityAI item = EntityAI.Cast(entity);
		ECachedEquipmentItemCategory category = item.GetCachedEquipmentCategory();
		if (category != ECachedEquipmentItemCategory.NONE)
		{
			#ifdef ENABLE_LOGGING
			LogMessage(string.Format("parent:%1 entity:%2 (slot:%3)", parent, entity, InventorySlots.GetSlotName(slotId)), "OnItemDetached");
			#endif

			Remove(category, entity);				
		}
		
		CachedEquipmentStorageInventoryEntry params 	= new CachedEquipmentStorageInventoryEntry();
		params.m_Entity 			= item;
		params.m_GameInventory 		= item.GetInventory();
		params.m_SlotId 			= slotId;
		params.m_CallerMethod 		= "OnItemDetached";
		params.m_OperationMode 		= ECachedEquipmentOperationMode.REMOVE;
		
		ProcessItemInventory(params);
	}
	
	// --------------------------------------------------------------------------------

	override void OnItemCargoIn(notnull Entity entity)
	{
		EntityAI item = EntityAI.Cast(entity);
		if (!item.GetHierarchyParent())
			return;

		ECachedEquipmentItemCategory category = item.GetCachedEquipmentCategory();
		if (category != ECachedEquipmentItemCategory.NONE && !IsStored(entity, category))
		{
			CachedEquipmentItemAttribute newItemAttributes 	= new CachedEquipmentItemAttribute();
			newItemAttributes.m_Entity 						= entity;
			newItemAttributes.m_Parent 						= item.GetHierarchyParent();
			newItemAttributes.m_InventoryDepth 				= item.GetHierarchyLevel();
			newItemAttributes.m_InventoryPlacement 			= ECachedEquipmentPlacement.CARGO;

			#ifdef ENABLE_LOGGING
			LogMessage(string.Format("parent:%1 entity:%2", item.GetHierarchyParent(), entity), "OnItemCargoIn");
			#endif

			Insert(category, newItemAttributes);
		}
		
		CachedEquipmentStorageInventoryEntry params 	= new CachedEquipmentStorageInventoryEntry();
		params.m_Entity 			= item;
		params.m_GameInventory 		= item.GetInventory();
		params.m_CallerMethod 		= "OnItemCargoIn";
		params.m_OperationMode 		= ECachedEquipmentOperationMode.INSERT;
		
		ProcessItemInventory(params);
	}
	
	// --------------------------------------------------------------------------------
	
	override void OnItemCargoOut(notnull Entity entity)
	{
		EntityAI item = EntityAI.Cast(entity);
		ECachedEquipmentItemCategory category = item.GetCachedEquipmentCategory();
		if (category != ECachedEquipmentItemCategory.NONE)
		{
			#ifdef ENABLE_LOGGING
			LogMessage(string.Format("parent:%1 entity:%2", item.GetHierarchyParent(), entity), "OnItemCargoOut");
			#endif
			
			Remove(category, entity);
		}
		
		CachedEquipmentStorageInventoryEntry params = new CachedEquipmentStorageInventoryEntry();
		params.m_Entity 							= item;
		params.m_GameInventory 						= item.GetInventory();
		params.m_CallerMethod 						= "OnItemCargoOut";
		params.m_OperationMode 						= ECachedEquipmentOperationMode.REMOVE;
		
		ProcessItemInventory(params);
	}
	
	// --------------------------------------------------------------------------------

	protected void ProcessItemInventory(CachedEquipmentStorageInventoryEntry params)
	{
		CachedItemAttributesPerCategory attachmentAttributesPerCategory = ProcessAttachments(params);
		foreach (ECachedEquipmentItemCategory attachmentCategory, array<ref CachedEquipmentItemAttribute> attachmentAttributes : attachmentAttributesPerCategory)
		{
			foreach (CachedEquipmentItemAttribute attachmentAttribute : attachmentAttributes)
			{
				if (params.m_OperationMode == ECachedEquipmentOperationMode.INSERT)
					Insert(attachmentCategory, attachmentAttribute);
				else
					Remove(attachmentCategory, attachmentAttribute.m_Entity);
			}
		}
		
		CachedItemAttributesPerCategory cargoAttributesPerCategory = ProcessCargo(params);
		foreach (ECachedEquipmentItemCategory cargoCategory, array<ref CachedEquipmentItemAttribute> cargoAttributes : cargoAttributesPerCategory)
		{
			foreach (CachedEquipmentItemAttribute cargoAttribute : cargoAttributes)
			{
				if (params.m_OperationMode == ECachedEquipmentOperationMode.INSERT)
					Insert(cargoCategory, cargoAttribute);
				else
					Remove(cargoCategory, cargoAttribute.m_Entity);
			}
		}		
	}
	
	// --------------------------------------------------------------------------------
	/**
	 * \brief Processes attachments of item given through params
	 * @param params inventory item setting defined by CachedEquipmentStorageInventoryEntry
	 * \return map containing ECachedEquipmentItemCategory and array of CachedEquipmentItemAttribute for further processing (cache insert/removal)
	 */
	private map<ECachedEquipmentItemCategory, ref array<ref CachedEquipmentItemAttribute>> ProcessAttachments(CachedEquipmentStorageInventoryEntry params)
	{
	    CachedItemAttributesPerCategory attachmentAttributesPerCategory = new CachedItemAttributesPerCategory();
	    
	    int attachments = params.m_GameInventory.AttachmentCount();
	    for (int attachmentIndex = 0; attachmentIndex < attachments; ++attachmentIndex)
	    {
	        EntityAI attachment = params.m_GameInventory.GetAttachmentFromIndex(attachmentIndex);
	        ECachedEquipmentItemCategory category = attachment.GetCachedEquipmentCategory();
	        if (category == ECachedEquipmentItemCategory.NONE)
	            continue;
			
			if (params.m_OperationMode == ECachedEquipmentOperationMode.INSERT && IsStored(attachment, category))
				continue;
	        
	        CachedEquipmentItemAttribute newItemAttributes 	= new CachedEquipmentItemAttribute();
	        newItemAttributes.m_Entity 						= attachment;
	        newItemAttributes.m_Parent 						= attachment.GetHierarchyParent();
	        newItemAttributes.m_InventoryDepth 				= attachment.GetHierarchyLevel();
	        newItemAttributes.m_SlotIndex 					= params.m_SlotId;
	        newItemAttributes.m_InventoryPlacement 			= ECachedEquipmentPlacement.ATTACHMENT;
	
	        array<ref CachedEquipmentItemAttribute> arr;
	        if (!attachmentAttributesPerCategory.Find(category, arr))
	        {
	            arr = new array<ref CachedEquipmentItemAttribute>();
	            attachmentAttributesPerCategory.Set(category, arr);
	        }
			
			#ifdef ENABLE_LOGGING
			LogMessage(string.Format("parent:%1 entity:%2", newItemAttributes.m_Parent, attachment), string.Format("%1:ProcessAttachments", params.m_CallerMethod));
			#endif

	        arr.Insert(newItemAttributes);
	    }
		
	    return attachmentAttributesPerCategory;
	}
	
	// --------------------------------------------------------------------------------
	/**
	 * \brief Processes cargo of item given through params
	 * @param params inventory item setting defined by CachedEquipmentStorageInventoryEntry
	 * \return map containing ECachedEquipmentItemCategory and array of CachedEquipmentItemAttribute for further processing (cache insert/removal)
	 */
	private map<ECachedEquipmentItemCategory, ref array<ref CachedEquipmentItemAttribute>> ProcessCargo(CachedEquipmentStorageInventoryEntry params)
	{
		CachedItemAttributesPerCategory cargoAttributesPerCategory = new CachedItemAttributesPerCategory();
		if (params.m_GameInventory.GetCargo())
		{
			int cargoCount = params.m_GameInventory.GetCargo().GetItemCount();
	        for (int cargoIndex = 0; cargoIndex < cargoCount; ++cargoIndex)
	        {
	            EntityAI cargo = params.m_GameInventory.GetCargo().GetItem(cargoIndex);
				ECachedEquipmentItemCategory category = cargo.GetCachedEquipmentCategory();
				if (category == ECachedEquipmentItemCategory.NONE)
					continue;

				if (params.m_OperationMode == ECachedEquipmentOperationMode.INSERT && IsStored(cargo, category))
					continue;

				CachedEquipmentItemAttribute newItemAttributes = new CachedEquipmentItemAttribute();
				
				newItemAttributes.m_Entity 					= cargo;
				newItemAttributes.m_Parent 					= cargo.GetHierarchyParent();				
				newItemAttributes.m_InventoryDepth 			= cargo.GetHierarchyLevel();
				newItemAttributes.m_InventoryPlacement 		= ECachedEquipmentPlacement.CARGO;

		        array<ref CachedEquipmentItemAttribute> arr;
		        if (!cargoAttributesPerCategory.Find(category, arr))
		        {
		            arr = new array<ref CachedEquipmentItemAttribute>();
		            cargoAttributesPerCategory.Set(category, arr);
		        }
				
				#ifdef ENABLE_LOGGING
				LogMessage(string.Format("parent:%1 entity:%2", newItemAttributes.m_Parent, cargo), string.Format("%1:ProcessCargo", params.m_CallerMethod));
				#endif

		        arr.Insert(newItemAttributes);
			}
		}

		return cargoAttributesPerCategory;
	}
	
	// --------------------------------------------------------------------------------
	
	#ifdef ENABLE_LOGGING
	private void LogMessage(string message, string method)
	{
		#ifdef CACHED_EQUIPMENT_STORAGE_LOGGING
		Debug.Log(message, this.ClassName(), "", string.Format("%1(%2)", m_Player.ToString(), Debug.GetDebugName(m_Player)), method);
		#endif
	}
	#endif
}
