class CASingleUseQuantityEdible : CASingleUseQuantity
{
	void CASingleUseQuantityEdible(float quantity_used_per_action)
	{
		m_QuantityUsedPerAction = quantity_used_per_action;
	}
	
	override void CalcAndSetQuantity(ActionData action_data)
	{
		super.CalcAndSetQuantity(action_data);
		
		PlayerConsumeData consumeData = new PlayerConsumeData();
		consumeData.m_Type = EConsumeType.ITEM_CONTINUOUS;
		consumeData.m_Amount = m_QuantityUsedPerAction;
		consumeData.m_Source = action_data.m_MainItem;

		consumeData.m_Agents = 0;
		Edible_Base edibleItem;
		if (Edible_Base.CastTo(edibleItem, action_data.m_MainItem))
		{
			if ((edibleItem.GetConsumptionPenaltyContext() & EConsumptionPenaltyContext.DRINK|EConsumptionPenaltyContext.EAT) != EConsumptionPenaltyContext.NONE)
				consumeData.m_Agents = action_data.m_Player.GetBloodyHandsPenaltyAgents();
		}
		
		if (g_Game.IsServer())
		{
			PlayerBase ntarget = PlayerBase.Cast(action_data.m_Target.GetObject());
			if (ntarget)
				ntarget.Consume(consumeData);
			else
				action_data.m_Player.Consume(consumeData);
		}
	}
}
