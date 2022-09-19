//! Flags to pass to ParticleSource.SetParticleAutoDestroyFlags
enum ParticleAutoDestroyFlags
{
	//! No automatic destroying
	NONE,
	//! Destroy when the Particle ends (looping particle never ends)
	ON_END,
	//! Destroy when particle stops
	ON_STOP,
	//! ON_END | ON_STOP
	ALL,
}

//! Flags to pass to ParticleSource.PlayParticleEx
enum PlayParticleFlags
{
	//! No flags
	NONE,		
	// Is just a placeholder for now
}

//! Flags to pass to ParticleSource.StopParticle
enum StopParticleFlags
{
	//! No flags
	NONE,
	//! Reset state after stopping
	RESET,
	/**
	\brief Flag will make the particle stop immediately, taking it out of simulation and clearing VISIBLE flag
	*	\note By default the particle will gradually fade
	*	\note Note that when using IMMEDIATE, it is possible to call PlayParticle and it will resume from the moment it stopped
	*		  Which is not possible by default, as the gradual fade is accomplished by nulling the lifetime
	*/
	IMMEDIATE,
	//! Is default behaviour, but can be used in conjuction with IMMEDIATE which hides it when this flag is not present
	VISIBLE,
	//! (SPF_IMMEDIATE | SPF_VISIBLE) "Freezes" the particle while keeping it visible
	PAUSE,
}

//! Mode for GetParticle
enum EGetParticleMode
{
	//! Full path with ext 		("graphics/particles/smoking_barrel_small.ptc")
	FULL,
	//! Full path without ext 	("graphics/particles/smoking_barrel_small")
	NO_EXT,
	//! Filename only 			("smoking_barrel_small")
	FILE,
}

enum ParticlePropertiesFlags
{
	NONE,
	//! Makes the particle start playing immediately after being created
	PLAY_ON_CREATION,
	//! Only applicable when there is a parent, this will force the localOri to be in world space instead of local
	FORCE_WORLD_ROT,
	//! By default, a particle unparents when it ends, this disables this behaviour
	KEEP_PARENT_ON_END,
};

//! Properties while creating a ParticleSource
class ParticleProperties
{
	/**
	*\brief Constructor (ctor)
	*	\param localPos \p vector Local position of particle, if no parent this is world position
	*	\param flags \p int See ParticlePropertiesFlags
	*	\param parent \p Object The parent that the ParticleSource will be added to as a child
	*	\param localOri \p vector Local orientation of particle, if no parent this is world orientation
	*	\param owner \p Class The owning instance for this particle, as long as it exists, this particle will not be reused (unless flagged)
	*					Important when for example virtual is enabled
	*					since if the particle owner has been deleted, the virtual should not be converted
	*					Only fill in owner if necessary, for example when the owner is reusing the same particle after storing it
	*/
	void ParticleProperties(vector localPos, int flags, Object parent = null, vector localOri = vector.Zero, Class owner = null)
	{
	}
	
	//! dtor
	void ~ParticleProperties()
	{
	}
	
	/** \name Data access
 		Methods to retrieve the data in script
	*/
	//@{
	proto Object GetParent();
	proto Class GetOwner();
	proto vector GetLocalPos();
	proto vector GetLocalOri();
	proto int GetPPFlags();
	proto bool IsPlayOnCreation();
	proto bool IsForceWorldRotation();	
	proto bool IsKeepParentOnEnd();
	//@}
}

typedef array<ref ParticleProperties> ParticlePropertiesArray;
typedef array<ParticleSource> ParticleSourceArray;

