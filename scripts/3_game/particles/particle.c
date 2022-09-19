/**
\brief Legacy way of using particles in the game
*	\note They work okay when just needing to play a particle once every once in a while
*		  But are extremely wasteful when it comes to playing multiple Particles at the same time
*/
class Particle : ParticleBase
{
	/** \name Generic data
		Generic data for the Particle
	*/
	//@{
	//! ID from ParticleList if assigned
	protected 	int		m_ParticleID;
	//! Approx. remaining lifetime of particle
	protected 	float	m_Lifetime;
	//! Whether this particle repeats
	protected 	bool 	m_IsRepeat;
	//! Whether this particle is queued for deletion
	private 	bool 	m_MarkedForDeletion;
	//@}
	
	/** \name Wiggle API
		Data for the wiggle API
	*/
	//@{
	//! Used for Wiggle API, to signal that wiggle API is currently doing work
	bool 				m_WiggleProcessing;
	//! Used for Wiggle API, to restore after unparenting
	bool 				m_ForceOrientationRelativeToWorld;
	//! Used for Wiggle API, to restore after unparenting
	vector 				m_DefaultOri;
	//! Used for Wiggle API, to restore after unparenting
	vector 				m_DefaultPos;
	//! Used for Wiggle API, to restore after unparenting
	vector 				m_DefaultWorldOri;
	//! Used for Wiggle API, to restore after unparenting
	vector 				m_DefaultWorldPos;
	
	//! Used for Wiggle API, Wiggle room [-m_MaxOriWiggle, m_MaxOriWiggle]
	float 				m_MaxOriWiggle;
	//! Used for Wiggle API, Interval for wiggling [0, m_MaxOriInterval[
	float 				m_MaxOriInterval;
	//! Used for Wiggle API, calls the Wiggle functionality
	ref Timer 			m_RandomizeOri;
	//@}
	
	//! Parent Object the Particle is child of
	protected Object 	m_ParentObject;
	//! The child object which contains the actual particle
	protected Object 	m_ParticleEffect;
	
	//! DEPRECATED
	protected int		m_PreviousFrame;
	//! DEPRECATED
	private vector 		m_GlobalPosPreviousFrame;
	//! DEPRECATED	
	static private const int MAX_EMITORS = 30;
	
	
	//! ctor
	void Particle()
	{
		ParticleInit();
	}
	
	//! Purely here so that it can be emptied in ParticleSource
	protected void ParticleInit()
	{
		SetFlags(EntityFlags.VISIBLE, true);
		SetEventMask(EntityEvent.INIT);
		SetEventMask(EntityEvent.FRAME);
	}	
	
	/** \name Create a particle (static)
		You can create a particle either at some position, or create it as a child on some object.
	*/
	//@{
	
