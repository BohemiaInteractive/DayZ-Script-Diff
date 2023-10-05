enum SoundTypeBottle
{
	POURING			= 1,
	EMPTYING		= 0,	
}

class Bottle_Base extends Edible_Base
{
	//Particles
	protected Particle 	m_ParticleCooking;
	protected int 		m_ParticlePlaying 	= ParticleList.INVALID;
	//Boiling
	//waiting for proper particle effects
	protected int PARTICLE_BOILING_EMPTY 	= ParticleList.COOKING_BOILING_EMPTY;
	protected int PARTICLE_BOILING_START 	= ParticleList.COOKING_BOILING_START;
	protected int PARTICLE_BOILING_DONE 	= ParticleList.COOKING_BOILING_DONE;
	//Baking
	protected int PARTICLE_BAKING_START 	= ParticleList.COOKING_BAKING_START;
	protected int PARTICLE_BAKING_DONE 		= ParticleList.COOKING_BAKING_DONE;
	//Drying
	protected int PARTICLE_DRYING_START 	= ParticleList.COOKING_DRYING_START;
	protected int PARTICLE_DRYING_DONE 		= ParticleList.COOKING_DRYING_DONE;
	//Burning
	protected int PARTICLE_BURNING_DONE		= ParticleList.COOKING_BURNING_DONE;
	
	//Sounds
	protected EffectSound 	m_PouringLoopSound;
	protected EffectSound 	m_EmptyingLoopSound;
	
	//cooking data
	protected CookingMethodType 	m_CookingMethod;
	protected bool 					m_CookingIsDone;
	protected bool 					m_CookingIsEmpty;
	protected bool 					m_CookingIsBurned;	
	
	//Boiling
	const string SOUND_BOILING_EMPTY 		= "Boiling_SoundSet";
	const string SOUND_BOILING_START 		= "Boiling_SoundSet";
	const string SOUND_BOILING_DONE 		= "Boiling_Done_SoundSet";
	const string SOUND_DRYING_START 		= "Drying_SoundSet";
	const string SOUND_DRYING_DONE 			= "Drying_Done_SoundSet";
	
	float m_LiquidEmptyRate;
	private const float QUANTITY_EMPTIED_PER_SEC_DEFAULT = 200; //default
		
	void Bottle_Base()
	{
		RegisterNetSyncVariableInt("m_CookingMethod", CookingMethodType.NONE, CookingMethodType.COUNT);
		RegisterNetSyncVariableBool("m_CookingIsDone");
		RegisterNetSyncVariableBool("m_CookingIsEmpty");
		RegisterNetSyncVariableBool("m_CookingIsBurned");
		
		m_LiquidEmptyRate = QUANTITY_EMPTIED_PER_SEC_DEFAULT;
	}
	
	void ~Bottle_Base()
	{
		SEffectManager.DestroyEffect(m_PouringLoopSound);
		SEffectManager.DestroyEffect(m_EmptyingLoopSound);
	}
	
	override void EEDelete( EntityAI parent )
	{
		super.EEDelete( parent );
		
		//remove audio visuals
		RemoveAudioVisuals();
	}	
	
	override void EECargoIn(EntityAI item)
	{
		super.EECargoIn(item);
		
		MiscGameplayFunctions.SoakItemInsideParentContainingLiquidAboveThreshold(ItemBase.Cast(item), this);
	}
	
	//================================================================
	// PARTICLES & SOUNDS
	//================================================================
	//Refreshes the audio and partcile effects on cooking pot
	//is_done - is the food baked, boiled, dried?
	//is_empty - is cooking quipment (cargo) empty?
	//is_burned - is any of the food items in the cargo in burned food stage?
	override void Synchronize()
	{
		SetSynchDirty();
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx) 
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		Param1<bool> p = new Param1<bool>(false);
				
		if (!ctx.Read(p))
			return;
		
		bool play = p.param1;
		switch (rpc_type)
		{
			case SoundTypeBottle.POURING:
				if (play)
					PlayPouringLoopSound();
				else
					StopPouringLoopSound();			

				break;
			
			case SoundTypeBottle.EMPTYING:
				if (play)
					PlayEmptyingLoopSound();
				else
					StopEmptyingLoopSound();
			
				break;
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (m_CookingMethod != CookingMethodType.NONE)
		{
			RefreshAudioVisuals(m_CookingMethod, m_CookingIsDone, m_CookingIsEmpty, m_CookingIsBurned);
		}
		else
		{
			RemoveAudioVisuals();
		}
	}
	
	void RemoveAudioVisualsOnClient()
	{
		m_CookingMethod	= CookingMethodType.NONE;
		
		Synchronize();
	}
	
	void RefreshAudioVisualsOnClient( CookingMethodType cooking_method, bool is_done, bool is_empty, bool is_burned )
	{
		m_CookingMethod		= cooking_method;
		m_CookingIsDone 	= is_done;
		m_CookingIsEmpty	= is_empty;
		m_CookingIsBurned	= is_burned;

		Synchronize();
	}
	