/**
*\brief Entity which has the particle instance as an ObjectComponent
*	\note Unfortunately the native methods are private and have a script wrapper
*		  This is because of backwards compatibility with the old Particle and to avoid confusion
*		  As we should continue to use Particle for consistency, as it is too deeply rooted in script
*		  Which means switching is no longer possible due to it being harmful for already existing mods
*		  Even though using the ParticleSource directly would be preferred
*
*	\note Some variables from Particle are invalid on this level, those marked DEPRECATED by Particle are still DEPRECATED here
*		  m_ParticleID; 			//!< INVALID - Use GetParticleID() instead
*		  m_Lifetime;				//!< INVALID - No EOnFrame, so no use for it
*		  m_IsPlaying;				//!< VALID - Set through events - IsParticlePlaying() is still better
*		  m_IsRepeat;				//!< INVALID - Use IsRepeat() instead
*		  m_MarkedForDeletion;		//!< INVALID - No EOnFrame, so no use for it
*		  Wiggle API variables are still used for Wiggle API
*		  m_ParentObject; 			//!< VALID - Set through events - Or use GetParticleParent() or GetParent()
*		  m_ParticleEffect;			//!< VALID - Not in the same usage as Particle, as it will always be 'this', never null
*/
class ParticleSource : Particle
{
	//! ctor
	void ParticleSource()
	{
		m_ParticleEffect = this;
	}
	
	//! dtor
	void ~ParticleSource()
	{
	}
	
	//! Empty - Only needed for Particle
	override protected void ParticleInit() {}
	
	/**
	\brief Enables the particle to automatically clean up itself when ending or stopping
		\note Is enabled by default (ALL)
		\note Looping particles never end
		\note ParticleSource belonging to a ParticleManager will ignore these
		\param flags \p ParticleAutoDestroyFlags Flags enabling when the particle should be auto destroyed
	*/
	proto native void SetParticleAutoDestroyFlags(ParticleAutoDestroyFlags flags);
	
	/**
	\brief Disables the particle automatically cleaning up itself when ending or stopping
	*/
	void DisableAutoDestroy()
	{
		SetParticleAutoDestroyFlags(ParticleAutoDestroyFlags.NONE);
	}
	
	/**
	\brief Gets the currently set ParticleAutoDestroyFlags flags set on this ParticleSource
		\return \p int Currently set ParticleAutoDestroyFlags flags set on this ParticleSource
	*/
	proto native int GetParticleAutoDestroyFlags();
	
	/** \name Create a particle (static)
		Helper static methods to create/attach/play a ParticleSource on one line
	*/
	//@{
	
	/**
	\brief Create function
		\param id \p int Particle ID registered in ParticleList
		\param pos \p vector Position of ParticleSource in LS (WS when no parent)
		\param playOnCreation \p bool Whether to play immediately after creation (Optional)
		\param parent \p Object Parent Object which will child the ParticleSource (Optional)		
		\param ori \p vector Orientation of ParticleSource in LS (WS when no parent) (Pitch, Yaw, Roll in degrees) (Optional)
		\param forceWorldRotation \p bool Forces orientation to rotate relative to the world and not with the parent (Optional)
		\param owner \p Class The owning instance for this particle (Optional)
		\return \p ParticleSource Created particle instance when successful
	*/
	static ParticleSource CreateParticle( int id, vector pos, bool playOnCreation = false, Object parent = null, vector ori = vector.Zero, bool forceWorldRotation = false, Class owner = null )
	{
		int flags = ParticlePropertiesFlags.NONE;
		
		if (playOnCreation)
		{
			flags = flags | ParticlePropertiesFlags.PLAY_ON_CREATION;
		}
		
		if (forceWorldRotation)
		{
			flags = flags | ParticlePropertiesFlags.FORCE_WORLD_ROT;
		}

		return CreateParticleEx(id, pos, flags, parent, ori, owner);
	}
	
