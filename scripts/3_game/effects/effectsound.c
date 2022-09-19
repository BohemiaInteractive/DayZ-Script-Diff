/**
\brief Wrapper class for managing sound through SEffectManager
*/
class EffectSound : Effect
{
	/** \name Event invokers
	*	ScriptInvonkers for certain events
	*/
	//@{
	ref ScriptInvoker Event_OnSoundWaveStarted		= new ScriptInvoker();
	ref ScriptInvoker Event_OnSoundWaveEnded		= new ScriptInvoker();
	ref ScriptInvoker Event_OnSoundFadeInStopped	= new ScriptInvoker();
	ref ScriptInvoker Event_OnSoundFadeOutStarted	= new ScriptInvoker();
	//@}
	
	/** \name Sound objects and structures
	*	Objects and structures for the sound
	*/
	//@{
	protected ref SoundParams			m_SoundParams;
	protected ref SoundObjectBuilder	m_SoundObjectBuilder;
	protected ref SoundObject			m_SoundObject;
	protected AbstractWave				m_SoundWaveObject;
	//@}
	
	/** \name Generic data
	*	Generic data for the sound
	*/
	//@{
	protected Object					m_SoundParent;
	protected WaveKind					m_SoundWaveKind;
	protected string					m_SoundSetName;	
	protected bool						m_SoundLoop;
	protected bool						m_SetEnvVariables;
	protected bool						m_SoundAutodestroy;
	protected bool						m_SoundWaveIsPlaying;
	protected float						m_SoundWaveLenght;
	protected float						m_SoundWaveVolume;
	protected float						m_SoundWaveVolumeMax;
	protected float						m_SoundWaveTime;
	//@}
	
	/** \name Fading data
	*	Data for fadein/fadeout for the sound
	*/
	//@{
	protected bool						m_SoundWaveStarting;
	protected bool						m_SoundWaveStopping;
	
	protected float						m_SoundFadeInDuration;
	
	protected float						m_SoundFadeOutStartTime;
	protected float						m_SoundFadeOutDuration;
	protected float						m_SoundFadeOutInitVolume;
	//@}
	
	
	
	/**
	\brief ctor
	*/
	void EffectSound()
	{
		m_SoundWaveKind = WaveKind.WAVEEFFECTEX;
		m_SoundWaveVolume = 0;
		m_SoundWaveVolumeMax = 1;
		m_SoundAutodestroy = false;
		m_SoundWaveStopping = false;
	}
	
	/**
	\brief dtor
	*/
	void ~EffectSound()
	{

	}
	
	/**
	\brief init
	*/
	override void InitEffect()
	{
		super.InitEffect();
		
		// These will be called by the sound events
		Event_OnStarted.Remove(Event_OnEffectStarted);
		Event_OnStopped.Remove(Event_OnEffectEnded);
	}
	
	/**
	\brief Override when getting debug information
	*/
	override string GetDebugName()
	{
		string identifier;
		if (m_SoundSetName != "")
		{
			identifier = m_SoundSetName;
		}
		else
		{
			identifier = "NO_SOUNDSET";
		}
		
		return string.Format("%1:%2", super.GetDebugName(), identifier);
	}
	
	
	
	/** \name EffectType
		Information about what type of effect the Effect is, without the need for casting
	*/
	//@{
	
	/**
	\brief Get what type of effect the Effect is
		\return \p EffectType What type of effect the Effect is
	*/
	override EffectType GetEffectType()
	{
		return EffectType.SOUND;
	}
	
	/**
	\brief Check whether the Effect is EffectSound without casting
		\return \p bool Whether the Effect is EffectSound
	*/
	override bool IsSound()
	{
		return true;
	}
	
	//@}
	
	
	
	/** \name Playback
	*	Methods to Play/Stop sound
	*	Generally, SEffectManager.PlaySound methods are used instead of SoundPlay
	*/
	//@{
	
