class InventoryItem extends EntityAI
{	
	static private const float SOUND_CONTACT_SKIP = 0.33;//second
	
#ifdef DEVELOPER
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
	
	proto native void ThrowPhysically(DayZPlayer player, vector force);
	
	
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
		if (soundBuilder != NULL)
		{
			soundBuilder.SetVariable("weight", weight);
			soundBuilder.SetVariable("speed", velocity);
			soundBuilder.UpdateEnvSoundControllers(GetPosition());
				
			SoundObject soundObject = soundBuilder.BuildSoundObject();
			if (soundObject != NULL)
			{
				soundObject.SetKind(WaveKind.WAVEEFFECTEX);
				PlaySound(soundObject, soundBuilder);
			}
		}
	}
	
	// -------------------------------------------------------------------------------
	protected void InitImpactSoundData()
	{
		if ( GetGame().IsDedicatedServer() )
			return;

		string soundImpactType = "default";
		if ( ConfigIsExisting("soundImpactType") )
			soundImpactType = ConfigGetString("soundImpactType");
		
		m_SoundImpactTable = AnimSoundLookupTableBank.GetInstance().GetImpactTable(soundImpactType + "_Impact_LookupTable");
	}
	
	// -------------------------------------------------------------------------------
	AbstractWave PlaySound(SoundObject so, SoundObjectBuilder sob)
	{
		if (so == NULL)
		{
			//Print("PlaySound: NULL argument");
			return NULL;
		}

		so.SetPosition(GetPosition());
		AbstractWave wave = GetGame().GetSoundScene().Play3D(so, sob);
		return wave;
	}
	
	// -------------------------------------------------------------------------------
	string GetImpactSurfaceType(IEntity other, Contact impact)
	{
		string surface;
		if (DayZPhysics.GetHitSurface(
			Object.Cast(other),
			impact.Position + impact.RelativeVelocityBefore * 5,
			impact.Position - impact.RelativeVelocityBefore * 5,
			surface))
		{
			return surface;
		}
		
		int liquid;
		GetGame().SurfaceUnderObject(this, surface, liquid);
		return surface;
	}
	
	//! returns ammo (projectile) used in melee if the item is destroyed. Override higher for specific use
	string GetRuinedMeleeAmmoType()
	{
		return "MeleeSoft";
	}

	// -------------------------------------------------------------------------------
	float ProcessImpactSound(IEntity other, Contact extra, float weight, out int surfaceHash)
	{
		float impactVelocity = extra.RelativeVelocityBefore.Length();
		if ( impactVelocity < 0.3 )
			return 0.0;
		
		float tickTime = GetGame().GetTickTime();
		if ( m_SoundContactTickTime + SOUND_CONTACT_SKIP > tickTime )
			return 0.0;
		
		string surfaceName = GetImpactSurfaceType(other, extra);
		if ( surfaceName == "" )
			return 0.0;

#ifdef DEVELOPER
		string infoText = "Surface: " + surfaceName + ", Weight: " + weight + ", Speed: " + impactVelocity;
		
		if ( s_ImpactSoundsInfo.Count() == 10 )
			s_ImpactSoundsInfo.Remove(9);
		
		s_ImpactSoundsInfo.InsertAt(infoText, 0);
#endif

		m_SoundContactTickTime = tickTime;

		surfaceHash = surfaceName.Hash();
		return impactVelocity;		
	}
	
#ifdef DEVELOPER
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