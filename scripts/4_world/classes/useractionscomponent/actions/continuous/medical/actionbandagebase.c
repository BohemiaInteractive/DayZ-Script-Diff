class ActionBandageBase : ActionContinuousBase
{
	void ApplyBandage(ItemBase item, PlayerBase player)
	{	
		if (player.GetBleedingManagerServer())
			player.GetBleedingManagerServer().RemoveMostSignificantBleedingSourceEx(item);
		
		PluginTransmissionAgents transmissionAgents = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		transmissionAgents.TransmitAgents(item, player, AGT_ITEM_TO_FLESH);
		
		if (item.HasQuantity())
			item.AddQuantity(-1, true);
		else
			item.Delete();
	}
}
