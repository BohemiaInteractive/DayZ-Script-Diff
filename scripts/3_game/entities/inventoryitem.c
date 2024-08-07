class InventoryItem extends EntityAI
{	
	static private const float SOUND_CONTACT_SKIP = 0.33;//second
	
#ifdef DIAG_DEVELOPER
	static private ref array<ref string> s_ImpactSoundsInfo = new array<ref string>();
#endif

	private SoundLookupTable m_SoundImpactTable;
	private float m_SoundContactTickTime;
	private bool m_IsMeleeWeapon = false;
	
	proto native InventoryItemType GetInventoryItemType();

	//! Some inventoryItem devices can be switched on/off (radios, transmitters)
	proto native void SwitchOn(bool onOff);
	//! Some inventoryItem devices can be switched on/off (radios, transmitters)
	proto native bool IsOn();
	
	//! collisions with character
	proto native void EnableCollisionsWithCharacter(bool state);
	proto native bool HasCollisionsWithCharacter();

	proto native MeleeCombatData GetMeleeCombatData();	
	
	proto native void ThrowPhysically(DayZPlayer player, vector force, bool collideWithCharacters = true);

	//! Sets the item to use the server configured 'networkRangeFar' instead of 'networkRangeNear'
	//  This method performs an OR operation with the config 'forceFarBubble'. If set in the config 
	//  this method has no effect. 
	proto native void ForceFarBubble(bool state);
	
	void InventoryItem()
	{
		InitImpactSoundData();
		
		if (ConfigIsExisting("isMeleeWeapon"))
			m_IsMeleeWeapon = ConfigGetBool("isMeleeWeapon");
	}
	
	
	void OnRightClick()
	{
	
	}

	event bool OnUseFromInventory(Man owner)
	{
		return false;
	}

	//! Get tooltip text
	string GetTooltip()
	{
		string temp;
		if (!DescriptionOverride(temp))
			temp = ConfigGetString("descriptionShort");
		return temp;
	}

	override bool IsInventoryItem()
	{
		return true;
	}
	
	int GetMeleeMode()
	{
		return 0;
	}

	int GetMeleeHeavyMode()
	{
		return 1;
	}	
	
	int GetMeleeSprintMode()
	{
		return 2;
	}
	
	override bool IsMeleeWeapon()
	{
		return m_IsMeleeWeapon;
	}
	
	bool IsMeleeFinisher()
	{
		return false;
	}
	
	// -------------------------------------------------------------------------------
	void PlayImpactSound(float weight, float velocity, int surfaceHash)
	{
		if (!m_SoundImpactTable)
			return;

		SoundObjectBuilder soundBuilder = m_SoundImpactTable.GetSoundBuilder(surfaceHash);
		if (soundBuilder != null)
		{
			soundBuilder.AddVariable("weight", weight);
			soundBuilder.AddVariable("speed", velocity);
			soundBuilder.AddEnvSoundVariables(GetPosition());
				
			SoundObject soundObject = soundBuilder.BuildSoundObject();
			if (soundObject != null)
			{
				soundObject.SetKind(WaveKind.WAVEEFFECTEX);
				PlaySound(soundObject, soundBuilder);
			}
		}
	}
	
	// -------------------------------------------------------------------------------
	protected void InitImpactSoundData()
	{
		#ifndef SERVER
		string soundImpactType = "default";
		if ( ConfigIsExisting("soundImpactType") )
			soundImpactType = ConfigGetString("soundImpactType");
		
		m_SoundImpactTable = AnimSoundLookupTableBank.GetInstance().GetImpactTable(soundImpactType + "_Impact_LookupTable");
		#endif
	}
	
	// -------------------------------------------------------------------------------
	AbstractWave PlaySound(SoundObject so, SoundObjectBuilder sob)
	{
		if (so == null)
			return null;

		so.SetPosition(GetPosition());
		AbstractWave wave = GetGame().GetSoundScene().Play3D(so, sob);

		return wave;
	}
	
	// -------------------------------------------------------------------------------
	void PlaySoundByAnimEvent(EAnimSoundEventID id)
	{
		AnimSoundEvent soundEvent = GetInventoryItemType().GetSoundEvent(id);
		if (soundEvent)
		{
			SoundObjectBuilder builder = soundEvent.GetSoundBuilder();
			SoundObject soundObject = builder.BuildSoundObject();
			if (soundObject)
				PlaySound(soundObject, builder);
		}
	}
	
	
	// -------------------------------------------------------------------------------
	string GetImpactSurfaceType(IEntity other, Contact impact)
	{
		string surface;
		int liquid = -1;
		return GetImpactSurfaceTypeEx(other, impact, liquid);
	}
	
	// -------------------------------------------------------------------------------
	string GetImpactSurfaceTypeEx(IEntity other, Contact impact, out int liquid)
	{		
		vector mins, maxs;
		GetWorldBounds(mins, maxs);
		vector size = maxs - mins;		

		vector add = impact.RelativeVelocityBefore.Normalized() * size.Length();
		string surfaceImpact;
		if (DayZPhysics.GetHitSurfaceAndLiquid(
			Object.Cast(other),
			impact.Position + add,
			impact.Position - add,
			surfaceImpact,
			liquid))
		{
			return surfaceImpact;
		}
		string surface;
		GetGame().SurfaceUnderObjectExCorrectedLiquid(this, surface, surfaceImpact, liquid);

		return surfaceImpact;
	}
	
	//! returns ammo (projectile) used in melee if the item is destroyed. Override higher for specific use
	string GetRuinedMeleeAmmoType()
	{
		return "MeleeSoft";
	}

	// -------------------------------------------------------------------------------
	float ProcessImpactSound(IEntity other, Contact extra, float weight, out int surfaceHash)
	{
		int liquidType = -1;
		return ProcessImpactSoundEx(other, extra, weight, surfaceHash,liquidType);
	}
	
	
	// -------------------------------------------------------------------------------
	float ProcessImpactSoundEx(IEntity other, Contact extra, float weight, out int surfaceHash, out int liquidType)
	{
		float impactVelocity = extra.RelativeVelocityBefore.Length();
		if ( impactVelocity < 0.3 )
			return 0.0;
		
		float tickTime = GetGame().GetTickTime();
		if ( m_SoundContactTickTime + SOUND_CONTACT_SKIP > tickTime )
			return 0.0;
		
		string surfaceName = GetImpactSurfaceTypeEx(other, extra, liquidType);
		if ( surfaceName == "" )
			return 0.0;

#ifdef DIAG_DEVELOPER
		string infoText = "Surface: " + surfaceName + ", Weight: " + weight + ", Speed: " + impactVelocity;
		
		if ( s_ImpactSoundsInfo.Count() == 10 )
			s_ImpactSoundsInfo.Remove(9);
		
		s_ImpactSoundsInfo.InsertAt(infoText, 0);
#endif

		m_SoundContactTickTime = tickTime;

		surfaceHash = surfaceName.Hash();
		return impactVelocity;		
	}
	
#ifdef DIAG_DEVELOPER
	static void DrawImpacts()
	{
		DbgUI.Begin("Item impact sounds", 10, 200);
		
		for ( int i = 0; i < s_ImpactSoundsInfo.Count(); ++i )
		{
			string line = (i + 1).ToString() + ". " + s_ImpactSoundsInfo.Get(i);
			DbgUI.Text(line);
		}
		
		DbgUI.End();
	}
#endif
};