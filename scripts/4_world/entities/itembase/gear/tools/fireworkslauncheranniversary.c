class AnniversaryFireworksLauncherClientEvent: FireworksLauncherClientEvent
{
	override protected int GetExplParticleFromSequence()
	{
		switch (m_Color)
		{
			case "1":
				return ParticleList.FIREWORKS_EXPLOSION_THANKS1;
			case "2":
				return ParticleList.FIREWORKS_EXPLOSION_THANKS2;
			case "3":
				return ParticleList.FIREWORKS_EXPLOSION_THANKS3;
			case "4":
				return ParticleList.FIREWORKS_EXPLOSION_THANKS4;
			case "5":
				return ParticleList.FIREWORKS_EXPLOSION_THANKS5;
			default:
				ErrorEx("Incorrect explosion particle color in the sequence");
		}
		return ParticleList.FIREWORKS_EXPLOSION_RED;
	}
	
	override protected void SetupLight(PointLightBase light)
	{
		switch (m_Color)
		{
			case "1":
				light.SetDiffuseColor(255,51,51);
				light.SetAmbientColor(255,51,51);
				
				break;
			case "2":
				light.SetDiffuseColor(0,255,128);
				light.SetAmbientColor(0,255,128);
				break;
			case "3":
				light.SetDiffuseColor(51,153,255);
				light.SetAmbientColor(51,153,255);
				break;
			case "4":
				light.SetDiffuseColor(255,255,51);
				light.SetAmbientColor(255,255,51);
				break;
			case "5":
				light.SetDiffuseColor(255,102,255);
				light.SetAmbientColor(255,102,255);
				break;
			default:
				ErrorEx("Incorrect explosion particle color in the sequence");
		}
	}
	
	override protected void SpawnSecondaryExplosion()
	{
		AnniversaryFireworksLauncherClientEventSecondary evnt = new AnniversaryFireworksLauncherClientEventSecondary(m_Item,m_Index);
		evnt.Init(GetExplosionPosition());
		evnt.OnExplode();
		m_Events.Insert(evnt);
		RequestSecondaryExplosion();
	}
}
//--------------------------------------------------------------------------------------
class AnniversaryFireworksLauncherClientEventSecondary : AnniversaryFireworksLauncherClientEvent
{
	protected vector m_ShotPos;
	
	override protected vector GetExplosionPosition()
	{
		return GetShotPos() + m_ShotDir * GetExplosionDistance();
	}
	
	void Init(vector pos)
	{
		m_ShotPos = pos;
		m_ShotDir[0] = Math.RandomFloatInclusive(-1,1);
		m_ShotDir[1] = Math.RandomFloatInclusive(-1,1);
		m_ShotDir[2] = Math.RandomFloatInclusive(-1,1);
		m_ShotDir.Normalize();
	}
	override protected vector GetShotPos()
	{
		return m_ShotPos;
	}
	
	override protected float GetExplosionDistance()
	{
		return Math.RandomFloatInclusive(10,15);
	}
	
	override void OnExplode()
	{
		g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( PlayExplosionSound, GetSoundDelay(), false);
		g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( SpawnLight, GetLightDelay(), false);
		m_ParticleExplosion = ParticleManager.GetInstance().PlayInWorld(GetExplParticleFromSequence(), GetExplosionPosition());
	}
}
//--------------------------------------------------------------------------------------
class Anniversary_FireworksLauncher: FireworksLauncher
{
	int sequence = 0;
	
	override protected void SetupColorSequences()
	{
		m_ColorSequence.Insert("1234512345123451234512345");
	}
	
	override protected FireworksLauncherClientEventBase SpawnEvent()
	{
		FireworksLauncherClientEventBase evnt = new AnniversaryFireworksLauncherClientEvent(this,m_Index);
		evnt.OnFired();
		return evnt;
	}
	
	override bool IsTakeable()
	{
		return false;
	}
	
	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		return false;
	}
	
	override bool DisableVicinityIcon()
	{
		return true;
	}
	
	override protected int GetMaxShots()
	{
		return 25;
	}
	
	override protected float GetFuseDelay()
	{
		return 1;
	}
	
		//!Called periodically but only after the entity gets ignited
	override protected void OnEventServer(int type)
	{
		m_Index++;
		DamageSystem.ExplosionDamage(this, NULL, GetAmmoType(), GetPosition(), GetDamageType());
		
		if(m_Index > 16)
		{
			sequence++;
			m_Index = 1;
		}
		
		SetSynchDirty();
		if (m_Index + (sequence*16)> GetMaxShots())
		{
			m_Index = GetMaxShots();
			m_TimerEvent = null;
			SetState(EFireworksState.FINISHED);
		}
		else
		{
			RestartEventTimer();
		}
	}
	
}