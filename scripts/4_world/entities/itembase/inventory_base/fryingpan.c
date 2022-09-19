class FryingPan extends Inventory_Base
{
	// Cooking data
	protected CookingMethodType 		m_CookingMethod;
	protected bool 						m_CookingIsDone;
	protected bool 						m_CookingIsEmpty;
	protected bool 						m_CookingIsBurned;

	// Particles
	protected Particle 	m_ParticleCooking;
	protected int 		m_ParticlePlaying 	= ParticleList.INVALID;
	// Baking
	protected int PARTICLE_BAKING_START 	= ParticleList.COOKING_BAKING_START;
	protected int PARTICLE_BAKING_DONE 		= ParticleList.COOKING_BAKING_DONE;
	// Drying
	protected int PARTICLE_DRYING_START 	= ParticleList.COOKING_DRYING_START;
	protected int PARTICLE_DRYING_DONE 		= ParticleList.COOKING_DRYING_DONE;
	// Burning
	protected int PARTICLE_BURNING_DONE		= ParticleList.COOKING_BURNING_DONE;

	// Sounds
	protected SoundOnVehicle	m_SoundCooking;
	protected string			m_SoundPlaying = "";
	// Baking
	const string SOUND_BAKING_START 	= "bake";
	const string SOUND_BAKING_DONE 		= "bakeDone";
	// Drying
	const string SOUND_DRYING_START 	= "dry";
	const string SOUND_DRYING_DONE 		= "dryDone";	
	// Burning
	const string SOUND_BURNING_DONE 	= "burned";

	void FryingPan()
	{
		RegisterNetSyncVariableInt( "m_CookingMethod", CookingMethodType.NONE, CookingMethodType.COUNT );
		RegisterNetSyncVariableBool( "m_CookingIsDone" );
		RegisterNetSyncVariableBool( "m_CookingIsEmpty" );
		RegisterNetSyncVariableBool( "m_CookingIsBurned" );
	}
	void ~FryingPan() {}

	override bool IsContainer()
	{
		return true;		
	}
	
	override bool CanHaveTemperature()
	{
		return true;
	}

	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo( parent ) )
			return false;
		
		if ( parent && IsCargoException4x3( parent ) )
			return false;

		return true;
	}

	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if ( !super.CanReceiveItemIntoCargo( item ) )
			return false;

		if ( IsCargoException4x3( item ) )
			return false;

		return true;
	}
	
	override bool CanLoadItemIntoCargo( EntityAI item )
	{
		if ( !super.CanLoadItemIntoCargo( item ) )
			return false;

		if ( IsCargoException4x3( item ) )
			return false;

		return true;
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}

	override void EEDelete( EntityAI parent )
	{
		super.EEDelete( parent );
		
		//remove audio visuals
		RemoveAudioVisuals();
	}

	void Synchronize()
	{
		if ( GetGame() && GetGame().IsServer() )
		{
			SetSynchDirty();
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		//refresh audio visuals
		if ( m_CookingMethod != CookingMethodType.NONE )
		{
			RefreshAudioVisuals( m_CookingMethod, m_CookingIsDone, m_CookingIsEmpty, m_CookingIsBurned );
		}
		else
		{
			RemoveAudioVisuals();
		}
	}

	void RemoveAudioVisualsOnClient()
	{
		m_CookingMethod	= CookingMethodType.NONE;
		
		//synchronize
		Synchronize();
	}

	void RefreshAudioVisualsOnClient( CookingMethodType cooking_method, bool is_done, bool is_empty, bool is_burned )
	{
		m_CookingMethod		= cooking_method;
		m_CookingIsDone 	= is_done;
		m_CookingIsEmpty	= is_empty;
		m_CookingIsBurned	= is_burned;
		
		//synchronize
		Synchronize();
	}

	void RefreshAudioVisuals( CookingMethodType cooking_method, bool is_done, bool is_empty, bool is_burned )
	{
		string sound_name;
		int particle_id;
		
		//if at least one of the food items is burned
		if ( is_burned )		
		{
			sound_name 		= SOUND_BURNING_DONE;
			particle_id 	= PARTICLE_BURNING_DONE;
		}
		//proper cooking methods
		else
		{
			if ( cooking_method == CookingMethodType.BAKING )
			{
				if ( is_done )											//DONE
				{
					sound_name 		= SOUND_BAKING_DONE;
					particle_id 	= PARTICLE_BAKING_DONE;
				}
				else													//START
				{
					sound_name 		= SOUND_BAKING_START;
					particle_id 	= PARTICLE_BAKING_START;
				}
			}
			else if ( cooking_method == CookingMethodType.DRYING )
			{
				if ( is_done )											//DONE
				{
					sound_name 		= SOUND_DRYING_DONE;
					particle_id 	= PARTICLE_DRYING_DONE;
				}
				else													//START
				{
					sound_name 		= SOUND_DRYING_START;
					particle_id 	= PARTICLE_DRYING_START;
				}
			}			
		}
		
		//play effects
		ParticleCookingStart( particle_id );
		SoundCookingStart( sound_name );
	}
	
	void RemoveAudioVisuals()
	{
		ParticleCookingStop();
		SoundCookingStop();
	}

	void ParticleCookingStart( int particle_id )
	{
		if ( m_ParticlePlaying != particle_id )
		{
			//stop previous particles
			ParticleCookingStop();
			
			//create new
			if ( GetGame() && ( !GetGame().IsDedicatedServer() ) )
			{
				vector local_pos = MiscGameplayFunctions.GetSteamPosition( GetHierarchyParent() );
				//TODO set steam position to pot (proxy) memory point (new hierarchy needed)
				//m_ParticleCooking = Particle.Create( particle_id, this, local_pos );
				m_ParticleCooking = ParticleManager.GetInstance().PlayInWorld( particle_id, local_pos );
				m_ParticlePlaying = particle_id;
			}
		}
	}
	void ParticleCookingStop()
	{
		if ( m_ParticleCooking && GetGame() && ( !GetGame().IsDedicatedServer() ) )
		{
			m_ParticleCooking.Stop();
			m_ParticleCooking = NULL;
			m_ParticlePlaying = ParticleList.INVALID;
		}
	}

	protected void SoundCookingStart( string sound_name )
	{
		if ( GetGame() && ( !GetGame().IsDedicatedServer() ) )
		{	
			if ( m_SoundPlaying != sound_name )
			{
				//stop previous sound
				SoundCookingStop();
				
				//create new
				m_SoundCooking = PlaySoundLoop( sound_name, 50 );
				m_SoundPlaying = sound_name;
			}
		}
	}
	protected void SoundCookingStop()
	{
		if ( m_SoundCooking )
		{
			GetGame().ObjectDelete( m_SoundCooking );
			m_SoundCooking = NULL;
			m_SoundPlaying = "";
		}
	}	
}