	/**
	\brief Master create function
		\param id \p int Particle ID registered in ParticleList
		\param pos \p vector Position of ParticleSource in LS (WS when no parent)
		\param flags \p int See ParticlePropertiesFlags (Optional)
		\param parent \p Object Parent Object which will child the ParticleSource (Optional)		
		\param ori \p vector Orientation of ParticleSource in LS (WS when no parent) (Pitch, Yaw, Roll in degrees) (Optional)
		\param owner \p Class The owning instance for this particle (Optional)
		\return \p ParticleSource Created particle instance when successful
	*/
	static ParticleSource CreateParticleEx( int id, vector pos, int flags = ParticlePropertiesFlags.NONE, Object parent = null, vector ori = vector.Zero, Class owner = null )
	{
		string particlePath = ParticleList.GetParticleFullPath(id);
		if (particlePath == "") // There is already an error inside of ParticleList signaling this
		{
			ErrorEx(string.Format("Could not create ParticleSource as particle id %1 is invalid.", id));
			return null;
		}

		vector localPos = pos;
		
		if (parent)
			pos = parent.GetPosition();
		
		ParticleSource p = ParticleSource.Cast( GetGame().CreateObjectEx("ParticleSource", pos, ECE_LOCAL) );
		p.SetParticle(particlePath);
		ParticleProperties props = new ParticleProperties(localPos, flags, parent, ori, owner);
		p.ApplyProperties(props);
		
		return p;
	}
	
	/**
	\brief Creates a particle emitter and attaches it on the given object
		\param particle_id \p int Particle ID registered in ParticleList
		\param parent_obj \p Object Instance on which this particle will be attached
		\param local_pos \p vector Attachment position local to the parent (Optional)
		\param local_ori \p vector Orientation local to the parent (Pitch, Yaw, Roll in degrees) (Optional)
		\param force_world_rotation \p bool Forces particle's orientation to rotate relative to the world and not with the object (Optional)
		\return \p Particle Created particle instance
	*/
	override static Particle CreateOnObject(
		int particle_id,
		Object parent_obj,
		vector local_pos = "0 0 0",
		vector local_ori = "0 0 0",
		bool force_world_rotation = false )
	{
		return CreateParticle(particle_id, local_pos, false, parent_obj, local_ori, force_world_rotation);
	}
	
