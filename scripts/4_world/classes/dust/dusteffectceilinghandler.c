class DustEffectCeilingHandlerBase
{
	protected static int RANGE_MIN_DEFAULT = 4;
	protected static int RANGE_MAX_DEFAULT = 8;
	protected static float FLOOR_THICKNESS_DEFAULT = 0.4;
	
	protected EntityAI m_Owner;
	protected int m_StepCounter;
	protected int m_NextEffectStepIdx;
	
	void DustEffectCeilingHandlerBase(EntityAI owner)
	{
		m_Owner = owner;
		m_StepCounter = 0;
	}
	
	protected void CalculateNextEffect(string surfaceType, int particleID)
	{
		int rangeMin = RANGE_MIN_DEFAULT;
		int rangeMax = RANGE_MAX_DEFAULT;
		DustEffectManager.AdjustCeilingDustEffectFrequency(surfaceType, particleID, rangeMin, rangeMax);
		m_NextEffectStepIdx = Math.RandomIntInclusive(rangeMin,rangeMax);
	}
	
	void OnStepUpdate(SurfaceInfo surfaceInfo, string boneName = "")
	{
		int ceilingParticleID = surfaceInfo.GetCeilingDustParticleId();
		if (ceilingParticleID <= 0)
			return;
		
		m_StepCounter++;
		
		if (m_StepCounter >= m_NextEffectStepIdx)
		{
			vector pos = GetBonePosition(boneName);
			AdjustBySurfaceThickness(pos, surfaceInfo);
			CreateEffects(ceilingParticleID, pos);
			m_StepCounter = 0;
			CalculateNextEffect(surfaceInfo.GetSurfaceType(), ceilingParticleID);
		}
	}
	
	protected vector GetBonePosition(string boneName)
	{
		DayZCreature creature;
		DayZPlayerImplement player;
		
		vector ownerPos = m_Owner.GetPosition();
		if (boneName == "")
			return ownerPos;
		
		int boneIdx = -1;
		if (Class.CastTo(creature, m_Owner))
			boneIdx = creature.GetBoneIndexByName(boneName);
		if (Class.CastTo(player, m_Owner))
			boneIdx = player.GetBoneIndexByName(boneName);
		
		if (boneIdx == -1)
			return ownerPos;
		
		vector bonePos = m_Owner.GetBonePositionWS(boneIdx);
		bonePos[1] = ownerPos[1]; //equalize Y to allow for consistent height adjustment later
		return bonePos;
	}
	
	protected void AdjustBySurfaceThickness(inout vector pos, SurfaceInfo surfaceInfo)
	{
		float thicc = surfaceInfo.GetThickness(); //returns '-1' if not defined, for reasons
		if (thicc < 0)
			thicc = FLOOR_THICKNESS_DEFAULT;
		pos[1] = pos[1] - thicc;
	}
	
	protected void CreateEffects(int ceilingParticleID, vector position);
}

class DustEffectCeilingHandlerLocal : DustEffectCeilingHandlerBase
{
	override protected void CreateEffects(int ceilingParticleID, vector position)
	{
		if (DustEffectManager.GetDustEffectManager().IsCeilingDustEffectEnabled())
		{
			CeilingDustEffectBase effect = CeilingDustEffectBase.Cast(DustEffectManager.GetCeilingEffectType(ceilingParticleID).ToType().Spawn());
			effect.SetParticle(ceilingParticleID);
			effect.SetAutodestroy(true);
			SEffectManager.PlayInWorld(effect, position);
		}
	}
}

//! Generic synced solution
class DustEffectCeilingHandlerSynced : DustEffectCeilingHandlerBase
{
	//! Request from server, effecter handles the rest
	override protected void CreateEffects(int ceilingParticleID, vector position)
	{
		//'spam' variant of effecters solution
		SEffectManager.CreateParticleServer(position, new ParticleEffecterParameters("CeilingDustEffecter", 2.0, ceilingParticleID));
	}
}

///////////////////////////////
//effecter

class CeilingDustEffecter : ParticleEffecter
{
	override protected typename GetEffectTypename()
	{
		return DustEffectManager.GetCeilingEffectType(m_ParticleEffectIDSync).ToType();
	}
}

///////////////////////////////
//effect class

class CeilingDustEffectBase : EffectParticleGeneral
{
	void CeilingDustEffectBase()
	{
		Event_OnEffectStarted.Insert(PlayCeilingDustSound);
	}
	
	void ~CeilingDustEffectBase()
	{
		Event_OnEffectStarted.Remove(PlayCeilingDustSound);
	}
	
	protected void PlayCeilingDustSound()
	{
		SEffectManager.PlaySound(GetAdjustedSoundSet(m_Position), m_Position).SetAutodestroy(true);
	}
	
	override void Start()
    {
		if (!DustEffectManager.GetDustEffectManager().IsCeilingDustEffectEnabled())
			return;
		
		super.Start();
	}
	
	protected string GetAdjustedSoundSet(vector effectPos)
	{
		vector playerPos = g_Game.GetPlayer().GetPosition();
		bool upperVariant = effectPos[1] < playerPos[1];
		
		if (upperVariant)
			return GetUpperSoundsetVariant();
		return GetLowerSoundsetVariant();
	}
	
	//////////////////////////////////////////
	//methods to override in children classes
	
	protected string GetLowerSoundsetVariant()
	{
		return "";
	}
	
	protected string GetUpperSoundsetVariant()
	{
		return "";
	}
}

class CeilingDustEffectWood : CeilingDustEffectBase
{
	override protected string GetLowerSoundsetVariant()
	{
		return "CeilingDustEffect_SoundSet";
	}
	
	override protected string GetUpperSoundsetVariant()
	{
		return "CeilingDustEffectTop_SoundSet";
	}
}