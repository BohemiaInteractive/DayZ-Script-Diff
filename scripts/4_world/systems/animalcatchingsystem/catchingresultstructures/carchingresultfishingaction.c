class CarchingResultFishingAction : CatchingResultBase
{
	override EntityAI SpawnAndSetup(out int yItemIdx, vector v = vector.Zero)
	{
		ItemBase catch = null;
		if (Class.CastTo(catch,super.SpawnAndSetup(yItemIdx,v)))
		{
			catch.SetQuantityNormalized(m_Quality); //just go with it
			catch.SetWet(0.3);
		}
		return catch;
	}
	
	float GetCurrentCycleTime(CatchingContextFishingRodAction ctx)
	{
		return FishYieldItemBase.Cast(m_YItem).GetCycleTimeForYieldItem(ctx);
	}
}