class Land_Misc_FireBarrel_ColorBase : BuildingWithFireplace
{
	override string GetFireplaceType(int firePointIndex)
	{
		return "FireplaceFireBarrel";
	}
}

class Land_Misc_FireBarrel_Green : Land_Misc_FireBarrel_ColorBase {}
class Land_Misc_FireBarrel_Red : Land_Misc_FireBarrel_ColorBase {}
class Land_Misc_FireBarrel_Yellow : Land_Misc_FireBarrel_ColorBase {}
class Land_Misc_FireBarrel_Blue : Land_Misc_FireBarrel_ColorBase {}