	void RefreshAudioVisuals(CookingMethodType cooking_method, bool is_done, bool is_empty, bool is_burned)
	{
		string soundName;
		int particleId;
		
		//if at least one of the food items is burned
		if (is_burned)
		{
			soundName 	= SOUND_BURNING_DONE;
			particleId 	= PARTICLE_BURNING_DONE;
		}
		//proper cooking methods
		else
		{
			switch (cooking_method)
			{
				case CookingMethodType.BOILING:
					if (is_empty)
					{
						soundName 	= SOUND_BOILING_EMPTY;
						particleId 	= PARTICLE_BOILING_EMPTY;
					}
					else
					{
						if (is_done)
						{
							soundName 	= SOUND_BOILING_DONE;
							particleId 	= PARTICLE_BOILING_DONE;
						}
						else
						{
							soundName 	= SOUND_BOILING_START;
							particleId 	= PARTICLE_BOILING_START;
						}
					}

					break;

				case CookingMethodType.BAKING:
					if (is_done)
					{
						soundName 	= SOUND_BAKING_DONE;
						particleId 	= PARTICLE_BAKING_DONE;
					}
					else
					{
						soundName 	= SOUND_BAKING_START;
						particleId 	= PARTICLE_BAKING_START	;
					}

					break;

				case CookingMethodType.DRYING:
					if (is_done)
					{
						soundName 	= SOUND_DRYING_DONE;
						particleId 	= PARTICLE_DRYING_DONE;
					}
					else
					{
						soundName 	= SOUND_DRYING_START;
						particleId 	= PARTICLE_DRYING_START;
					}
				
					break;
				
				default:
					soundName = "";
					particleId = ParticleList.NONE;

					break;
			}
		}
		
		//play effects
		ParticleCookingStart(particleId);
		SoundCookingStart(soundName);
	}
	
	void RemoveAudioVisuals()
	{
		ParticleCookingStop();
		SoundCookingStop();
	}
	
	//particles
	void ParticleCookingStart(int particle_id)
	{
		#ifndef SERVER
		if (m_ParticlePlaying != particle_id)
		{
			//stop previous particles
			ParticleCookingStop();
			
			//create new
			vector localPos = MiscGameplayFunctions.GetSteamPosition(GetHierarchyParent());
			m_ParticleCooking = ParticleManager.GetInstance().PlayInWorld(particle_id, localPos);
			m_ParticlePlaying = particle_id;
		
		}
		#endif
	}
	
	void ParticleCookingStop()
	{
		if (m_ParticleCooking && GetGame() && !GetGame().IsDedicatedServer())
		{
			m_ParticleCooking.Stop();
			m_ParticleCooking = null;
			m_ParticlePlaying = ParticleList.INVALID;
		}
	}
		
	void PlayPouringLoopSound()
	{
		if (!m_PouringLoopSound || !m_PouringLoopSound.IsSoundPlaying())
		{
			m_PouringLoopSound = SEffectManager.PlaySoundOnObject(GetPouringSoundset(), this, 0, 0, true);
		}
	}
	
	void StopPouringLoopSound()
	{
		if (m_PouringLoopSound)
			m_PouringLoopSound.SoundStop();
	}
		
	void PlayEmptyingLoopSound()
	{
		if (!m_EmptyingLoopSound || !m_EmptyingLoopSound.IsSoundPlaying())
		{
			m_EmptyingLoopSound = SEffectManager.PlaySoundOnObject(GetEmptyingLoopSoundset(), this, 0, 0, true);
		}
	}
	
	void StopEmptyingLoopSound()
	{
		if (m_EmptyingLoopSound)
			m_EmptyingLoopSound.SoundStop();
				
		EffectSound sound =	SEffectManager.PlaySoundOnObject(GetEmptyingEndSoundset(), this);
		sound.SetAutodestroy(true);
	}
		
	string GetEmptyingLoopSoundset()
	{		
		vector pos = GetPosition();
		string surfaceType = GetGame().GetPlayer().GetSurfaceType();
		string soundSet = "";
		
		bool diggable = GetGame().IsSurfaceDigable(surfaceType);
		
		if (!diggable)
		{
			soundSet = GetEmptyingLoopSoundsetHard();
		}
		else if (diggable)
		{
			soundSet = GetEmptyingLoopSoundsetSoft();
		}
		else if (GetGame().SurfaceIsPond(pos[0], pos[2]) || GetGame().SurfaceIsSea(pos[0], pos[2]))
		{
			soundSet = GetEmptyingLoopSoundsetWater();
		}
		
		return soundSet;
	}
	
	string GetEmptyingEndSoundset()
	{		
		vector pos = GetPosition();
		string surfaceType = GetGame().GetPlayer().GetSurfaceType();
		string soundSet = "";
		
		bool diggable = GetGame().IsSurfaceDigable(surfaceType);
		
		if (!diggable)
		{
			soundSet = GetEmptyingEndSoundsetHard();
		}
		else if (diggable)
		{
			soundSet = GetEmptyingEndSoundsetSoft();
		}
		else if (GetGame().SurfaceIsPond(pos[0], pos[2]) || GetGame().SurfaceIsSea(pos[0], pos[2]))
		{
			soundSet = GetEmptyingEndSoundsetWater();
		}
		
		return soundSet;
	}

	string GetPouringSoundset();
	string GetEmptyingLoopSoundsetHard();
	string GetEmptyingLoopSoundsetSoft();
	string GetEmptyingLoopSoundsetWater();
	string GetEmptyingEndSoundsetHard();
	string GetEmptyingEndSoundsetSoft();
	string GetEmptyingEndSoundsetWater();
	
	//! Returns base liquid empty rate (absolute)..preferrably use the 'GetLiquidThroughputCoef' instead
	float GetLiquidEmptyRate()
	{
		return m_LiquidEmptyRate;
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionFillFuel);
		AddAction(ActionFillCoolant);
		AddAction(ActionFillGeneratorTank);
		AddAction(ActionExtinguishFireplaceByLiquid);
		AddAction(ActionFillBottleBase);
		AddAction(ActionWaterGardenSlot);
		AddAction(ActionWaterPlant);
		AddAction(ActionForceDrink);
		AddAction(ActionDrainLiquid);
		AddAction(ActionPourLiquid);
		AddAction(ActionWashHandsItemContinuous);
		AddAction(ActionDrink);
		AddAction(ActionEmptyBottleBase);
	}
	
	override void OnDebugSpawn()
	{
		SetQuantityMax();
	}
}
