class BuildingWithFireplace : BuildingSuper
{
	void BuildingWithFireplace()
	{
	}
	
	string GetFireplaceType(int firePointIndex)
	{
		return "FireplaceIndoor";
	}
	
	string GetOvenType(int firePointIndex)
	{
		return "OvenIndoor";
	}
};

//! TODO move this to the DLC related scripts before release of Badlands
class BuildingWithFireplaceNasdara : BuildingWithFireplace
{
	void BuildingWithFireplaceNasdara()
	{
	}
	
	override string GetFireplaceType(int firePointIndex)
	{
		return "FireplaceTandoor";
	}
	
	override string GetOvenType(int firePointIndex)
	{
		return "FireplaceTandoor";
	}
	
}

//Moved from bliss folder
class Land_Mil_ControlTower				: BuildingWithFireplace {};
class Land_Ruin_HouseBig1				: BuildingWithFireplace {};
class Land_Ruin_HouseBig1_NoIvy			: BuildingWithFireplace {};
class Land_Ruin_HouseBig1_Half			: BuildingWithFireplace {};
class Land_Ruin_HouseBig1_NoIvy_Half	: BuildingWithFireplace {};
class Land_Ruin_HouseBig2				: BuildingWithFireplace {};
class Land_Ruin_HouseBig2_NoIvy			: BuildingWithFireplace {};
class Land_Ruin_HouseBig2_Half			: BuildingWithFireplace {};
class Land_Ruin_HouseBig2_NoIvy_Half	: BuildingWithFireplace {};
class Land_Ruin_HouseBig3				: BuildingWithFireplace {};
class Land_Ruin_HouseBig3_NoIvy			: BuildingWithFireplace {};
class Land_Ruin_HouseBig3_Half			: BuildingWithFireplace {};
class Land_Ruin_HouseBig3_NoIvy_Half	: BuildingWithFireplace {};
class Land_Ruin_HouseSmall1				: BuildingWithFireplace {};
class Land_Ruin_HouseSmall1_NoIvy		: BuildingWithFireplace {};
class Land_Ruin_HouseSmall2				: BuildingWithFireplace {};
class Land_Ruin_HouseSmall2_NoIvy		: BuildingWithFireplace {};
class Land_House_1W13_Dam				: BuildingWithFireplace {};

//Ruins
class Land_Ruin_Chimney					: BuildingWithFireplace {};

//Sakhal
class Land_Tenement_Small_Brick			: BuildingWithFireplace {};
class Land_Tenement_Small_Brown			: BuildingWithFireplace {};
class Land_Tenement_Small_Yellow		: BuildingWithFireplace {}; 	