class CAContinuousMineRock : CAContinuousMineWood
{	
	void CAContinuousMineRock(float time_between_drops)
	{
		m_TimeBetweenMaterialDrops = time_between_drops;
	}
	
	override bool GetMiningData(ActionData action_data )
	{
		RockBase ntarget;
		if ( Class.CastTo(ntarget, action_data.m_Target.GetObject()) )
		{
			m_AmountOfDrops = ntarget.GetAmountOfDrops(action_data.m_MainItem);
			ntarget.GetMaterialAndQuantityMap(action_data.m_MainItem,m_MaterialAndQuantityMap);
			m_DamageToMiningItemEachDrop = ntarget.GetDamageToMiningItemEachDrop(action_data.m_MainItem);
			m_AdjustedDamageToMiningItemEachDrop = action_data.m_Player.GetSoftSkillsManager().SubtractSpecialtyBonus( m_DamageToMiningItemEachDrop, m_Action.GetSpecialtyWeight(), true );
			return true;
		}
		return false;
	}
};