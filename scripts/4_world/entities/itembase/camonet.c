class CamoNet extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachOnTentProxy); //tents
		AddAction(ActionAttachOnSelection); //basebuilding
	}
}