class AntiPestsSpray: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDisinfectPlantBit);
		AddAction(ActionDisinfectPlant);
		AddAction(ActionForceDrink);
		AddAction(ActionDrink);
	}
};
class CannabisSeeds : SeedBase{};
class PepperSeeds : SeedBase{};
class Plant_Cannabis : PlantBase {};
class Plant_Pepper : PlantBase
{
	void Plant_Pepper()
	{
		m_FullMaturityTime = 2250;
	}
};
class Plant_Potato : PlantBase
{
	void Plant_Potato()
	{
		m_FullMaturityTime = 2850;
	}
};
class Plant_Pumpkin : PlantBase
{
	void Plant_Pumpkin()
	{
		m_FullMaturityTime = 2850;
	}
};
class Plant_Tomato : PlantBase
{
	void Plant_Tomato()
	{
		m_FullMaturityTime = 1650;
	}
};
class Plant_Zucchini : PlantBase
{
	void Plant_Zucchini()
	{
		m_FullMaturityTime = 1350;
	}
};
class PumpkinSeeds : SeedBase{};
class SeedBase : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionPlantSeed);
	}
};
class TomatoSeeds : SeedBase{};
class ZucchiniSeeds : SeedBase{};
class PotatoSeed : SeedBase{};