	/**
	\brief Plays sound
		\param params \p SoundParams Sound Parameters for the sound
		\return \p bool Whether the sound will start playing
	*/
	bool SoundPlayEx(out SoundParams params)
	{
		super.Start();
		
		if (m_SoundSetName != "")
		{
			if ( SoundLoadEx(params) )
			{
				if (m_SetEnvVariables && m_SoundParams)
				{
					m_SoundObjectBuilder.UpdateEnvSoundControllers(GetPosition());
					m_SoundObject = m_SoundObjectBuilder.BuildSoundObject();
					m_SoundObject.SetKind( m_SoundWaveKind );
				}
				
				if ( m_SoundObject )
				{
					SetCurrentPosition(GetPosition(), false);
					m_SoundWaveObject = GetGame().GetSoundScene().Play3D( m_SoundObject, m_SoundObjectBuilder );
					if( !m_SoundWaveObject )
						return false;

					// Wait for header to be loaded before asking for its length, else we block the main thread
					if (m_SoundWaveObject.IsHeaderLoaded())
						ValidateSoundWave();
					else
						m_SoundWaveObject.GetEvents().Event_OnSoundWaveHeaderLoaded.Insert(ValidateSoundWave);					
					
					return true;
				}
				else
				{
					SoundError("m_SoundObject is null.");
				}
			}
		}
		
		return false;
	}
	
	/**
	\brief Plays sound
		\return \p bool Whether the sound will start playing
	*/
	bool SoundPlay()
	{
		SoundParams params;
		return SoundPlayEx(params);
	}
	
	/**
	\brief Plays sound
	*/
	override void Start()
    {		
		SoundPlay();
	}
	
	/**
	\brief Stops sound
		\note Will not be immediately if a SoundFadeOutDuration was set
	*/
	void SoundStop()
	{
		super.Stop();
		
		if ( IsSoundPlaying() )
		{
			if ( m_SoundFadeOutDuration > 0 && !m_SoundWaveStopping )
			{
				m_SoundWaveStopping = true;
				m_SoundWaveStarting = false;
				m_SoundFadeOutStartTime = m_SoundWaveTime;
			}
			else
			{
				m_SoundWaveObject.Stop();
			}
		}
		else
		{
			SoundReset();
		}	
	}
	
	/**
	\brief Stops sound
	*/
	override void Stop()
    {
		SoundStop();		
	}
	
	/**
	\brief Resets EffectSound
	*/
	protected void SoundReset()
	{
		m_IsPlaying					= false;
		m_SoundWaveIsPlaying		= false;
		m_SoundWaveVolume			= m_SoundWaveVolumeMax;
		m_SoundWaveTime				= 0;
		m_SoundFadeOutInitVolume	= 0;
		m_SoundFadeOutStartTime		= 0;
		
		if ( m_SoundWaveObject )
		{
			m_SoundWaveObject.Stop();
			m_SoundWaveObject.SetVolumeRelative( m_SoundWaveVolumeMax );
		}
	}
	
	/**
	\brief Get whether EffectSound is currently playing
		\return \p bool Whether EffectSound is currently playing
	*/
	bool IsSoundPlaying()
	{
		return m_SoundWaveIsPlaying;
	}
	
	/**
	\brief Returns true when the effect is playing, false otherwise
	*/
	override bool IsPlaying()
	{
		return IsSoundPlaying(); // Just in case, as that's what used to be the actual way to check
	}
	
	//@}
	
	
	
	/** \name Sound load
		Methods regarding the loading of the sound, used in SoundPlay
	*/
	//@{
	
