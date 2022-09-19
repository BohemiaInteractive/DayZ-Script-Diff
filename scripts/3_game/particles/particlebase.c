/**
\brief Invokers for ParticleBase events, called from events
	@code
	void EnableOnEndPrint(ParticleBase psrc)
	{
		psrc.GetEvents().Event_OnParticleEnd.Insert(PrintParticleEnded);
	}
	
	void PrintParticleEnded(ParticleBase psrc)
	{
		Print(string.Format("%1 ended.", psrc.GetDebugNameNative());
	}
	@endcode
*/
class ParticleEvents
{
	/**
	\brief Called when particle starts playing
		\note Particle: Called when any Play method has been called
		\note ParticleSource: Called when PlayParticleNative is successful
	*/
	ref ScriptInvoker Event_OnParticleStart = new ScriptInvoker();
	/**
	\brief Called when particle stops playing
		\note Particle: Called when any Stop method has been called or when lifetime is over
		\note ParticleSource: Called when StopParticleNative is successful, when the particle ends and when the particle is destroyed while it is playing
	*/
	ref ScriptInvoker Event_OnParticleStop = new ScriptInvoker();
	/**
	\brief Called when particle is reset
		\note Particle: Not present, there is no reset functionality
		\note ParticleSource: Called when ResetParticleNative is successful
	*/
	ref ScriptInvoker Event_OnParticleReset = new ScriptInvoker();
	/**
	\brief Called when particle ends
		\note Particle: Called when lifetime is over and there are no emitors active anymore
		\note ParticleSource: Called when particle ends naturally or after the particle is stopped and there are no emitors active anymore
		\warning Looped never ends naturally and need to be stopped
	*/
	ref ScriptInvoker Event_OnParticleEnd = new ScriptInvoker();
	/**
	\brief Called when particle receives a parent
		\note Particle: Not present
	*/
	ref ScriptInvoker Event_OnParticleParented = new ScriptInvoker();
	/**
	\brief Called when particle is orphaned
		\note Particle: Not present
	*/
	ref ScriptInvoker Event_OnParticleUnParented = new ScriptInvoker();
}

/**
\brief Engine base class with internal functionality 
	\note Is NOT intended for direct creation
		* As this does not have an actual particle
		* Use either Particle or ParticleSource
*/
class ParticleBase : Entity
{
	//! Whether the particle is currently playing
	protected 	bool 	m_IsPlaying;
	
	//! Event invokers
	protected ref ParticleEvents m_EventInvokers;
	
	
	
	//! ctor
	void ParticleBase()
	{
		m_EventInvokers = new ParticleEvents();
	}
	
	
	
	//! Have script recognize this as a Particle without casting
	override bool IsParticle()
	{
		return true;
	}
	
	
	
	/** \name Playback
 		Methods regarding playing/stopping of particle
	*/
	//@{

	/**
	\brief Method to tell the particle to start playing
		\param particle_id \p int Particle ID registered in ParticleList to start playing
		\return \p bool Whether the particle successfully started
	*/
	void PlayParticle(int particle_id = -1)
	{
		ErrorEx("Not implemented.", ErrorExSeverity.WARNING);
	}
	
	/**
	\brief Method to tell the particle to start playing
		\param particle_id \p int Particle ID registered in ParticleList to start playing
		\param flags \p int Flags to pass to the playing
		\return \p bool Whether the particle successfully started
	*/
	bool PlayParticleEx(int particle_id = -1, int flags = 0)
	{
		ErrorEx("Not implemented.", ErrorExSeverity.WARNING);
		return false;
	}

	/**
	\brief Method to tell the particle to stop playing
		\param flags \p int Flags to pass to the stopping
		\return \p bool Whether the particle successfully stopped
	*/
	bool StopParticle(int flags = 0)
	{
		ErrorEx("Not implemented.", ErrorExSeverity.WARNING);
		return false;
	}
	
	/**
	\brief Method to tell the particle to reset
		\return \p bool Whether the particle successfully reset
	*/
	bool ResetParticle()
	{
		ErrorEx("Not implemented.", ErrorExSeverity.WARNING);
		return false;
	}

	/**
	\brief Method to tell the particle to restart (reset + play)
		\return \p bool Whether the particle successfully restarted
	*/
	bool RestartParticle()
	{
		ErrorEx("Not implemented.", ErrorExSeverity.WARNING);
		return false;
	}

	/**
	\brief Ask if the particle is still playing
		\return \p bool Whether the particle is playing
	*/
	bool IsParticlePlaying()
	{
		ErrorEx("Not implemented.", ErrorExSeverity.WARNING);
		return false;
	}
	
	//@}
	
	
	
	/**
	\brief Get the events
		\return \p ParticleEvents If there is any events set, this will return them so that additional functionality can be bound to them
	*/
	ParticleEvents GetEvents()
	{
		return m_EventInvokers;
	}
	
	
	
	/** \name Events
	*	ParticleBase events
	*	For ParticleSource, these are handed on C++ side
	*	For more information, read ParticleEvents
	*/
	//@{ 
	
	/**
	\brief Event when the particle starts
	*/
	protected void OnParticleStart()
	{
		m_IsPlaying = true;
		GetEvents().Event_OnParticleStart.Invoke(this);
	}
	
	/**
	\brief Event when the particle stops
	*/
	protected void OnParticleStop()
	{
		m_IsPlaying = false;
		GetEvents().Event_OnParticleStop.Invoke(this);
	}
	
	/**
	\brief Event when the particle is restarted
	*/
	protected void OnParticleReset()
	{
		GetEvents().Event_OnParticleReset.Invoke(this);
	}
	
	/**
	\brief Event when the particle ends
		\note Looping particles will never end
	*/
	protected void OnParticleEnd()
	{
		GetEvents().Event_OnParticleEnd.Invoke(this);
	}
	
	/**
	\brief Event when the particle receives a parent
	*/
	protected void OnParticleParented(IEntity parent)
	{
		GetEvents().Event_OnParticleParented.Invoke(this);
	}
	
	/**
	\brief Event when the particle is orphaned
	*/
	protected void OnParticleUnParented(IEntity parent)
	{
		GetEvents().Event_OnParticleUnParented.Invoke(this);
	}
	
	//@}
}