class ActionFoldBandanaToMask : ActionFoldEntityToSlot
{
	void ActionFoldBandanaToMask ()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_SlotID = InventorySlots.MASK;
	}

	override string GetText () { return "#bandana_to_mask"; }
};
