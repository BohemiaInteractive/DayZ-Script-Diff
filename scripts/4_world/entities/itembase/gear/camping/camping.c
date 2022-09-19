class CamoNetShelter : ItemBase {};
//cluttercutter classes
class MediumTentClutterCutter: Static {};
class LargeTentClutterCutter: Static {};
class SeaChest: DeployableContainer_Base
{
	void SeaChest()
	{
		m_HalfExtents = Vector(0.2,0.5,0.4);
	}
	
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
};
class WoodenCrate: DeployableContainer_Base
{
	void WoodenCrate()
	{
		m_HalfExtents = Vector(0.15,0.25,0.4);
	}
	
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
	
	override void SetActions()
	{
		AddAction(ActionAttachOnSelection);
		super.SetActions();
	}
};
