class MilitaryBoots_ColorBase extends ClothingBase
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent != this )
		{
			return true;
		}
		return false;
	}	
};

class MilitaryBoots_Beige extends MilitaryBoots_ColorBase {};
class MilitaryBoots_Black extends MilitaryBoots_ColorBase {};
class MilitaryBoots_Bluerock extends MilitaryBoots_ColorBase {};
class MilitaryBoots_Brown extends MilitaryBoots_ColorBase {};
class MilitaryBoots_Redpunk extends MilitaryBoots_ColorBase {};