class BUISOptic : ItemOptics
{
	override bool IsSightOnly()
	{
		return true;
	}

	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent))
			return false;

		if (parent.IsKindOf("M4A1_Base"))
			return true;

		return false;
	}
}