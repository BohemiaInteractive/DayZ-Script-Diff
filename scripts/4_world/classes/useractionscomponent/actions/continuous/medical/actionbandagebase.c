class ActionBandageBase: ActionContinuousBase
{	

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( target)
		{
			PlayerBase target_player = PlayerBase.Cast(target.GetObject());
			if(target_player)
				return target_player.IsBleeding();
		}
		return player.IsBleeding();
		
	}
	
	void ApplyBandage( ItemBase item, PlayerBase player )
	{	
		if (player.GetBleedingManagerServer() )
		{
			player.GetBleedingManagerServer().RemoveMostSignificantBleedingSourceEx(item);	
		}
		
		PluginTransmissionAgents m_mta = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		m_mta.TransmitAgents(item, player, AGT_ITEM_TO_FLESH);
		
		if (item.HasQuantity())
		{
			item.AddQuantity(-1,true);
		}
		else
		{
			item.Delete();
		}
	}
};