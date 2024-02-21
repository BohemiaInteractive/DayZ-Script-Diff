class PlayerSpawnJsonDataBase : Managed
{
	//! sets default values throughout the freshly created structure as required
	
	bool IsValid()
	{
		return true;
	}
}

class PlayerSpawnJsonData : PlayerSpawnJsonDataBase
{
	ref array<ref PlayerSpawnPreset> presets;
}

class PlayerSpawnPreset : PlayerSpawnJsonDataBase
{
	int spawnWeight; //spawn probability weight
	string name; //optional
	ref array<string> characterTypes;
	ref array<ref PlayerSpawnPresetSlotData> attachmentSlotItemSets;
	ref array<ref PlayerSpawnPresetDiscreteCargoSetData> discreteUnsortedItemSets;
	
	string GetRandomCharacterType()
	{
		if (characterTypes && characterTypes.Count() > 0)
			return characterTypes.GetRandomElement();
		
		Debug.Log("No characterTypes defined. Falling back to 'default' character type, or random, if undefined","n/a","n/a","PlayerSpawnPreset");
		return string.Empty;
	}
	
	override bool IsValid()
	{
		if (!super.IsValid())
			return false;
		
		if (spawnWeight < 1)
		{
			Debug.Log("Invalid spawn weight, skipping preset: " + name,"n/a","Validation","PlayerSpawnPreset");
			return false;
		}
		
		return true;
	}
	
	//! preset might be valid even with no attachmentSlotItemSets configured, checked separately
	bool HasAttachmentSlotSetsDefined()
	{
		return attachmentSlotItemSets && attachmentSlotItemSets.Count() > 0;
	}
	
	//! preset might be valid even with no unsorted item sets configured, checked separately
	bool HasDiscreteUnsortedItemSetsDefined()
	{
		return discreteUnsortedItemSets && discreteUnsortedItemSets.Count() > 0;
	}
}

class PlayerSpawnPresetSlotData : PlayerSpawnJsonDataBase
{
	string slotName;
	ref array<ref PlayerSpawnPresetDiscreteItemSetSlotData> discreteItemSets;
	
	//! Translates slot name to match something from both 'CfgSlots' and 'attachments[]' in entity's config
	bool TranslateAndValidateSlot(EntityAI parent, inout int slotID)
	{
		string tmp = slotName;
		if (slotName == "shoulderL")
		{
			tmp = "Shoulder";
		}
		else if (slotName == "shoulderR")
		{
			tmp = "Melee";
		}
		
		slotID = InventorySlots.GetSlotIdFromString(tmp);
		if (!InventorySlots.IsSlotIdValid(slotID))
		{
			Debug.Log("Wrong slot name used: " + slotName,"n/a","Validation","PlayerSpawnPresetSlotData");
			return false;
		}
		if (!parent)
		{
			Debug.Log("No parent entity found when trying to populate slot: " + slotName,"n/a","Validation","PlayerSpawnPresetSlotData");
			return false;
		}
		if (!parent.GetInventory().HasAttachmentSlot(slotID))
		{
			Debug.Log("Slot: " + slotName + " undefined on entity: " + parent.GetType(),"n/a","Validation","PlayerSpawnPresetSlotData");
			return false;
		}
		
		return true;
	}
	
	//! slot name validity checked separately
	override bool IsValid()
	{
		if (!super.IsValid())
			return false;
		
		if (discreteItemSets == null || discreteItemSets.Count() < 1)
		{
			Debug.Log("discreteItemSets for slot: " + slotName + " undefined","n/a","Validation","PlayerSpawnPresetSlotData");
			return false;
		}
		
		return true;
	}
}

//! base for any item set
class PlayerSpawnPresetItemSetBase : PlayerSpawnJsonDataBase
{
	bool simpleChildrenUseDefaultAttributes;
	ref PlayerSpawnAttributesData attributes;
	ref array<ref PlayerSpawnPresetComplexChildrenType> complexChildrenTypes;
	ref array<string> simpleChildrenTypes;
	
	//! overriden later
	int GetQuickbarIdx()
	{
		return -1;
	}
}

//base for DISCRETE item sets
class PlayerSpawnPresetDiscreteItemSetBase : PlayerSpawnPresetItemSetBase
{
	int spawnWeight;
	
	override bool IsValid()
	{
		if (!super.IsValid())
			return false;
		
		if (spawnWeight < 1)
		{
			Debug.Log("Invalid spawnWeight set for a discrete item set!","n/a","Validation","PlayerSpawnPresetDiscreteItemSetBase");
			return false;
		}
		return true;
	}
}

//! one item set for slot
class PlayerSpawnPresetDiscreteItemSetSlotData : PlayerSpawnPresetDiscreteItemSetBase
{
	string itemType;
	int quickBarSlot;
	
	override bool IsValid()
	{
		if (!super.IsValid())
			return false;
		
		//empty 'itemType' is valid alternative here
		
		if (!attributes)
		{
			Debug.Log("No attributes defined for a discrete item set!","n/a","Validation","PlayerSpawnPresetDiscreteItemSetSlotData");
			return false;
		}
		
		//unable to verify any of the other integers, since they always default to '0'. Needs to be configured carefully!
		
		return true;
	}
	
	override int GetQuickbarIdx()
	{
		return quickBarSlot;
	}
}

//! one set for cargo
class PlayerSpawnPresetDiscreteCargoSetData : PlayerSpawnPresetDiscreteItemSetBase
{
	string name;
}

//! used for specific hierarchical child spawning
class PlayerSpawnPresetComplexChildrenType : PlayerSpawnPresetItemSetBase
{
	string itemType;
	int quickBarSlot;
	
	override bool IsValid()
	{
		if (!super.IsValid())
			return false;
		
		return itemType != string.Empty; //needs item type to function
	}
	
	override int GetQuickbarIdx()
	{
		return quickBarSlot;
	}
}

class PlayerSpawnAttributesData : PlayerSpawnJsonDataBase
{
	float healthMin;
	float healthMax;
	float quantityMin;
	float quantityMax;
	//ref array<string> magazineAmmoOrdered;
}