	/**
	\brief Creates a particle emitter and attaches it on the given object
		\param particle_id \p int Particle ID registered in ParticleList
		\param parent_obj \p Object Instance on which this particle will be attached
		\param local_pos \p vector Attachment position local to the parent (Optional)
		\param local_ori \p vector Orientation local to the parent (Pitch, Yawn, Roll in degrees) (Optional)
		\param force_world_rotation \p bool Forces particle's orientation to rotate relative to the world and not with the object (Optional)
		\return \p Particle Created particle instance
	*/
	static Particle CreateOnObject( int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0", bool force_world_rotation = false )
	{
		if (!parent_obj)
			Error("ERROR when creating a particle! Parameter parent_obj is NULL!");
		
		vector global_pos = parent_obj.GetPosition();
		Particle p = CreateInWorld(particle_id, global_pos, Vector(0,0,0), force_world_rotation);
		p.AddAsChild(parent_obj, local_pos, local_ori, force_world_rotation);
		p.m_DefaultOri = local_ori;
		
		return p;
	}
	
	/**
	\brief Legacy function for backwards compatibility
	*/
	static Particle Create( int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0" )
	{
		return CreateOnObject( particle_id, parent_obj, local_pos, local_ori);
	}
	
	/**
	\brief Creates a particle emitter on the given position
		\param particle_id \p int Particle ID registered in ParticleList
		\param global_pos \p Vector Position where the particel will be created
		\param global_ori \p vector Orientation (Pitch, Yawn, Roll in degrees) (Optional)
		\param force_world_rotation \p bool Forces particle's orientation to rotate relative to the world and not with the object (Optional)
		\return \p Particle Created particle instance
	*/
	static Particle CreateInWorld( int particle_id, vector global_pos, vector global_ori = "0 0 0", bool force_world_rotation = false )
	{
		Particle p = Particle.Cast( GetGame().CreateObjectEx("Particle", global_pos, ECE_LOCAL) );
		p.SetSource(particle_id);
		p.SetOrientation(global_ori);
		p.m_ForceOrientationRelativeToWorld = force_world_rotation;
		return p;
	}
	
	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
	*/
	static Particle Create( int particle_id, vector global_pos, vector global_ori = "0 0 0" )
	{
		return CreateInWorld( particle_id, global_pos, global_ori );
	}
	
	//@}
	
	
	
	/** \name Static play on creation
		You can use the following Play(...) functions to create and activate a particle in 1 line of your script.
	*/
	//@{
	
	/**
	\brief Creates a particle emitter, attaches it on the given object and activates it
		\param particle_id \p int Particle ID registered in ParticleList
		\param parent_obj \p Object Instance on which this particle will be attached
		\param local_pos \p vector Attachment position local to the parent (Optional)
		\param local_ori \p vector Orientation local to the parent (Pitch, Yaw, Roll in degrees) (Optional)
		\param force_world_rotation \p bool Forces particle's orientation to rotate relative to the world and not with the object (Optional)
		\return \p Particle Created particle instance
	*/
	static Particle PlayOnObject( int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0", bool force_world_rotation = false )
	{
		Particle p = CreateOnObject(particle_id, parent_obj, local_pos, local_ori, force_world_rotation);
		p.PlayParticle();
		
		return p;
	}
	
	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
	*/
	static Particle Play( int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0" )
	{
		return PlayOnObject( particle_id, parent_obj, local_pos, local_ori);
	}
	
	/**
	\brief Creates a particle emitter on the given position and activates it
		\param particle_id \p int Particle ID registered in ParticleList
		\param global_pos \p Vector Position where the particel will be created
		\return \p Particle Created particle instance
	*/
	static Particle PlayInWorld( int particle_id, vector global_pos)
	{
		Particle p = CreateInWorld(particle_id, global_pos);
		p.PlayParticle();
		
		return p;
	}

	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
	*/
	static Particle Play( int particle_id, vector global_pos)
	{
		return PlayInWorld( particle_id, global_pos);
	}
	
	//@}
	
	
	
	/** \name Playback
 		Methods regarding playing/stopping of particle
	*/
	//@{
	
	/**
	\brief Method to tell the particle to start playing
		\param particle_id \p int Particle ID registered in ParticleList to start playing
	*/
	override void PlayParticle(int particle_id = -1)
	{
		PlayParticleEx(particle_id, 0);
	}
	
	/**
	\brief Method to tell the particle to start playing
		\note The parameter to set the ID will only work when the particle is not already playing
		\param particle_id \p int Particle ID registered in ParticleList to start playing
		\param flags \p int Flags to pass to the playing (None on this level)
		\return \p bool Whether the particle successfully started
	*/
	override bool PlayParticleEx(int particle_id = -1, int flags = 0)
	{
		if ( particle_id > -1 )
		{
			SetSource(particle_id);
		}
		
		OnParticleStart();

		UpdateState();
		
		return true;
	}
	
	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
		\param particle_id \p int Particle ID registered in ParticleList to start playing
	*/
	void Play(int particle_id = -1)
	{
		PlayParticle(particle_id);
	}
	
	/**
	\brief Method to tell the particle to stop playing
		\note No flags available for Particle
		\note Emitors are automatically removed later when its particle count is 0
		\param flags \p int Flags to pass to the stopping (None on this level)
		\return \p bool Whether the particle successfully stopped
	*/
	override bool StopParticle(int flags = 0)
	{
		OnParticleStop();		
		
		// Without the following we might get an error when a particle parent is despawned client-side.
		Object parent = Object.Cast( GetParent() );
		if ( parent && !ToDelete())
		{
			vector world_pos = GetPosition();
			parent.RemoveChild(this);
			SetPosition(world_pos);
		}
		
		UpdateState();
		
		return true;
	}
	
	/**
	\brief Legacy function for backwards compatibility with 1.14 and below
	*/
	void Stop()
	{
		StopParticle();
	}
	
	//@}
	
	
	
	/** \name Properties and state
		Obtain information or set properties regarding the state of the Particle
	*/
	//@{
	
	/**
	\brief Sets particle id
		\note Does not work at runtime, particle object needs to be destroyed and then Particle needs to play again
		\param particle_id \p int Particle ID registered in ParticleList to start playing
	*/
	void SetSource(int particle_id)
	{
		m_ParticleID = particle_id;
	}
	
	/**
	\brief Gets particle id
		\note This is not necessarily the CURRENT particle
			* As one can use SetSource while the Particle is still playing
			* But that will not change the particle before Particle is played again
		\return \p int The last set Particle ID registered in ParticleList
	*/
	int GetParticleID()
	{
		return m_ParticleID;
	}
	
	/**
	\brief Returns direct particle effect entity which is usually handled by this class 'Particle' if there is one
		\note Is a child of this Particle
		\return \p Object The Object with the particle component or null
	*/
	Object GetDirectParticleEffect()
	{
		return m_ParticleEffect;
	}
	
	/**
	\brief Returns the parent of this Particle if there is one
		\return \p Object The registered parent or null
	*/
	Object GetParticleParent()
	{
		return m_ParentObject;
	}
	
	/**
	\brief Returns if there is any particle active
		\return \p bool Whether there is any particle active
	*/
	bool HasActiveParticle()
	{
		if (m_ParticleEffect)
		{
			return m_ParticleEffect.HasActiveParticle(m_ParticleEffect);
		}
		
		return false;
	}
	
	/**
	\brief Returns the total count of active particles in all emitors
		\note Internally does a sum, HasActiveParticle is better for a quick check
		\return \p int Total count of active particles
	*/
	int GetParticleCount()
	{
		if (m_ParticleEffect)
		{
			return m_ParticleEffect.GetParticleCount(m_ParticleEffect);
		}
		
		return 0;
	}
	
	/**
	\brief Returns whether there is a repeating particle
		\return \p bool whether there is a repeating particle
	*/
	bool IsRepeat()
	{
		if (m_ParticleEffect)
		{
			bool repeat = false;
			
			int emitors = GetParticleEmitorCount(m_ParticleEffect);
			
			for (int i = 0; i < emitors; ++i)
			{
				GetParticleParm(m_ParticleEffect, i, EmitorParam.REPEAT, repeat);
				
				if (repeat)
				{
					return true;
				}
			}
		}
		
		return false;
	}
	
	/**
	\brief Returns the approx. max lifetime
		\return \p float The largest lifetime sum among the emitors
	*/
	float GetMaxLifetime()
	{
		float lifetime_return = 0;
		
		if (m_ParticleEffect)
		{
			float lifetime_min = 0;
			float lifetime_random = 0;			
			float effect_time = 0;
			
			float lifetime_sum = 0;
			
			int emitors = GetParticleEmitorCount(m_ParticleEffect);
			
			for (int i = 0; i < emitors; ++i)
			{
				GetParticleParm(m_ParticleEffect, i, EmitorParam.LIFETIME, lifetime_min);
				GetParticleParm(m_ParticleEffect, i, EmitorParam.LIFETIME_RND, lifetime_random);
				GetParticleParm(m_ParticleEffect, i, EmitorParam.EFFECT_TIME, effect_time);
				
				lifetime_sum = lifetime_min + lifetime_random + effect_time;
				
				if ( lifetime_sum > lifetime_return )
				{
					lifetime_return = lifetime_sum;
				}
			}
		}
		
		return lifetime_return;
	}
	
	//@}
	
	
	
	/** \name Misc Particle specific helpers
		Functionality specific for Particle
	*/
	//@{
	
	/**
	\brief Creates/Destroys ParticleEffect child according to current state
		\note Is called from Play/Stop methods
	*/
	protected void UpdateState()
	{
		if ( m_IsPlaying == false && m_ParticleEffect)
		{
			DestroyParticleEffect();
		}
		else if ( m_IsPlaying == true && m_ParticleEffect == null )
		{
			CreateParticleEffect();
		}
	}
	
	/**
	\brief Creates ParticleEffect child, called from UpdateState
	*/
	private void CreateParticleEffect()
	{
		if ( !GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			string fullPath = ParticleList.GetParticleFullPath(m_ParticleID);
			if (fullPath == "")
			{
				ErrorEx("Could not play Particle as there is no valid particle id assigned.");
				m_IsPlaying = false;
				return;
			}
			
			if ( m_ParticleEffect == null )
			{
				m_ParticleEffect = GetGame().CreateObjectEx("#particlesourceenf", vector.Zero, ECE_LOCAL); // particle source must be lowercase!
			}
			
			AddChild(m_ParticleEffect, -1, m_ForceOrientationRelativeToWorld);
			
			vobject vobj = GetObject( fullPath );			
			m_ParticleEffect.SetObject(vobj, "");
			ReleaseObject(vobj);
			
			m_IsRepeat = IsRepeat();
			m_Lifetime = GetMaxLifetime();
		}
	}
	
	/**
	\brief Destroys ParticleEffect child, called from UpdateState
		\note Does not destroy it immediately
			* As it simply nulls the lifetime
			* Then it will be cleaned up by EOnFrame eventually
	*/
	private void DestroyParticleEffect()
	{
		if ( m_ParticleEffect && GetGame() )
		{
			SetParameter(-1, EmitorParam.LIFETIME, 0);
			SetParameter(-1, EmitorParam.LIFETIME_RND, 0);
			SetParameter(-1, EmitorParam.REPEAT, 0);
			
			m_IsRepeat = false;
		}
	}
	
	/**
	\brief OnFrame update event decrementing the stored approx. lifetime and checking for deletion
	*/
	override void EOnFrame(IEntity other, float timeSlice)
	{		
		m_Lifetime -= timeSlice;
		OnCheckAutoDelete();
	}
	
	/**
	\brief Creates ParticleEffect child, called from UpdateState
	*/
	void OnCheckAutoDelete()
	{
		if (m_Lifetime <= 0)
		{
			if (!m_MarkedForDeletion)
			{
				m_IsRepeat = IsRepeat(); // It is possible that the REPEAT flag was changed during lifetime, so it needs to be checked again.
				
				if ( m_IsRepeat )
				{
					m_Lifetime = GetMaxLifetime();
				}
				else
				{
					OnParticleStop();
					
					if ( GetParticleCount() == 0 )
					{
						m_MarkedForDeletion = true;
						OnToDelete();
						OnParticleEnd();
					}
				}
			}
			else
			{
				if ( m_MarkedForDeletion )
				{
					if (m_ParticleEffect)
					{
						m_ParticleEffect.Delete();
						m_ParticleEffect = null;
					}
					
					Delete();
				}
			}
		}
	}
	
	/**
	\brief Called before deletion from OnCheckAutoDelete
	*/
	private void OnToDelete()
	{

	}
	
	//@}
	
	
	
	/** \name Misc
 		Various helpers
	*/
	//@{
	
	/**
	\brief Attaches this particle onto some object. If null value is provided then the particle will be detached from the current parent.
		\note Due to the members being filled in, AddChild/RemoveChild cannot be used with Particle when using Wiggle
		\param parent \p Object Parent onto which this particle will be attached
		\param local_pos \p vector Attachment position local to the parent (optional)
		\param local_ori \p vector Orientation local to the parent (Pitch, Yawn, Roll in degrees) (Optional)
		\param force_rotation_to_world \p bool Force rotation to be in WS (Optional)
	*/
	void AddAsChild(Object parent, vector local_pos = "0 0 0", vector local_ori = "0 0 0", bool force_rotation_to_world = false)
	{
		if (ToDelete())
			return;
		
		if (parent)
		{
			// AddAsChild method is sometimes called from a timer.
			// Due to that it is necesarry to use ToDelete() here to check if the parent object is flagged for deletion or not on client,
			// because sometimes this code is executed before the parent's destructor from where this would normally be handled.
			if (!parent.ToDelete())
			{
				SetPosition(local_pos);
				SetOrientation(local_ori);
				m_ParentObject = parent;
				m_DefaultPos = local_pos;
				m_ForceOrientationRelativeToWorld = force_rotation_to_world;
				
				if (m_ParticleEffect)
					AddChild(m_ParticleEffect, -1, m_ForceOrientationRelativeToWorld);
				
				parent.AddChild(this, -1, false);
			}
		}
		else
		{
			if (m_ParentObject && !m_ParentObject.ToDelete())
			{
				m_ParentObject.RemoveChild(this, true);
				m_ParentObject = null;
			}
		}
	}
	
	//@}
	
	
	
	/** \name Parameter API
 		Helpful methods for getting or setting parameters
	*/
	//@{
	
	/**
	\brief Set the value of a parameter of all emitors in the particle
		\param parameter \p int The parameter to apply the new value to (enum EmitorParam)
		\param value \p float The value to apply
	*/
	void SetParticleParam(int parameter_id, float value )
	{
		if (!m_ParticleEffect)
			return;
		
		SetParticleParm(m_ParticleEffect, -1, parameter_id, value);
	}
	
	/**
	\brief Set the value of a parameter of an emitor in the particle
		\param emitter \p int The emitter to apply the new value to, -1 for all emitter
		\param parameter \p int The parameter to apply the new value to (enum EmitorParam)
		\param value \p float The value to apply
	*/
	void SetParameter(int emitter, int parameter, float value)
	{
		if (!m_ParticleEffect)
			return;
		
		SetParticleParm(m_ParticleEffect, emitter, parameter, value);
	}
	
	/**
	\brief Get the value of a parameter of an emitor in the particle
		\param emitter \p int The emitor to get the value from
		\param parameter \p int The parameter to get the value from (enum EmitorParam)
		\param value \p float The value
	*/
	void GetParameter(int emitter, int parameter, out float value)
	{
		if (!m_ParticleEffect)
			return;
		
		GetParticleParm(m_ParticleEffect, emitter, parameter, value);
	}
	
	/**
	\brief Get the value of a parameter of an emitor in the particle
		\param emitter \p int The emitor to get the value from
		\param parameter \p int The parameter to get the value from (enum EmitorParam)
		\return \p float The value
	*/
	float GetParameterEx(int emitter, int parameter)
	{
		if (!m_ParticleEffect)
			return 0;
		
		float value;
		GetParticleParm(m_ParticleEffect, emitter, parameter, value);
		return value;
	}
	
	/**
	\brief Scales the given parameter on all emitors relatively to their ORIGINAL value.
		\param parameter_id \p int The parameter to adjust (enum EmitorParam)
		\param coef \p float The multiplier to apply
	*/
	void ScaleParticleParamFromOriginal(int parameter_id, float coef )
	{
		if (!m_ParticleEffect)
			return;
		
		int emitors = GetParticleEmitorCount(m_ParticleEffect);			
		for (int i = 0; i < emitors; ++i)
		{
			float value;
			GetParticleParmOriginal(m_ParticleEffect, i, parameter_id, value);
			SetParticleParm(m_ParticleEffect, i, parameter_id, value * coef);
		}
	}
	
	/**
	\brief Scales the given parameter on all emitors relatively to their CURRENT value.
		\param parameter_id \p int The parameter to adjust (enum EmitorParam)
		\param coef \p float The multiplier to apply
	*/
	void ScaleParticleParam(int parameter_id, float coef )
	{
		if (!m_ParticleEffect)
			return;
		
		int emitors = GetParticleEmitorCount(m_ParticleEffect);	
		for (int i = 0; i < emitors; ++i)
		{
			float value;
			GetParticleParm(m_ParticleEffect, i, parameter_id, value);
			SetParticleParm(m_ParticleEffect, i, parameter_id, value * coef);
		}
	}
	
	/**
	\brief Increments the value of the given parameter relatively from the ORIGINAL value.
		\note It's a simple sum, so negative value decrements
		\param parameter_id \p int The parameter to adjust (enum EmitorParam)
		\param value \p float The value to sum
	*/
	void IncrementParticleParamFromOriginal(int parameter_id, float value )
	{
		if (!m_ParticleEffect)
			return;
		
		int emitors = GetParticleEmitorCount(m_ParticleEffect);	
		for (int i = 0; i < emitors; ++i)
		{
			float param;
			GetParticleParmOriginal(m_ParticleEffect, i, parameter_id, param);
			SetParticleParm(m_ParticleEffect, i, parameter_id, param + value);
		}
	}
	
	/**
	\brief Increments the value of the given parameter relatively from the CURRENT value.
		\note It's a simple sum, so negative value decrements
		\param parameter_id \p int The parameter to adjust (enum EmitorParam)
		\param value \p float The value to sum
	*/
	void IncrementParticleParam(int parameter_id, float value )
	{
		if (!m_ParticleEffect)
			return;
		
		int emitors = GetParticleEmitorCount(m_ParticleEffect);	
		for (int i = 0; i < emitors; ++i)
		{
			float param;
			GetParticleParm(m_ParticleEffect, i, parameter_id, param);
			SetParticleParm(m_ParticleEffect, i, parameter_id, param + value);
		}
	}
	
	//@}
	
	
	
	/** \name Wiggle API
 		Settings to make the Particle wiggle
	*/
	//@{
	
	/**
	\brief Checks if particle is currently wiggling
	*/
	bool IsWiggling()
	{
		return m_RandomizeOri && m_RandomizeOri.IsRunning();
	}
	
	/**
	\brief Makes the particle change direction by random_angle every random_interval seconds.
		\note This does not actually work on Particle with no parent, it should on ParticleSource
		\note Calling SetWiggle(0,0) will effectively stop all wiggle functionality
		\param random_angle \p float Will be the range [-random_angle, random_angle[ to wiggle between
		\param random_interval \p float Will be the time range [0, random_interval] to wiggle next time
	*/
	void SetWiggle(float random_angle, float random_interval)
	{	
		if ( random_angle != 0 || random_interval != 0 )
		{	
			m_MaxOriWiggle = random_angle;
			m_MaxOriInterval = random_interval;
			
			if ( !m_RandomizeOri )
				m_RandomizeOri = new Timer( CALL_CATEGORY_GAMEPLAY );
			
			if ( !m_RandomizeOri.IsRunning() ) // Makes sure the timer is NOT running already
				m_RandomizeOri.Run( Math.RandomFloat(0, m_MaxOriInterval) , this, "RandomizeOrientation", null, false);
		}
		else
		{
			StopWiggle();
		}
	}
	
	/**
	\brief Stops randomized wiggle
	*/
	void StopWiggle()
	{			
		if ( m_RandomizeOri )
		{
			m_RandomizeOri.Stop();
		}
		
		m_MaxOriWiggle = 0;
		m_MaxOriInterval = 0;
	}
	
	/**
	\brief Randomizes a new orientation and applies it
	*/
	void RandomizeOrientation()
	{
		m_WiggleProcessing = true;
		
		if (m_ParentObject)
		{
			if ( !m_RandomizeOri.IsRunning() )
			{
				m_RandomizeOri.Run( Math.RandomFloat(0, m_MaxOriInterval) , this, "RandomizeOrientation", NULL, false);
			}
			
			Object old_parent = m_ParentObject;
			AddAsChild( null );
			AddAsChild( old_parent, m_DefaultPos, m_DefaultOri + RandWiggleVector() );
		}
		
		m_WiggleProcessing = false;
	}
	
	/**
	\brief Helper to get a randomized wiggle vector
	*/
	protected vector RandWiggleVector()
	{
		return Vector( RandWiggleFloat(), RandWiggleFloat(), RandWiggleFloat() );
	}
	
	/**
	\brief Helper to get a randomized wiggle float value
	*/
	protected float RandWiggleFloat()
	{
		return Math.RandomFloatInclusive(-m_MaxOriWiggle, m_MaxOriWiggle);
	}
	
	//@}
}