	/**
	\brief Loads in the sound when it is requested for playing through 'SoundPlayEx'
		\param params \p SoundParams Possibility of passing in an already existing SoundParams, else one will get created
	*/
	bool SoundLoadEx(out SoundParams params)
	{		
		if ( !m_SoundParams || !m_SoundParams.IsValid() )
		{
			if (!params)
			{
				params = new SoundParams( m_SoundSetName );
			}
			
			//Print("SoundLoad is loading..");
			m_SoundParams = params;
			if ( !m_SoundParams.IsValid() )
			{
				SoundError("Invalid sound set.");
				return false;
			}
			
			m_SoundObjectBuilder = new SoundObjectBuilder( m_SoundParams );
			if (m_SetEnvVariables)
			{
				m_SoundObjectBuilder.UpdateEnvSoundControllers(GetPosition());
			}
			
			m_SoundObject = m_SoundObjectBuilder.BuildSoundObject();
			
			if ( m_SoundObject )
			{
				m_SoundObject.SetKind( m_SoundWaveKind );
			}
			else
			{
				SoundError("m_SoundObject is null.");
			}
		}
		else
		{
			//Print("SoundLoad is loaded.");
		}
		
		return true;
	}

	/**
	\brief Loads in the sound when it is requested for playing
		\note Legacy, backwards compatibility
	*/
	bool SoundLoad()
	{
		SoundParams params;
		return SoundLoadEx(params);
	}
	
	/**
	\brief Helper for checking if params are valid
	*/
	bool IsSoundValid()
	{
		return m_SoundParams.IsValid();
	}

	/**
	\brief Gets called to fill in the necessary data when the header has finished loading
		\note Called from 'SoundPlayEx'
	*/
	protected void ValidateSoundWave()
	{
		m_SoundWaveLenght = m_SoundWaveObject.GetLength();
					
		if ( SoundWaveValidation() )
		{
			if ( m_SoundFadeInDuration > 0 )
			{
				m_SoundWaveObject.SetVolumeRelative( 0 );
				m_SoundFadeOutStartTime = m_SoundWaveLenght - m_SoundFadeInDuration;
			}
						
			SetSoundLoop( m_SoundLoop );
						
			m_SoundWaveStarting = true;
						
			AbstractWaveEvents events = m_SoundWaveObject.GetEvents();
			events.Event_OnSoundWaveStarted.Insert( Event_OnSoundWaveStarted );
			events.Event_OnSoundWaveEnded.Insert( Event_OnSoundWaveEnded );
						
			UpdateEvents();
		}
		else
		{
			m_SoundWaveObject.Stop();
		}
	}

	/**
	\brief Validation of fade settings
		\note Called from 'ValidateSoundWave'
	*/
	protected bool SoundWaveValidation()
	{
		bool valid = true;
		
		if ( m_SoundFadeInDuration > GetSoundWaveLength() )
		{
			SoundError("FadeIn is longer than sound wave length.");
			valid = false;
		}
		
		if ( m_SoundFadeOutDuration > GetSoundWaveLength() )
		{
			SoundError("FadeOut is longer than sound wave length.");
			valid = false;
		}
		
		if ( m_SoundFadeOutDuration + m_SoundFadeInDuration > GetSoundWaveLength() )
		{
			SoundError("FadeIn & FadeOut are longer than sound wave length.");
			valid = false;
		}
		
		return valid;
	}

	/**
	\brief Enables the frame event on the EffectSound
		\note Called from 'ValidateSoundWave' when all is successful
		\note So this is effectively enabling frame event on all valid EffectSound
	*/
	protected void UpdateEvents()
	{
		if ( m_SoundWaveObject )
		{
			SetEnableEventFrame(true);
		}
		else
		{
			SetEnableEventFrame(false);
		}
	}

	//@}
	
	
	
	/** \name Events
		Various events that can be overriden for custom behaviour
	*/
	//@{
	
