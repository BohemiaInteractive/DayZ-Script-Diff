class ActionWashHeadWettingClothesBase : ActionContinuousBase
{
	private static ref map<int, float> s_WetnessContainer;
	private static ref map<int, float> s_WetnessSurface;

	private map<int, float> GetWetnessMap(bool fromContainer)
	{
		if (fromContainer)
		{
			if (!s_WetnessContainer)
			{
				s_WetnessContainer = new map<int, float>;
				s_WetnessContainer.Insert(InventorySlots.HEADGEAR, 0.24);
				s_WetnessContainer.Insert(InventorySlots.MASK,    0.24);
				s_WetnessContainer.Insert(InventorySlots.BODY,    0.12);
				s_WetnessContainer.Insert(InventorySlots.VEST,    0.08);
				s_WetnessContainer.Insert(InventorySlots.GLOVES,  0.04);
				s_WetnessContainer.Insert(InventorySlots.LEGS,    0.06);
				s_WetnessContainer.Insert(InventorySlots.FEET,    0.04);
				s_WetnessContainer.Insert(InventorySlots.ARMBAND, 0.06);
				s_WetnessContainer.Insert(InventorySlots.HIPS,    0.06);
				s_WetnessContainer.Insert(InventorySlots.BACK,    0.04);
			}
			return s_WetnessContainer;
		}
		else
		{
			if (!s_WetnessSurface)
			{
				s_WetnessSurface = new map<int, float>;
				s_WetnessSurface.Insert(InventorySlots.HEADGEAR, 0.12);
				s_WetnessSurface.Insert(InventorySlots.MASK,    0.24);
				s_WetnessSurface.Insert(InventorySlots.BODY,    0.12);
				s_WetnessSurface.Insert(InventorySlots.VEST,    0.08);
				s_WetnessSurface.Insert(InventorySlots.GLOVES,  0.24);
				s_WetnessSurface.Insert(InventorySlots.LEGS,    0.06);
				s_WetnessSurface.Insert(InventorySlots.FEET,    0.04);
				s_WetnessSurface.Insert(InventorySlots.ARMBAND, 0.06);
				s_WetnessSurface.Insert(InventorySlots.HIPS,    0.06);
				s_WetnessSurface.Insert(InventorySlots.BACK,    0.04);
			}
			return s_WetnessSurface;
		}
	}

	protected float GetEyeIrritationReduction() { return 10.0; }
	protected bool IsFromContainer() { return false; }

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		action_data.m_Player.m_AgentPool.ReduceAgent(eAgents.EYES_IRRITATION, GetEyeIrritationReduction());
		action_data.m_Player.GetThermalBiasHandler().Add(-PlayerConstants.THERMAL_BIAS_WASH_HEAD_DECREMENT);
		action_data.m_Player.GetThermalBiasHandler().SetTemporaryResistance(PlayerConstants.THERMAL_BIAS_WASH_HEAD_TEMPORARY_RESISTANCE_TIME);
		ApplyWetnessToClothes(action_data.m_Player, IsFromContainer(), 1.0);
	}

	override void OnEndServer(ActionData action_data)
	{
		float progress = action_data.m_ActionComponent.GetProgress();
		if (progress > 0)
		{
			action_data.m_Player.m_AgentPool.ReduceAgent(eAgents.EYES_IRRITATION, GetEyeIrritationReduction() * progress);
			action_data.m_Player.GetThermalBiasHandler().Add(-PlayerConstants.THERMAL_BIAS_WASH_HEAD_DECREMENT * progress);
			ApplyWetnessToClothes(action_data.m_Player, IsFromContainer(), progress);
		}

		super.OnEndServer(action_data);
	}

	void ApplyWetnessToClothes(PlayerBase player, bool fromContainer, float scale)
	{
		if (!player)
			return;

		map<int, float> wetnessBySlot = GetWetnessMap(fromContainer);

		foreach (int slotId, float wetnessValue : wetnessBySlot)
		{
			float add  = wetnessValue * scale;

			ItemBase clothing = ItemBase.Cast(player.GetInventory().FindAttachment(slotId));
			if (!clothing)
				continue;

			clothing.SetWet(Math.Clamp(clothing.GetWet() + add, 0.0, 1.0));
		}
	}
}
