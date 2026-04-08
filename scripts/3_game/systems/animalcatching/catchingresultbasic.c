class CatchingResultBasic
{
	protected EntityAI m_Owner;
	protected bool m_OverrideChanceActive = false; //do not perform chance updates
	protected bool m_OverrideQualityActive = false; //overrides quality
	protected float m_CatchChance = 1.0; //guaranteed catch by default
	protected float m_Quality = 1.0; //max quality default
	protected YieldItemBase m_YItem;
	
	void CatchingResultBasic(EntityAI owner)
	{
		m_Owner = owner;
	}
	
	void SetYieldItem(YieldItemBase yItem)
	{
		m_YItem = yItem;
	}
	
	int GetYieldItemParticleId()
	{
		if (m_YItem)
			return m_YItem.GetCatchParticleID();
		
		return ParticleList.INVALID;
	}
	
	protected void SetCatchChance(float val)
	{
		m_CatchChance = Math.Clamp(val,0,1);
	}
	
	protected void SetQuality(float val)
	{
		m_Quality = Math.Clamp(val,0,1);
	}
	
	void SetCatchChanceOverride(bool ovrd, float val = 0.0)
	{
		if (ovrd)
			m_CatchChance = val;
		m_OverrideChanceActive = ovrd;
	}
	
	void SetQualityOverride(bool ovrd, float val = 0.0)
	{
		if (ovrd)
			m_Quality = val;
		m_OverrideQualityActive = ovrd;
	}
	
	void UpdateCatchQuality(CatchingContextBase ctx)
	{
		if (m_OverrideQualityActive)
			return;
		
		if (!m_YItem)
			return;
		
		float val = m_YItem.GetQualityForYieldItem(ctx);
		SetQuality(val);
	}
	
	void UpdateCatchChance(CatchingContextBase ctx)
	{
		if (m_OverrideChanceActive)
			return;
		
		if (!m_YItem)
			return;
		
		float val = m_YItem.GetChanceForYieldItem(ctx);
		SetCatchChance(val);
	}
	
	bool RollChance()
	{
		if (m_CatchChance >= 1)
			return true;
		if (m_CatchChance <= 0)
			return false;
		
		float roll;
		if (m_Owner && m_Owner.GetHierarchyRootPlayer())
			roll = RollChanceSeeded();
		else
			roll = Math.RandomFloat01();
		
		return roll < m_CatchChance;
	}
	
	protected float RollChanceSeeded()
	{
		return 1.0;
	}
	
	EntityAI SpawnAndSetup(out int yItemIdx, vector v = vector.Zero)
	{
		vector pos = v;
		if (v == vector.Zero && m_Owner != null)
			pos = m_Owner.GetPosition();
		
		if (!m_YItem)
			return null;
		
		yItemIdx = m_YItem.GetRegistrationIdx();
		EntityAI ret = EntityAI.Cast(g_Game.CreateObjectEx(m_YItem.GetType(), pos, ECE_PLACE_ON_SURFACE));
		if (ret)
			m_YItem.OnEntityYieldSpawned(ret);
		
		return ret;
	}
};