	/**
	\brief Event called on frame when enabled by SetEnableEventFrame(true)
		\note Is always enabled on sound
		\note Called from SEffectManager.Event_OnFrameUpdate in MissionGameplay.OnUpdate
		\param time_delta \p float Time passed since the previous frame
	*/
	override void Event_OnFrameUpdate(float time_delta)
	{
		if ( IsSoundPlaying() )
		{
			// Make the sound follow the parent
			if ( m_SoundParent )
			{
				SetCurrentPosition( m_SoundParent.ModelToWorld( GetLocalPosition() ) );
			}
			
			// FadeIn
			if ( m_SoundWaveStarting )
			{
				if ( m_SoundFadeInDuration > 0 )
				{
					SetSoundVolume( GetSoundVolume() + (time_delta / m_SoundFadeInDuration) );
					
					if ( GetSoundVolume() >= m_SoundWaveVolumeMax )
					{
						Event_OnSoundFadeInStopped();
						SetSoundVolume( m_SoundWaveVolumeMax );
						m_SoundWaveStarting = false;
					}
				}
				else
				{
					SetSoundVolume( m_SoundWaveVolumeMax );
					m_SoundWaveStarting = false;
				}
			}
			
			// FadeOut
			if ( m_SoundWaveStopping )
			{
				if ( m_SoundFadeOutDuration > 0 )
				{
					if ( m_SoundFadeOutInitVolume == 0 )
					{
						m_SoundFadeOutInitVolume = GetSoundVolume();
						Event_OnSoundFadeOutStarted();
					}
					SetSoundVolume( GetSoundVolume() - (m_SoundFadeOutInitVolume / m_SoundFadeOutDuration * time_delta) );
				}
				else
				{
					SetSoundVolume( 0 );
				}
				
				if ( GetSoundVolume() <= 0 )
				{		
					if ( m_SoundWaveObject )
					{
						m_SoundWaveObject.Stop();
					}
				}
			}
			
			// Counting timer here because loop play
			m_SoundWaveTime += time_delta;
		}
	}
	
	/**
	\brief Event called from SEffectManager when the Effect is registered
		\note Should only ever be called by SEffectManager!
		\param id \p int ID registered in SEffectManager
	*/
	override void Event_OnRegistered(int id)
	{
		super.Event_OnRegistered(id);
		
		Event_OnSoundWaveEnded.Insert( SEffectManager.Event_OnSoundWaveEnded );
	}
	
	/**
	\brief Event called from SEffectManager when the Effect is unregistered
		\note Should only ever be called by SEffectManager!
	*/
	override void Event_OnUnregistered()
	{
		super.Event_OnUnregistered();
		
		Event_OnSoundWaveEnded.Remove( SEffectManager.Event_OnSoundWaveEnded );
	}
	
	/**
	\brief Event called when sound starts playing
		\note Inserted into events of m_SoundWaveObject
	*/
	void Event_OnSoundWaveStarted()
	{
		m_SoundWaveIsPlaying = true;
		
		Event_OnSoundWaveStarted.Invoke(this);
		
		Event_OnEffectStarted();
	}
	
	/**
	\brief Event called when sound stops playing
		\note Inserted into events of m_SoundWaveObject
	*/
	void Event_OnSoundWaveEnded()
	{
		m_SoundWaveIsPlaying = false;
		
		Event_OnSoundWaveEnded.Invoke(this);
		
		Event_OnEffectEnded();
	}
	
	/**
	\brief Event called when sound fade in stops
		\note Called from Event_OnFrameUpdate
	*/
	void Event_OnSoundFadeInStopped()
	{
		Event_OnSoundFadeInStopped.Invoke(this);
	}
	
	/**
	\brief Event called when sound fade out starts
		\note Called from Event_OnFrameUpdate
	*/
	void Event_OnSoundFadeOutStarted()
	{
		Event_OnSoundFadeOutStarted.Invoke(this);
	}
	
	//@}
	
	
	
	/** \name AutoDestroy
		Methods regarding automatic cleanup on stop
	*/
	//@{
	
	/**
	\brief Sets whether Effect automatically cleans up when it stops
		\note This means that it will be unregistered from SEffectManager as well
		\param auto_destroy \p bool Whether Effect automatically cleans up when it stops
	*/
	override void SetAutodestroy(bool auto_destroy)
	{
		super.SetAutodestroy(auto_destroy);
		m_SoundAutodestroy = auto_destroy;		
	}
	
