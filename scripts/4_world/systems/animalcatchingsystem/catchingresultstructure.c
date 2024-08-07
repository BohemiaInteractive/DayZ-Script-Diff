class CatchingResultBase : CatchingResultBasic
{
	override protected float RollChanceSeeded()
	{
		PlayerBase player;
		if (Class.CastTo(player,m_Owner.GetHierarchyRootPlayer()))
			return player.GetRandomGeneratorSyncManager().GetRandom01(RandomGeneratorSyncUsage.RGSAnimalCatching);
		
		ErrorEx("Player not present or wrong entity used | m_Owner: " + m_Owner + " | root entity: " + m_Owner.GetHierarchyRoot());
		return 2.0;
	}
};