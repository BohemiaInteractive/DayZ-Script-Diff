
/**@class		InventorySlots
 * @brief		provides access to slot configuration
 **/
class InventorySlots
{	
	/**@NOTE: engine pre-populates this class with first 32 slots from CfgSlots (uppercased)
	 * i.e. currently defined values are
	 *   InventorySlots.INVALID  (has value -1)
	 *   InventorySlots.COUNT
	 **/
	#ifdef DIAG_DEVELOPER
	private void InventorySlots() {}
	private void ~InventorySlots() {}
	#else
	void InventorySlots() {}
	void ~InventorySlots() {}
	#endif

	/**@fn		GetSlotIdFromString
	 * @brief	converts string to slot_id
	 * @param[in]	slot_name	\p	slot name to be find.
	 * @return		slot id or InventorySlots.INVALID
	 *
	 * @example 
	 *		int slot = InventorySlots.GetSlotIdFromString("FireWood");
	 *		if (slot != InventorySlots.INVALID)
	 *
	 * @NOTE the example looks in the DZ/data/config.cpp and searches for class entry Slot_##slot_name
	 *		class Slot_Firewood { whatever };
	 *
	 * i.e. it does NOT look at name= or displayName= attributes of the entry!
	 **/
	static proto native int GetSlotIdFromString(string slot_name);
	/**@fn		GetSlotName
	 * @brief	converts slot_id to string
	 * @param[in]	slot_id	\p	slot id to be find.
	 * @return		string or null string
	 **/
	static proto native owned string GetSlotName(int id);
	/**@fn		GetSlotDisplayName
	 * @brief	converts slot_id to string
	 * @param[in]	slot_id	\p	slot id to be find.
	 * @return		string or null string
	 **/
	static proto native owned string GetSlotDisplayName(int id);
	/**@fn		IsSlotIdValid
	 * @brief	verifies existence of the slot id
	 * @return	true if slot valid
	 **/
	static proto native bool IsSlotIdValid(int slotId);
	/**@fn		GetStackMaxForSlotId
	 * @param[in]	slot_id	\p	queried slot
	 * @return	stackMax attribute from corresponding entry (or default value == 1)
	 **/
	static proto native int GetStackMaxForSlotId(int slot_Id);
	/**@fn		GetShowForSlotId
	 * @param[in]	slot_id	\p	queried slot
	 * @return	show attribute from corresponding entry (or default value == true)
	 **/
	static proto native bool GetShowForSlotId(int slot_Id);
	/**@fn		GetAutoAttachForSlotId
	 * @param[in]	slot_id	\p	queried slot
	 * @return	auto attach attribute from corresponding entry (or default value == 1)
	 **/
	static proto native bool GetAutoAttachForSlotId(int slot_Id);
	/**@fn		GetBoneNameForSlotId
	 * @param[in]	slot_id	\p	queried slot
	 * @param[out]	bone_name	\p	selection filled with attribute from corresponding entry
	 * @return	true if slot valid
	 **/	
	static proto bool GetBoneNameForSlotId(int slot_Id, out string bone_name);
	/**@fn		GetSelectionForSlotId
	 * @param[in]	slot_id	\p	queried slot
	 * @param[out]	selection	\p	selection filled with attribute from corresponding entry
	 * @return	true if slot valid
	 **/
	static proto bool GetSelectionForSlotId(int slot_Id, out string selection);

	/**@fn		GetBoneIndexForSlotId
	 * @param[in]	slot_id	\p	queried slot
	 * @return	stackMax attribute from corresponding entry (or default value == 1)
	 **/
	//static proto native int GetBoneIndexForSlotId (int slot_Id);
};


