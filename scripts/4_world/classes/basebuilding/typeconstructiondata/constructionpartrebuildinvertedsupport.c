//type
class ConstructionPartRebuildInvertedSupportTypeData : ConstructionPartRebuildTypeData
{
}

//instance
class ConstructionPartRebuildInvertedSupport : ConstructionPartRebuild
{
	override bool InvertSupportBehavior()
	{
		return true;
	}
}