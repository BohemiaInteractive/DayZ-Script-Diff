class M9A1_Bayonet extends ToolBase
{
	override bool IsMeleeFinisher()
	{
		return true;
	}
	
	override array<int> GetValidFinishers()
	{
		return {EMeleeHitType.FINISHER_LIVERSTAB,EMeleeHitType.FINISHER_NECKSTAB};
	}
	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent))
			return false;
		
		if (parent.IsSlotReserved(InventorySlots.GetSlotIdFromString("suppressorImpro")) || parent.IsSlotReserved(InventorySlots.GetSlotIdFromString("weaponMuzzleM4")))
			return false;
		
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == null && parent.FindAttachmentBySlotName("weaponMuzzleM4") == null )
		{
			return true;
		}
		return false;
	}

	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		if( parent.IsWeapon() )
		{
			parent.SetBayonetAttached(true,slot_id);
		}		
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		if( parent.IsWeapon() )
		{
			parent.SetBayonetAttached(false);
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionSkinning);
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionDigWorms);
		AddAction(ActionShaveTarget);
		AddAction(ActionDisarmMine);
		AddAction(ActionDisarmExplosive);
		AddAction(ActionShave);
		AddAction(ActionCraftBolts);
	}
}