	/**
	\brief Get whether Effect automatically cleans up when it stops
		\return \p bool Whether Effect automatically cleans up when it stops
	*/
	override bool IsAutodestroy()
	{
		return IsSoundAutodestroy();
	}
	
	/**
	\brief Sets whether EffectSound automatically cleans up when sound stops
		\param auto_destroy \p bool Whether EffectSound automatically cleans up when sound stops
	*/
	void SetSoundAutodestroy(bool auto_destroy)
	{
		SetAutodestroy(auto_destroy);
	}
	
	/**
	\brief Get whether EffectSound automatically cleans up when sound stops
		\return \p bool Whether EffectSound automatically cleans up when sound stops
	*/
	bool IsSoundAutodestroy()
	{
		return m_SoundAutodestroy;
	}
	
	//@}

	
	
	/** \name Generic API
		Setters and getters for generic data and properties
	*/
	//@{
	
	/**
	\brief Set parent for the sound to follow
		\note The position of the sound is adjusted to the parent in Event_OnFrameUpdate...
		\note There is no real parenting with sound, so the setters and getters for parents do the exact same
		\param parent_obj \p Object The parent for the sound to follow
	*/
	override void SetParent(Object parent_obj)
	{
		super.SetParent(parent_obj); // ...
		m_SoundParent = parent_obj; // ......
	}
	
	/**
	\brief Get parent for the EffectSound
		\return \p Object The parent for the EffectSound
	*/
	override Object GetParent()
	{
		// .........
		if (m_SoundParent)
			return m_SoundParent;
		else
			return super.GetParent();
	}
	
	/**
	\brief Set parent for the sound to follow
		\note The position of the sound is adjusted to the parent in Event_OnFrameUpdate...
		\note There is no real parenting with sound, so the setters and getters for parents do the exact same
		\param parent_obj \p Object The parent for the sound to follow
		\param updateCached \p bool Whether to update the cached variable
	*/
	override void SetCurrentParent(Object parent_obj, bool updateCached = true)
	{
		super.SetCurrentParent(parent_obj, updateCached); // ...
		m_SoundParent = parent_obj; // ......
	}
	
	/**
	\brief Get parent for the EffectSound
		\note There is no real parenting with sound, so the setters and getters for parents do the exact same
		\return \p Object The parent for the EffectSound
	*/
	override Object GetCurrentParent()
	{
		// .........
		if (m_SoundParent)
			return m_SoundParent;
		else
			return super.GetParent(); // Yes, intentionally this one
	}
	
	/**
	\brief Set the world position of the managed sound
		\param pos \p vector The world position of the managed sound
		\param updateCached \p bool Whether to update the cached variable
	*/
	override void SetCurrentPosition( vector pos, bool updateCached = true )
	{
		super.SetCurrentPosition(pos, updateCached);
		
		if (m_SoundObject)
			m_SoundObject.SetPosition(pos);
	}
	
	/**
	\brief Get the current world position of the managed sound
		\return \p vector The current world position of the managed sound
	*/
	override vector GetCurrentPosition()
	{
		if (m_SoundObject)
			return m_SoundObject.GetPosition();
		else
			return super.GetCurrentPosition();
	}
	
	/**
	\brief Set the current local position of the managed sound
		\param pos \p vector The current local position for the managed sound
		\param updateCached \p bool Whether to update the cached variable
	*/
	override void SetCurrentLocalPosition( vector pos, bool updateCached = true )
    {
		super.SetCurrentLocalPosition(pos, updateCached);
				
		if (m_SoundObject)
		{
			Object parent = GetParent();
			
			if (parent)
				m_SoundObject.SetPosition(parent.ModelToWorld(pos));
			else
				m_SoundObject.SetPosition(pos);
		}
    }

	/**
	\brief Get the current local position of the managed sound
		\return \p vector The current local position of the managed sound
	*/
	override vector GetCurrentLocalPosition()
	{
		if (m_SoundObject)
		{
			Object parent = GetParent();
			
			if (parent)
				return parent.WorldToModel(m_SoundObject.GetPosition());
			else
				return m_SoundObject.GetPosition();
		}
		else
			return super.GetCurrentLocalPosition();
	}
	
