class LeatherSack_ColorBase extends Clothing
{
	override bool IsClothing()
	{
		return true;
	}

};

class LeatherSack_Natural extends LeatherSack_ColorBase {};
class LeatherSack_Black extends LeatherSack_ColorBase {};
class LeatherSack_Beige extends LeatherSack_ColorBase {};
class LeatherSack_Brown extends LeatherSack_ColorBase {};