	/**
	\brief Legacy function for backwards compatibility
	*/
	override static Particle Create( int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0" )
	{
		return CreateOnObject( particle_id, parent_obj, local_pos, local_ori);
	}
	
	/**
	\brief Creates a particle emitter on the given position
		\param particle_id \p int Particle ID registered in ParticleList
		\param global_pos \p Vector Position where the particel will be created
		\param global_ori \p vector Orientation (Pitch, Yawn, Roll in degrees) (Optional)
		\param force_world_rotation \p bool Has absolutely no effect here as there is no parent
		\return \p Particle Created particle instance
	*/
	override static Particle CreateInWorld( int particle_id, vector global_pos, vector global_ori = "0 0 0", bool force_world_rotation = false )
	{
		return CreateParticle(particle_id, global_pos, false, null, global_ori, force_world_rotation);
	}
	
	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
	*/
	override static ParticleSource Create( int particle_id, vector global_pos, vector global_ori = "0 0 0" )
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
	override static Particle PlayOnObject(int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0", bool force_world_rotation = false )
	{
		return CreateParticle(particle_id, local_pos, true, parent_obj, local_ori, force_world_rotation);
	}
	
	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
	*/
	override static Particle Play( int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0" )
	{
		return PlayOnObject( particle_id, parent_obj, local_pos, local_ori);
	}
	
	/**
	\brief Creates a particle emitter on the given position and activates it
		\param particle_id \p int Particle ID registered in ParticleList
		\param global_pos \p Vector Position where the particel will be created
		\return \p Particle Created particle instance
	*/
	override static Particle PlayInWorld( int particle_id, vector global_pos)
	{
		return CreateParticle(particle_id, global_pos, true);
	}

	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
	*/
	override static Particle Play( int particle_id, vector global_pos)
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
		\note Will call OnParticleStart when successful
		\return \p bool Whether the particle successfully started
	*/
	private proto bool PlayParticleNative(int flags);
	
	/**
	\brief Method to tell the particle to start playing
		\param particle_id \p int Particle ID registered in ParticleList to start playing
		\return \p bool Whether the particle successfully started
	*/
	override bool PlayParticleEx(int particle_id = -1, int flags = 0)
	{
		if ( particle_id > -1 )
		{
			// Here we can just do it directly
			// While with the old system it will not work when the particle is already created
			SetParticleByID(particle_id);
		}
		
		return PlayParticleNative(flags);
	}
	
	/**
	\brief Method to tell the particle to stop playing
		\note The particle is not immediately stopped by default, instead it will gradually fade
			* If this is however desired, then pass in StopParticleFlags.IMMEDIATE
		\param flags \p int Flags to pass to the stopping (StopParticleFlags)
		\return \p bool Whether the particle successfully stopped
	*/
	private proto bool StopParticleNative(int flags);
	
	/**
	\brief Method to tell the particle to stop playing
		\param flags \p int Flags to pass to the stopping (StopParticleFlags)
		\return \p bool Whether the particle successfully stopped
	*/
	override bool StopParticle(int flags = 0)
	{
		return StopParticleNative(flags);
	}
	
	/**
	\brief Method to tell the particle to reset
		\return \p bool Whether the particle successfully reset
	*/
	private proto native bool ResetParticleNative();
	
	/**
	\brief Method to tell the particle to reset
		\return \p bool Whether the particle successfully reset
	*/
	override bool ResetParticle()
	{
		return ResetParticleNative();
	}
	
	/**
	\brief Method to tell the particle to restart (reset + play)
		\return \p bool Whether the particle successfully restarted
	*/
	private proto native bool RestartParticleNative();
	
	/**
	\brief Method to tell the particle to restart (reset + play)
		\return \p bool Whether the particle successfully restarted
	*/
	override bool RestartParticle()
	{
		return RestartParticleNative();
	}
	
	/**
	\brief Ask if the particle is still playing
		\return \p bool Whether the particle is playing
	*/
	private proto bool IsParticlePlayingNative();
	
	/**
	\brief Ask if the particle is still playing
		\return \p bool Whether the particle is playing
	*/
	override bool IsParticlePlaying()
	{
		return IsParticlePlayingNative();
	}
	
	//@}
	
	
	
	/** \name SetParticle
 		Methods that set the particle to be used
	*/
	//@{
	
	/**
	\brief Assigns a particle to the ParticleSource
		\param path \p string Path of particle effect
		\return \p bool Whether the creating and assigning of particle was successful
	*/
	private proto native bool SetParticleNative(string path);
	
	/**
	\brief Assigns a particle to the ParticleSource
		\note Intentionally private, as it is best to use 'SetParticleByID' instead from script, because ParticleList
		\param path \p string Path of particle effect
		\return \p bool Whether the creating and assigning of particle was successful
	*/
	private bool SetParticle(string path)
	{
		return SetParticleNative(path);
	}
	
	/**
	\brief Assigns a particle to the ParticleSource
		\param id \p int ID of particle registered in ParticleList
		\return \p bool Whether the creating and assigning of particle was successful
	*/
	bool SetParticleByID(int id)
	{
		return SetParticle(ParticleList.GetParticleFullPath(id));
	}
	
	/**
	\brief Sets particle id
		\note Purely here for backwards compatibility with Particle
		\param particle_id \p int Particle ID registered in ParticleList to start playing
	*/
	override void SetSource(int particle_id)
	{
		SetParticleByID(particle_id);
	}
	
	//@}
	
	
	
	/** \name GetParticle
 		Methods that get the particle assigned
	*/
	//@{
	
	/**
	\brief Gets the path to the currently assigned particle
		\param path \p string Path of particle effect
		\param mode \p EGetParticleMode What path to return
		\return \p bool Whether the path is filled in or not
	*/
	private proto bool GetParticleNative(out string path, EGetParticleMode mode);
	
	/**
	\brief Gets the path to the currently assigned particle
		\param path \p string Path of particle effect
		\param mode \p EGetParticleMode What path to return
		\return \p bool Whether the path is filled in or not
	*/
	bool GetParticle(out string path, EGetParticleMode mode)
	{
		return GetParticleNative(path, mode);
	}
	
	/**
	\brief Gets the ParticleList ID of the currently assigned particle
		\return \p int ID of particle registered in ParticleList
	*/
	override int GetParticleID()
	{
		string path;
		if (GetParticle(path, EGetParticleMode.FILE))
			return ParticleList.GetParticleIDByName(path);
		else
			return -1;			
	}
	
	/**
	\brief Gets the ParticleList ID of the currently assigned particle
		\note This is because the new GetParticleID will only work when every particle file name has a unique name
			* In case this is not the case, then this method is what one will want to use
		\return \p int ID of particle registered in ParticleList
	*/
	int GetParticleIDLegacy()
	{
		string path;
		if (GetParticle(path, EGetParticleMode.NO_EXT))
			return ParticleList.GetParticleID(path);
		else
			return -1;
	}
	
	//@}
	
	
	
	/** \name Apply properties
 		Methods to apply properties to the particle
	*/
	//@{
	
	/**
	\brief Applies the properties given to the ParticleSource
		\note Already handles proper reparenting
		\param properties \p ParticleProperties The properties to apply
		\return \p bool Whether the properties were successfully applied
	*/
	private proto native bool ApplyPropertiesNative(ParticleProperties properties);
	
	/**
	\brief Applies the properties given to the ParticleSource		
		\param properties \p ParticleProperties The properties to apply
		\return \p bool Whether the properties were successfully applied
	*/
	bool ApplyProperties(ParticleProperties properties)
	{
		return ApplyPropertiesNative(properties);
	}
	
	//@}
	
	
	
	/** \name Properties and state
		Obtain information or set properties regarding the state of the Particle
	*/
	//@{
	
	/**
	\brief Returns direct particle effect entity
		\note Backwards compatibility with Particle, here it is the Object itself
		\return \p Object The Object with the particle component (this)
	*/
	override Object GetDirectParticleEffect()
	{
		return this;
	}
	
	/**
	\brief Returns the parent of this Particle if there is one
		\return \p Object The parent or null if there is none
	*/
	override Object GetParticleParent()
	{
		return Object.Cast(GetParent());
	}
	
	/**
	\brief Returns if there is any particle active
		\return \p bool Whether there is any particle active
	*/
	private proto bool HasActiveParticleNative();
	
	/**
	\brief Returns if there is any particle active
		\return \p bool Whether there is any particle active
	*/
	override bool HasActiveParticle()
	{
		return HasActiveParticleNative();
	}
	
	/**
	\brief Returns the total count of active particles in all emitors
		\note Internally does a sum, HasActiveParticle is better for a quick check
		\return \p int Total count of active particles
	*/
	private proto int GetParticleCountNative();
	
	/**
	\brief Returns the total count of active particles in all emitors
		\note Internally does a sum, HasActiveParticle is better for a quick check
		\return \p int Total count of active particles
	*/
	override int GetParticleCount()
	{
		return GetParticleCountNative();
	}
	
	/**
	\brief Returns whether there is a repeating particle
		\return \p bool whether there is a repeating particle
	*/
	private proto bool IsRepeatNative();
	
	/**
	\brief Returns whether there is a repeating particle
		\return \p bool whether there is a repeating particle
	*/
	override bool IsRepeat()
	{
		return IsRepeatNative();
	}
	
	/**
	\brief Returns the approx. max lifetime
		\return \p float The largest lifetime sum among the emitors
	*/
	private proto float GetMaxLifetimeNative();
	
	/**
	\brief Returns the approx. max lifetime
		\return \p float The largest lifetime sum among the emitors
	*/
	override float GetMaxLifetime()
	{
		return GetMaxLifetimeNative();
	}
	
	/**
	\brief Get the owner of this ParticleSource
		\return \p Class The owner or null
	*/
	proto native Class GetOwner();
	
	/**
	\brief Set the owner of this ParticleSource
		\param \p Class The owner or null
	*/
	proto native void SetOwner(Class owner);
	
	/**
	\brief null the owner of this ParticleSource
	*/
	proto native void Orphan();
		
	//@}
	
	
	
	/** \name Owning ParticleManager
 		Methods regarding the owner
	*/
	//@{
	
	/**
	\brief Get the ParticleManager the ParticleSource belongs to if any
		\return \p ParticleManager The ParticleManager which created the ParticleSource or null
	*/
	proto native ParticleManager GetParticleManager();
	
	/**
	\brief Get the index of this ParticleSource in the owning ParticleManager
		\return \p int The index in the pool in the owning ParticleManager, -1 if no owner
	*/
	proto native int GetIndex();
	
	//@}
	
	
	
	/** \name Static counter API
		Gives counter information for the class
	*/
	//@{
	
	/**
	\brief Gets the ID for the ParticleSource
		\note Different every restart, as it is the nth instance created since the start of the program
		\return \p int ID for the ParticleSource
	*/
	proto int GetCountID();
	
	/**
	\brief Gets the amount of ParticleSource that have been created since the start of the program
		\return \p int Amount of ParticleSource that have been created since the start of the program
	*/
	proto native static int GetStaticCount();
	
	/**
	\brief Gets the amount of ParticleSource that are currently existing
		\return \p int Amount of ParticleSource that are currently existing
	*/
	proto native static int GetStaticActiveCount();
	
	//@}
	
	
	
	/** \name Events
	*	ParticleBase events
	*	For ParticleSource, these are handed on C++ side
	*	For more information, read ParticleEvents
	*/
	//@{
	
	/**
	\brief Event when the particle receives a parent
	*/
	override protected void OnParticleParented(IEntity parent)
	{
		m_ParentObject = Object.Cast(parent);
		
		super.OnParticleParented(parent);
	}
	
	/**
	\brief Event when the particle is orphaned
	*/
	override protected void OnParticleUnParented(IEntity parent)
	{
		m_ParentObject = null;
		
		// Since we have lost the parent, we will need to refresh the wiggle
		// As it uses a cached local position, which is now no longer correct
		if (!m_WiggleProcessing && IsWiggling())
		{
			float randomAngle = m_MaxOriWiggle;
			float randomInterval = m_MaxOriInterval;
			m_DefaultPos = m_DefaultWorldPos;
			m_DefaultOri = m_DefaultWorldOri;
			
			StopWiggle();
			SetWiggle(randomAngle, randomInterval);
		}
		
		super.OnParticleUnParented(parent);
	}
	
	/**
	\brief Event when the particle stops
	*/
	override protected void OnParticleStop()
	{
		if (IsWiggling())
		{
			StopWiggle();
			delete m_RandomizeOri;
		}
		
		super.OnParticleStop();
	}
	
	//@}
	
	
	
	/** \name Misc backwards compatibility helpers
		Helpers for misc backwards compatibility functionality
	*/
	//@{
	
	/**
	\brief Attaches this particle onto some object. If null value is provided then the particle will be detached from the current parent.
		\note Unlike Particle, you are free to use AddChild/RemoveChild instead of this one if you know what you are doing
		\param parent \p Object Parent onto which this particle will be attached
		\param local_pos \p vector Attachment position local to the parent (optional)
		\param local_ori \p vector Orientation local to the parent (Pitch, Yawn, Roll in degrees) (Optional)
		\param force_rotation_to_world \p bool Force rotation to be in WS (Optional)
	*/
	override void AddAsChild(Object parent, vector local_pos = "0 0 0", vector local_ori = "0 0 0", bool force_rotation_to_world = false)
	{
		int flags = ParticlePropertiesFlags.NONE;
		
		if (force_rotation_to_world)
			flags = ParticlePropertiesFlags.FORCE_WORLD_ROT;
		
		ParticleProperties props = new ParticleProperties(local_pos, flags, parent, local_ori);
		ApplyProperties(props);
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
	override void SetParticleParam(int parameter_id, float value )
	{
		SetParticleParm(this, -1, parameter_id, value);
	}
	
	/**
	\brief Set the value of a parameter of an emitor in the particle
		\param emitter \p int The emitter to apply the new value to, -1 for all emitter
		\param parameter \p int The parameter to apply the new value to (enum EmitorParam)
		\param value \p float The value to apply
	*/
	override void SetParameter(int emitter, int parameter, float value)
	{
		SetParticleParm(this, emitter, parameter, value);
	}
	
	/**
	\brief Get the value of a parameter of an emitor in the particle
		\param emitter \p int The emitor to get the value from
		\param parameter \p int The parameter to get the value from (enum EmitorParam)
		\param value \p float The value
	*/
	override void GetParameter(int emitter, int parameter, out float value)
	{
		GetParticleParm(this, emitter, parameter, value);
	}
	
	/**
	\brief Get the value of a parameter of an emitor in the particle
		\param emitter \p int The emitor to get the value from
		\param parameter \p int The parameter to get the value from (enum EmitorParam)
		\return \p float The value
	*/
	override float GetParameterEx(int emitter, int parameter)
	{
		float value;
		GetParticleParm(this, emitter, parameter, value);
		return value;
	}
	
	/**
	\brief Scales the given parameter on all emitors relatively to their ORIGINAL value.
		\param parameter_id \p int The parameter to adjust (enum EmitorParam)
		\param coef \p float The multiplier to apply
	*/
	override void ScaleParticleParamFromOriginal(int parameter_id, float coef )
	{	
		int emitors = GetParticleEmitorCount(this);			
		for (int i = 0; i < emitors; ++i)
		{
			float value;
			GetParticleParmOriginal(this, i, parameter_id, value);
			SetParticleParm(this, i, parameter_id, value * coef);
		}
	}
	
	/**
	\brief Scales the given parameter on all emitors relatively to their CURRENT value.
		\param parameter_id \p int The parameter to adjust (enum EmitorParam)
		\param coef \p float The multiplier to apply
	*/
	override void ScaleParticleParam(int parameter_id, float coef )
	{
		int emitors = GetParticleEmitorCount(this);	
		for (int i = 0; i < emitors; ++i)
		{
			float value;
			GetParticleParm(this, i, parameter_id, value);
			SetParticleParm(this, i, parameter_id, value * coef);
		}
	}
	
	/**
	\brief Increments the value of the given parameter relatively from the ORIGINAL value.
		\note It's a simple sum, so negative value decrements
		\param parameter_id \p int The parameter to adjust (enum EmitorParam)
		\param value \p float The value to sum
	*/
	override void IncrementParticleParamFromOriginal(int parameter_id, float value )
	{
		int emitors = GetParticleEmitorCount(this);	
		for (int i = 0; i < emitors; ++i)
		{
			float param;
			GetParticleParmOriginal(this, i, parameter_id, param);
			SetParticleParm(this, i, parameter_id, param + value);
		}
	}
	
	/**
	\brief Increments the value of the given parameter relatively from the CURRENT value.
		\note It's a simple sum, so negative value decrements
		\param parameter_id \p int The parameter to adjust (enum EmitorParam)
		\param value \p float The value to sum
	*/
	override void IncrementParticleParam(int parameter_id, float value )
	{
		int emitors = GetParticleEmitorCount(this);	
		for (int i = 0; i < emitors; ++i)
		{
			float param;
			GetParticleParm(this, i, parameter_id, param);
			SetParticleParm(this, i, parameter_id, param + value);
		}
	}
	
	//@}
	
	
	
	/** \name Wiggle API
 		Settings to make the Particle wiggle
	*/
	//@{
	
	/**
	\brief Makes the particle change direction by random_angle every random_interval seconds.
		\note This does not actually work on Particle with no parent, it should on ParticleSource
		\param random_angle \p float Will be the range [-random_angle, random_angle[ to wiggle between
		\param random_interval \p float Will be the time range [0, random_interval] to wiggle next time
	*/
	override void SetWiggle(float random_angle, float random_interval)
	{
		if (random_angle != 0 || random_interval != 0)
		{
			if (IsWiggling())
			{
				m_MaxOriWiggle = random_angle;
				m_MaxOriInterval = random_interval;
				return;
			}
			
			// We need the position to be accurate before storing it
			Update();

			// These are only ever used within the Wiggle API
			// To restore the properties after wiggling
			// So let's only set them within the Wiggle API c:
			
			m_DefaultPos = GetLocalPosition();
			m_DefaultOri = GetLocalYawPitchRoll();
			m_DefaultWorldPos = GetWorldPosition();
			m_DefaultWorldOri = GetYawPitchRoll();	
			m_ForceOrientationRelativeToWorld = IsHierarchyPositionOnly();
		}
		
		super.SetWiggle(random_angle, random_interval);
	}
	
	/**
	\brief Stops randomized wiggle
	*/
	override void StopWiggle()
	{
		bool wiggling = IsWiggling();
		
		super.StopWiggle();
		
		if (wiggling)
		{
			// Restore pre-wiggle orientation
			int flags = ParticlePropertiesFlags.NONE;
			
			if (m_ForceOrientationRelativeToWorld)
				flags = ParticlePropertiesFlags.FORCE_WORLD_ROT;
			
			ParticleProperties prop = new ParticleProperties(m_DefaultPos, flags, GetParticleParent(), m_DefaultOri, GetOwner());
			ApplyProperties(prop);
		}
	}
	
	/**
	\brief Randomizes a new orientation and applies it
	*/
	override private void RandomizeOrientation()
	{
		if (ToDelete())
			return;
		
		m_WiggleProcessing = true;
		
		if ( !m_RandomizeOri.IsRunning() )
			m_RandomizeOri.Run( Math.RandomFloat(0, m_MaxOriInterval) , this, "RandomizeOrientation", null, false);
		
		int flags = ParticlePropertiesFlags.NONE;
		
		if (m_ForceOrientationRelativeToWorld)
			flags = ParticlePropertiesFlags.FORCE_WORLD_ROT;

		ParticleProperties prop = new ParticleProperties(m_DefaultPos, flags, GetParticleParent(), m_DefaultOri + RandWiggleVector(), GetOwner());
		ApplyProperties(prop);
		
		m_WiggleProcessing = false;
	}
	
	//@}
	
	
	
	/** \name Misc Particle specific helpers
		Functionality specific for Particle, so we empty them here
	*/
	//@{
	
	//! Empty
	override private void UpdateState() { ErrorEx("Should not be in use on ParticleSource."); }
	
	//! Empty
	override private void DestroyParticleEffect() { ErrorEx("Should not be in use on ParticleSource."); }

	//! Empty
	override private void CreateParticleEffect() { ErrorEx("Should not be in use on ParticleSource."); }
	
	//! Empty
	override protected void EOnFrame(IEntity other, float timeSlice) { ErrorEx("Should not be in use on ParticleSource."); }	
	
	//! Empty
	override private void OnCheckAutoDelete() { ErrorEx("Should not be in use on ParticleSource."); }	
	
	//! Empty
	override private void OnToDelete() { ErrorEx("Should not be in use on ParticleSource."); }
	
	//@}
}