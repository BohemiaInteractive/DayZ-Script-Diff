class LeatherHat_ColorBase extends ClothingBase
{
	/*override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		bool is_mask_only = false;
		
		if ( parent.FindAttachmentBySlotName( "Mask" ) )
		{
			is_mask_only = parent.FindAttachmentBySlotName( "Mask" ).ConfigGetBool( "noHelmet" );
		}
		
		if ( ( GetNumberOfItems() == 0 || !parent || parent.IsMan() ) && !is_mask_only )
		{
			return true;
		}
		return false;
	}*/
}

class LeatherHat_Natural extends LeatherHat_ColorBase {};
class LeatherHat_Beige extends LeatherHat_ColorBase {};
class LeatherHat_Brown extends LeatherHat_ColorBase {};
class LeatherHat_Black extends LeatherHat_ColorBase {};