	/**
	\brief Set WaveKind for the sound
		\note Needs to be set before playing
		\param wave_kind \p WaveKind The WaveKind for the sound
	*/
	void SetSoundWaveKind(WaveKind wave_kind)
	{
		m_SoundWaveKind = wave_kind;
	}
	
	/**
	\brief Set soundset for the sound
		\note Needs to be set before playing
		\param snd \p string Name of the soundset to play
	*/
	void SetSoundSet(string snd)
	{
		m_SoundSetName = snd;
	}
	
	/**
	\brief Get soundset for the sound
		\return \p string Name of the soundset
	*/
	string GetSoundSet()
	{
		return m_SoundSetName;
	}
	
	/**
	\brief Set if the sound loops
		\param loop \p bool Whether the sound should loop
	*/
	void SetSoundLoop(bool loop)
	{
		m_SoundLoop = loop;
		
		if ( m_SoundWaveObject )
			m_SoundWaveObject.Loop( loop );
	}
	
	/**
	\brief Sets whether UpdateEnvSoundControllers needs to be called during Loading
		\param setEnvVariables \p bool Whether UpdateEnvSoundControllers is called
	*/
	void SetEnviromentVariables(bool setEnvVariables)
	{
		m_SetEnvVariables = setEnvVariables;
	}	

	/**
	\brief Get the sound wave length
		\note Legacy, exists for backwards compatibility
		\return \p float The sound wave length
	*/
	float GetSoundWaveLenght()
	{
		return GetSoundWaveLength();
	}
	
	/**
	\brief Get the sound wave length
		\return \p float The sound wave length
	*/
	float GetSoundWaveLength()
	{
		return m_SoundWaveLenght;
	}
	
	/**
	\brief Set the RELATIVE volume for the sound
		\param volume \p float The relative volume for the sound
	*/
	void SetSoundVolume(float volume)
	{
		m_SoundWaveVolume = volume;
		if ( m_SoundWaveObject )
			m_SoundWaveObject.SetVolumeRelative( volume );
	}
	
	/**
	\brief Get the RELATIVE volume set by 'SetSoundVolume'
		\return \p float The relative volume for the sound set by 'SetSoundVolume'
	*/
	float GetSoundVolume()
	{
		return m_SoundWaveVolume;
	}
	
	/**
	\brief Set the sound max volume
		\warning Seems to purely be used for fade in effect, rather than really setting the max volume...
		\warning Adjusts the current volume to this value as well
		\param volume \p float The maximum volume for the sound
	*/
	void SetSoundMaxVolume(float volume)
	{
		m_SoundWaveVolumeMax = volume;
		if ( m_SoundWaveObject )
			m_SoundWaveObject.SetVolumeRelative( m_SoundWaveVolume );
	}

	/**
	\brief Get the time since EffectSound started playing
		\warning May not reflect the actual time of the sound, as it can start at negative time to simulate speed of sound
		\return \p float The time since EffectSound started playing
	*/
	float GetSoundWaveTime()
	{
		return m_SoundWaveTime;
	}
	
	/**
	\brief Set the sound fade in duration
		\param fade_in \p float The fade in duration
	*/
	void SetSoundFadeIn(float fade_in)
	{
		m_SoundFadeInDuration = fade_in;
	}
	
	/**
	\brief Set the sound fade out duration
		\param fade_out \p float The fade out duration
	*/
	void SetSoundFadeOut(float fade_out)
	{
		m_SoundFadeOutDuration = fade_out;
	}
	
	//@}
	
	
	
	/**
	\brief Helper for throwing sound errors
	*/
	protected void SoundError(string err_msg)
	{
		ErrorEx(string.Format("%1: SoundSetName: '%2' :: %3", this, m_SoundSetName, err_msg));
	}
}