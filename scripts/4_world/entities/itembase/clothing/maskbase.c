class MaskBase extends Clothing
{
	float m_LowFilterEventTime;
	
	override bool IsGasMask()
	{
		return true;
	}
	
	override array<int> GetEffectWidgetTypes()
	{
		return {EffectWidgetsTypes.MASK_OCCLUDER, EffectWidgetsTypes.MASK_BREATH};
	}
	
	override bool AllowFoodConsumption()
	{
		return false;
	}
	
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory("GasMask_Filter");
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
	}
	
	
	EntityAI GetExternalFilter()
	{
		return FindAttachmentBySlotName("GasMaskFilter");
	}
	
	//! has either external or integrated non-empty non-ruined filter ?
	bool HasValidFilter()
	{
		ItemBase filter = ItemBase.Cast(GetExternalFilter());
		if (filter && !filter.IsRuined() && filter.GetQuantity() > 0)
			return true;
		else if (GetQuantity() > 0 && !IsRuined())
			return true;

		return false;
	}
	
	float GetFilterQuantityMax()
	{
		ItemBase filter = ItemBase.Cast(GetExternalFilter());
		if (filter) //mask can have a filter, and filter is attached
			return filter.GetQuantityMax();
		else if (HasQuantity())//filter was not attached, checking if mask has internal filter
			return GetQuantityMax();
		
		//mask does not have a filter attached nor does it have internal filter, max quantity can't be obtained
		return 0;
	}
	
	float GetFilterQuantity()
	{
		ItemBase filter = ItemBase.Cast(GetExternalFilter());
		if (filter)
			return filter.GetQuantity();
		else
			return GetQuantity();
	}
	
	float GetFilterQuantity01()
	{
		if (!HasValidFilter())
		{
			return 0;
		}
		
		ItemBase filter = ItemBase.Cast(GetExternalFilter());
		float quantity, quantityMax;
		
		if (filter)
		{
			quantity = filter.GetQuantity();
			quantityMax = filter.GetQuantityMax();
		}
		else
		{
			quantity = GetQuantity();
			quantityMax = GetQuantityMax();
		}
		
		return Math.InverseLerp(0, quantityMax, quantity);
	}
	
	bool IsExternalFilterAttached()
	{
		return ItemBase.Cast(FindAttachmentBySlotName("GasMaskFilter")) != null;
	}
	
	bool HasIntegratedFilter()
	{
		return HasQuantity();
	}
	
	bool CanHaveExternalFilter()
	{
		return !HasIntegratedFilter();
	}
	
	bool ConsumeQuantity(float quantity, PlayerBase consumer_player)
	{
		ItemBase filter = ItemBase.Cast(FindAttachmentBySlotName("GasMaskFilter"));
		ItemBase itemToConsume = null;
		
		if (HasQuantity() && GetQuantity() > 0)
		{
			itemToConsume = this;//this mask itself has quantity(masks with integrated, non-attached filter)
		}
		else if (HasValidFilter())
		{
			itemToConsume = filter;
		}
		if (itemToConsume)
		{
			itemToConsume.AddQuantity(-quantity);
			OnQuantityConsumed(itemToConsume, consumer_player, quantity);
			return true;
		}
		
		return false;
	}

	void OnQuantityConsumed(notnull ItemBase filter, PlayerBase consumer_player) {};
	
	void OnQuantityConsumed(notnull ItemBase filter, PlayerBase consumer_player, float quantity)
	{
		float damage = quantity * filter.GetFilterDamageRatio();
		filter.AddHealth("","", -damage);
	}
}
