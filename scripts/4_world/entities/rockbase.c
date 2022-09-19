class RockBase: Object 
{
	override bool IsRock()
	{
		return true;
	}

	int GetAmountOfDrops(ItemBase item)
	{
		if (item)
		{
			switch (item.GetType())
			{
			case "Pickaxe":
				return 4;
	 		case "SledgeHammer":
				return 8;
			}
		}

		return 1;
	}	
	
	/* Defines the yield of the action*/
	void GetMaterialAndQuantityMap(ItemBase item, out map<string,int> output_map)
	{
		if (item == null)
		{
			return;
		}
		
		switch (item.GetType())
		{
		case "Pickaxe":
		case "SledgeHammer":
		case "Hammer":
			output_map.Insert("Stone",1);
			break;
 		case "PipeWrench":
		case "Wrench":
		case "Screwdriver":
		case "Crowbar":
		case "MeatTenderizer":
			output_map.Insert("SmallStone",1);
			break;
		}
	}

	/*Return value defines how much damage the item will take*/
	float GetDamageToMiningItemEachDrop(ItemBase item)
	{
		if (item)
		{
			switch (item.GetType())
			{
			case "SledgeHammer":
			case "Pickaxe":
				return 20;
			case "Wrench":
			case "Screwdriver":
			case "MeatTenderizer":
				return 25;
			case "PipeWrench":
			case "Crowbar":
				return 50;
			case "Hammer":
				return 40;
			}
		}

		return 25;
	}
};

	class Static_stone1: RockBase {};
	class Static_stone2: RockBase {};
	class Static_stone3: RockBase {};
	class Static_stone4: RockBase {};
	class Static_stone5: RockBase {};
	class Static_rock_apart1: RockBase {};
	class Static_rock_apart2: RockBase {};
	class Static_rock_bright_apart1: RockBase {};
	class Static_rock_bright_apart2: RockBase {};
	class Static_rock_bright_monolith1: RockBase {};
	class Static_rock_bright_monolith2: RockBase {};
	class Static_rock_bright_monolith3: RockBase {};
	class Static_rock_bright_monolith4: RockBase {};
	class Static_rock_bright_spike1: RockBase {};
	class Static_rock_bright_spike2: RockBase {};
	class Static_rock_bright_spike3: RockBase {};
	class Static_rock_bright_wallh1: RockBase {};
	class Static_rock_bright_wallh2: RockBase {};
	class Static_rock_bright_wallh3: RockBase {};
	class Static_rock_bright_wallv: RockBase {};
	class Static_rock_monolith1: RockBase {};
	class Static_rock_monolith2: RockBase {};
	class Static_rock_monolith3: RockBase {};
	class Static_rock_monolith4: RockBase {};
	class Static_rock_spike1: RockBase {};
	class Static_rock_spike2: RockBase {};
	class Static_rock_spike3: RockBase {};
	class Static_rock_wallh1: RockBase {};
	class Static_rock_wallh2: RockBase {};
	class Static_rock_wallh3: RockBase {};
	class Static_rock_wallv: RockBase {};
	class Static_rock_stone6: RockBase{};
	class Static_rock_stone6_moss: RockBase{};
	class Static_rock_stone7: RockBase{};
	class Static_rock_stone7_moss: RockBase{};
	class Static_rock_stone8: RockBase{};
	class Static_rock_stone8_moss: RockBase{};
	class Static_rock_stone9: RockBase{};
	class Static_rock_stone9_moss: RockBase{};
	class Static_rock_stone10: RockBase{};
	class Static_rock_stone10_moss: RockBase{};
// 	currently unused rocks
/*
	
	class Static_stone5_Trail_B: RockBase {};
	class Static_stone5_Trail_G: RockBase {};
	class Static_stone5_Trail_R: RockBase {};
	class Static_stone5_Trail_Y: RockBase {};
	class Static_stones_erosion: RockBase {};
*/


//jtomasik - it would be blast being able just to inherit it from RockBase, but the way static objects are handled most likely don't allow it? ask p cimo
// obsolete
/*class Static_r2_boulder1: RockBase
{
};
class Static_r2_boulder2: RockBase
{
};
class Static_small_stone_01_f: RockBase
{
};
class Static_small_stone_02_f: RockBase
{
};
class Static_stone_small_f: RockBase
{
};
class Static_stone_small_w: RockBase
{
};
class Static_bluntstone_01: RockBase
{
};
class Static_bluntstone_01_lc: RockBase
{
};
class Static_bluntstone_02: RockBase
{
};
class Static_bluntstone_02_lc: RockBase
{
};
class Static_bluntstone_03: RockBase
{
};
class Static_SharpStone_01: RockBase
{
};
class Static_sharpstone_01_lc: RockBase
{
};
class Static_sharpstone_02: RockBase
{
};
class Static_sharpstone_02_lc: RockBase
{
};
class Static_sharpstone_03: RockBase
{
};
class Static_sharpstone_03_lc: RockBase
{
};*/