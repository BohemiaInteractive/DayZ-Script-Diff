class ImprovisedSuppressor extends SuppressorBase
{
	
	const int SLOTS_ARRAY = 9;
	
	string slot_names[SLOTS_ARRAY] = { /*"weaponMuzzleAK", */"weaponBayonetAK", "weaponBayonet", "weaponBayonetMosin", "weaponBayonetSKS",/* "weaponMuzzleM4",*/ "weaponMuzzleMosin", /*"pistolMuzzle",*/ "weaponMuzzleMP5" };
		

	override bool CanPutAsAttachment( EntityAI parent )
	{
		bool cond_state = true;
		if(!super.CanPutAsAttachment(parent)) {return false;}
		for ( int i = 0; i < SLOTS_ARRAY ; i++ )
		{
			if ( parent.FindAttachmentBySlotName(slot_names[i]) != NULL )
			{
				cond_state = false;
				break;
			}
		}

		if ( cond_state && !parent.IsKindOf("PlateCarrierHolster") && !parent.IsKindOf("PlateCarrierComplete") && !parent.IsKindOf("CarrierHolsterSolo") && !parent.IsKindOf("ChestHolster") )
		{
			return true;
		}
		return false;
	}
}