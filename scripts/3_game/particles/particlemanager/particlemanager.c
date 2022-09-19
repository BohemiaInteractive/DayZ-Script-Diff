//! Flags for ParticleManagerSettings
enum ParticleManagerSettingsFlags
{
	NONE,
	//! Particles will be locked to the index and not reused
	FIXED_INDEX,
	//! Allocation blocks the game until it is done
	BLOCKING,
	//! Disable the creation of virtual particles when the pool is still allocating
	DISABLE_VIRTUAL,
	//! Reuse stopped particles even if they are owned by something
	REUSE_OWNED,
};

//! Class simply to have easily modded constants
class ParticleManagerConstants
{
	/** \name Global ParticleManager settings
 		Settings applied to the global ParticleManager
	*/
	//@{
	static const int POOL_SIZE = 10000;
	static const int FLAGS = ParticleManagerSettingsFlags.NONE;
	//@}
}

//! Settings given to ParticleManager on creation (in ctor)
class ParticleManagerSettings
{
	/**
	\brief Constructor (ctor)
		\param poolSize \p int Size of pool (amount of created and reserved particles)
		\param flags \p int ParticleManagerSettingsFlags
	*/
	void ParticleManagerSettings(int poolSize, int flags = ParticleManagerSettingsFlags.NONE)
	{
	}
	
	//! dtor
	void ~ParticleManagerSettings()
	{
	}
}

//! Invokers for ParticleManager events
class ParticleManagerEvents
{
	ref ScriptInvoker Event_OnAllocation = new ScriptInvoker();
	ref ScriptInvoker Event_OnAllocationEnd = new ScriptInvoker();
}

//! Has a fixed pool of precreated and reserved particles
class ParticleManager : Managed
{
	//! Static ParticleManager
	private static ref ParticleManager g_ParticleManager;

	//! Access to the static ParticleManager
	static ParticleManager GetInstance()
	{
		if (!g_ParticleManager && !GetGame().IsDedicatedServer())
		{
			g_ParticleManager = new ParticleManager(
				new ParticleManagerSettings(
					ParticleManagerConstants.POOL_SIZE,
					ParticleManagerConstants.FLAGS));
			g_ParticleManager.SetName("GlobalParticleManager");
		}
		
		return g_ParticleManager;
	}
	
	//! To clean it up properly before game closes
	static void CleanupInstance()
	{
		if (g_ParticleManager)
			delete g_ParticleManager;
	}
	
	/**
	\brief Constructor (ctor)
		\param settings \p ParticleManagerSettings Settings for the ParticleManager
	*/
	void ParticleManager(ParticleManagerSettings settings)
	{		
	}
	
	//! dtor
	void ~ParticleManager()
	{
	}
	
	
	
