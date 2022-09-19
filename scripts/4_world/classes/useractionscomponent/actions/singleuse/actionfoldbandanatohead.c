class ActionFoldBandanaToHead : ActionFoldEntityToSlot
{
	void ActionFoldBandanaToHead ()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_SlotID = InventorySlots.HEADGEAR;
	}

	override string GetText () { return "#bandana_to_head"; }
};