	/** \name API for compatibility with Particle/ParticleSource create/play
		Mimics the static Create and Play methods from Particle/ParticleSource
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
	ParticleSource CreateParticle( int id, vector pos, bool playOnCreation = false, Object parent = null, vector ori = vector.Zero, bool forceWorldRotation = false, Class owner = null )
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
	ParticleSource CreateParticleEx( int id, vector pos, int flags = ParticlePropertiesFlags.NONE, Object parent = null, vector ori = vector.Zero, Class owner = null )
	{
		string particlePath = ParticleList.GetParticleFullPath(id);
		if (particlePath == "") // There is already an error inside of ParticleList signaling this
		{
			ErrorEx(string.Format("Could not create ParticleSource as particle id %1 is invalid.", id));
			return null;
		}

		ParticleProperties props = new ParticleProperties(pos, flags, parent, ori, owner);
		ParticleSource p = CreateParticleByPath(particlePath, props);
		return p;
	}
	
	/**
	\brief Creates a particle emitter and attaches it on the given object
		\param particle_id \p int Particle ID registered in ParticleList
		\param parent_obj \p Object Instance on which this particle will be attached
		\param local_pos \p vector Attachment position local to the parent (Optional)
		\param local_ori \p vector Orientation local to the parent (Pitch, Yaw, Roll in degrees) (Optional)
		\param force_world_rotation \p bool Forces particle's orientation to rotate relative to the world and not with the object (Optional)
		\return \p ParticleSource Created particle instance
	*/
	ParticleSource CreateOnObject(
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
	ParticleSource Create( int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0" )
	{
		return CreateOnObject( particle_id, parent_obj, local_pos, local_ori);
	}
	
	/**
	\brief Creates a particle emitter on the given position
		\param particle_id \p int Particle ID registered in ParticleList
		\param global_pos \p Vector Position where the particel will be created
		\param global_ori \p vector Orientation (Pitch, Yawn, Roll in degrees) (Optional)
		\param force_world_rotation \p bool Has absolutely no effect here as there is no parent
		\return \p ParticleSource Created particle instance
	*/
	ParticleSource CreateInWorld( int particle_id, vector global_pos, vector global_ori = "0 0 0", bool force_world_rotation = false )
	{
		return CreateParticle(particle_id, global_pos, false, null, global_ori, force_world_rotation);
	}
	
	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
	*/
	ParticleSource Create( int particle_id, vector global_pos, vector global_ori = "0 0 0" )
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
		\return \p ParticleSource Created particle instance
	*/
	ParticleSource PlayOnObject(int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0", bool force_world_rotation = false )
	{
		return CreateParticle(particle_id, local_pos, true, parent_obj, local_ori, force_world_rotation);
	}
	
	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
	*/
	ParticleSource Play( int particle_id, Object parent_obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0" )
	{
		return PlayOnObject( particle_id, parent_obj, local_pos, local_ori);
	}
	
	/**
	\brief Creates a particle emitter on the given position and activates it
		\param particle_id \p int Particle ID registered in ParticleList
		\param global_pos \p Vector Position where the particel will be created
		\return \p ParticleSource Created particle instance
	*/
	ParticleSource PlayInWorld(int particle_id, vector global_pos)
	{
		return PlayInWorldEx(particle_id, null, global_pos);
	}
	
	ParticleSource PlayInWorldEx(int particle_id, Object parent_obj, vector global_pos, vector global_ori = "0 0 0", bool force_world_rotation = false)
	{
		return CreateParticle(particle_id, global_pos, true, parent_obj, global_ori, force_world_rotation);
		
	}

	/**
	\brief Legacy function for backwards compatibility with 1.01 and below
	*/
	ParticleSource Play( int particle_id, vector global_pos)
	{
		return PlayInWorld( particle_id, global_pos);
	}
	
	//@}
	
	
	
	/** \name Get Particles
 		API for creating, playing or obtaining particles from the pool
	*/
	//@{
	
	/**
	\brief Creates an amount of particles with the properties given
		\param particles \p array<ParticleSource> The resulting particles if an array is given
		\param path \p string Path of particle effect
		\param properties \p ParticleProperties Properties of the particles created
		\param count \p int Amount of particles to create with these properties
		\return \p int Amount of particles created in this frame, if the ParticleManager is still allocating while this is called and virtual particles are enabled, the overflow particles will be virtual instead and not given in the out array
	*/
	proto native int CreateParticles(array<ParticleSource> particles, string path, notnull ParticlePropertiesArray properties, int count = 1);
	
	/**
	\brief Create a particle
		\param path \p string Path of particle effect
		\param properties \p ParticleProperties Properties of the particles created
		\return \p ParticleSource The resulting particle
	*/
	ParticleSource CreateParticleByPath(string path, notnull ParticleProperties properties)
	{
		array<ParticleSource> tempArr = new array<ParticleSource>;
		CreateParticles(tempArr, path, {properties}, 1);
		
		if (tempArr.Count() > 0)
			return tempArr[0];
		else
			return null;
	}
	
	/**
	\brief QoL function using script ParticleList, strongly recommend to read comments for CreateParticles as well
		\param id \p int ID of particle registered in ParticleList
		\param properties \p ParticleProperties Properties of the particles created
		\param count \p int Amount of particles to create with these properties
		\return \p int Amount of particles created in this frame, if the ParticleManager is still allocating while this is called and virtual particles are enabled, the overflow particles will be virtual instead
	*/
	int CreateParticlesById(int id, notnull ParticlePropertiesArray properties, int count)
	{
		return CreateParticles(null, ParticleList.GetParticleFullPath(id), properties, count);
	}
	
	/**
	\brief QoL function using script ParticleList, strongly recommend to read comments for CreateParticles as well
		\param id \p int ID of particle registered in ParticleList
		\param properties \p ParticleProperties Properties of the particles created
		\param count \p int Amount of particles to create with these properties
		\return \p array<ParticleSource> The resulting particles - Read particles param of CreateParticles
	*/
	array<ParticleSource> CreateParticlesByIdArr(int id, notnull ParticlePropertiesArray properties, int count)
	{
		array<ParticleSource> outArr = new array<ParticleSource>;
		CreateParticles(outArr, ParticleList.GetParticleFullPath(id), properties, count);
		return outArr;
	}
	
	/**
	\brief QoL function for when only one particle is needed using script ParticleList, strongly recommend to read comments for CreateParticles as well
		\param id \p int ID of particle registered in ParticleList
		\param properties \p ParticleProperties Properties of the particles created
		\return \p ParticleSource The resulting particle
	*/
	ParticleSource CreateParticleById(int id, ParticleProperties properties)
	{
		array<ParticleSource> tempArr = new array<ParticleSource>;
		CreateParticles(tempArr, ParticleList.GetParticleFullPath(id), {properties}, 1);
		
		if (tempArr.Count() > 0)
			return tempArr[0];
		else
			return null;
	}
	
	/**
	\brief QoL function for when wanting to play a particle at a position right away
		\param particles \p array<ParticleSource> The resulting particles if an array is given
		\param path \p string Path of particle effect
		\param positions \p array<vector> Positions of particles
		\param count \p int Amount of particles to create at this position
		\return \p int Amount of particles created in this frame, if the ParticleManager is still allocating while this is called and virtual particles are enabled, the overflow particles will be virtual instead and not given in the out array
	*/
	proto native int PlayParticles(out array<ParticleSource> particles, string path, notnull array<vector> positions, int count = 1);
	
	/**
	\brief QoL function using script ParticleList, strongly recommend to read comments for PlayParticles as well
		\param id \p int ID of particle registered in ParticleList
		\param positions \p array<vector> Positions of particles
		\param count \p int Amount of particles to create at this position
		\return \p array<ParticleSource> The resulting particles - Read particles param of PlayParticles
	*/
	array<ParticleSource> PlayParticlesById(int id, array<vector> positions, int count)
	{
		array<ParticleSource> outArr = new array<ParticleSource>;
		PlayParticles(outArr, ParticleList.GetParticleFullPath(id), positions, count);
		return outArr;
	}
	
	/**
	\brief QoL function for when only one particle is needed using script ParticleList, strongly recommend to read comments for PlayParticles as well
		\param id \p int ID of particle registered in ParticleList
		\param positions \p array<vector> Positions of particles
		\return \p ParticleSource The resulting particle
	*/
	ParticleSource PlayParticleById(int id, array<vector> position)
	{
		array<ParticleSource> tempArr = new array<ParticleSource>;
		PlayParticles(tempArr, ParticleList.GetParticleFullPath(id), position, 1);
		
		if (tempArr.Count() > 0)
			return tempArr[0];
		else
			return null;
	}
	
	/**
	\brief Manually get the particle at index
		\param index \p int Index of particle
		\return \p ParticleSource ParticleSource at given index
	*/
	proto native ParticleSource GetParticle(int index);
	
	/**
	\brief Manually get a portion of the particles in the pool
		\param particles \p array<ParticleSource> The resulting particles
		\param startIndex \p int Starting index
		\param count \p int Amount of particles to get
		\return \p int Amount of particles in outArray
	*/
	proto native int GetParticles(out array<ParticleSource> outArray, int startIndex, int count);
	
	/**
	\brief Manually get a portion of the particles in the pool
		\param startIndex \p int Starting index
		\param count \p int Amount of particles to get
		\return \p array<ParticleSource> The resulting particles
	*/
	array<ParticleSource> GetParticlesEx(int startIndex, int count)
	{
		array<ParticleSource> outArr = new array<ParticleSource>;
		GetParticles(outArr, startIndex, count);
		return outArr;
	}
	
	//@}
	
	
	
	/** \name Identification
 		Identification functionality
	*/
	//@{
	
	/**
	\brief Set a name for the ParticleManager to identify it more easily
		\param name \p string Name for ParticleManager
	*/
	proto native void SetName(string name);
	
	/**
	\brief Gets the name which is set for the ParticleManager, default is "ParticleSourceManager"
		\param name \p string Name of ParticleManager
	*/
	proto string GetName();
	
	/**
	\brief Gets the debug name for the ParticleManager
		\param dbgName \p string "name:id"
	*/
	proto string GetDebugNameNative();
	
	/**
	\brief Gets the debug name for the ParticleManager
		\return \p string "name:id"
	*/
	override string GetDebugName()
	{
		return GetDebugNameNative();
	}
	
	/**
	\brief Gets the ID for the ParticleManager
		\return \p int ID for the ParticleManager (different every restart, as it is the nth instance created since the start of the program)
	*/
	proto int GetCountID();
	
	/**
	\brief Gets the amount of ParticleManager that have been created since the start of the program
		\return \p int Amount of ParticleManager that have been created since the start of the program
	*/
	proto native static int GetStaticCount();
	
	/**
	\brief Gets the amount of ParticleManager that are currently existing
		\return \p int Amount of ParticleManager that are currently existing
	*/
	proto native static int GetStaticActiveCount();
	
	//@}
	
	
	
	/** \name Properties and state
		Obtain information about the state of the ParticleManager
	*/
	//@{
	
	/**
	\brief Gets the fixed maximum size of the pool
		\return \p int The fixed maximum size of the pool
	*/
	proto native int GetPoolSize();
	
	/**
	\brief Gets the amount of particles currently allocated
		\return \p int Amount of particles currently allocated
	*/
	proto native int GetAllocatedCount();
	
	/**
	\brief Gets the amount of virtual particles
		\return \p int Amount of virtual particles currently waiting to be turned into real particles
	*/
	proto native int GetVirtualCount();
	
	/**
	\brief Gets the amount of playing particles
		\return \p int Amount of currently playing particles
	*/
	proto native int GetPlayingCount();
	
	/**
	\brief Checks if the ParticleManager has allocated all slots in the pool
		\return \p bool True when the number of allocated particles is the same as the pool size
	*/
	proto native bool IsFinishedAllocating();
	
	//@}
	
	
	
	/** \name Script Events API
		Setting and getting of ScriptEvents
	*/
	//@{
	
	/**
	\brief Set the events
		\param events \p Managed The events to set
	*/
	private proto void SetScriptEvents(Managed events);
	
	/**
	\brief Get the events
		\return \p Managed If there is any events set, this will return them
	*/
	private proto Managed GetScriptEvents();
	
	/**
	\brief Get the events
		\return \p ParticleManagerEvents If there is any events set, this will return them so that additional functionality can be bound to them
	*/
	ParticleManagerEvents GetEvents()
	{
		return ParticleManagerEvents.Cast(GetScriptEvents());
	}
	
	//@}
	
	
	
	/** \name Events
		Events called from C++
	*/
	//@{
	
	void OnAllocation(array<ParticleSource> allocatedParticles)
	{
		GetEvents().Event_OnAllocation.Invoke(this, allocatedParticles);
	}
	
	void OnAllocationEnd()
	{
		GetEvents().Event_OnAllocationEnd.Invoke(this);
	}
	
	//@}
}