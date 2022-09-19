//! Enum to determine what type of effect the Effect is
enum EffectType
{
	//! Plain Effect base
	NONE,
	//! EffectSound
	SOUND,
	//! EffectParticle
	PARTICLE,
}

/**
\brief Base wrapper class for managing effects (Particles, Sound) through SEffectManager
	\note This is just a base class, not intended for direct usage
*/
class Effect : Managed
{
	/** \name Event invokers
	*	ScriptInvonkers for certain events
	*/
	//@{
	ref ScriptInvoker Event_OnStarted		= new ScriptInvoker();
	ref ScriptInvoker Event_OnStopped		= new ScriptInvoker();
	ref ScriptInvoker Event_OnEffectStarted		= new ScriptInvoker();
	ref ScriptInvoker Event_OnEffectEnded		= new ScriptInvoker();
	//@}
	
	/** \name Generic data
	*	Generic data for the Effect
	*/
	//@{
	//! Whether the Effect cleans up after itself when stopped
	protected bool				m_IsAutodestroy;
	//! Whether the Destroy process has already been called
	protected bool				m_IsPendingDeletion;
	//! Whether the Effect is currently playing
	protected bool				m_IsPlaying;
	//! Cached parent
	protected Object 			m_ParentObject;
	//! Cached world position
	protected vector    		m_Position;
	//@}
	
	/** \name SEffectManager data
	*	Data filled in by SEffectManager to identify it when it is registered
	*/
	//@{
	//! ID of effect, given by SEffectManager when registered (automatically done when playing through it)
	protected int				m_ID;
	//! Whether the effect is registered in SEffectManager
	protected bool				m_IsRegistered;
	//@}
	
	/** \name Attachment data
	*	Cached settings set through 'SetAttachment...' methods
	*	Does not necessarily reflect the current state when EffectParticle
	*/
	//@{	
	//! Cached local pos
	protected vector    		m_LocalPos;
	//! Local orientation set by SetAttachedLocalOri, only used by EffectParticle
	protected vector    		m_LocalOri;
	//@}
	
	
	
	/**
	\brief ctor
	*/
	void Effect()
	{
		if (GetGame().IsDedicatedServer())
		{
			ErrorEx("Created Effect on server.", ErrorExSeverity.WARNING);
		}
		
		InitEffect();
	}
	
	/**
	\brief dtor
	*/
	void ~Effect()
	{
		// Safety
		if ( IsRegistered() )
			SEffectManager.EffectUnregister(GetID());
		
		// Certain effects need to be stopped to clean up properly
		Stop();
		
		// Another safety
		SetEnableEventFrame(false);
	}
	
	/**
	\brief init
	*/
	void InitEffect()
	{
		Event_OnStarted.Insert(Event_OnEffectStarted);
		Event_OnStarted.Insert(ValidateStart);
		Event_OnStopped.Insert(Event_OnEffectEnded);
	}
	
	
	/** \name EffectType
		Information about what type of effect the Effect is, without the need for casting
	*/
	//@{
	
	/**
	\brief Get what type of effect the Effect is
		\return \p EffectType What type of effect the Effect is
	*/
	EffectType GetEffectType()
	{
		return EffectType.NONE;
	}
	
	/**
	\brief Check whether the Effect is EffectSound without casting
		\return \p bool Whether the Effect is EffectSound
	*/
	bool IsSound()
	{
		return false;
	}
	
	/**
	\brief Check whether the Effect is EffectParticle without casting
		\return \p bool Whether the Effect is EffectParticle
	*/
	bool IsParticle()
	{
		return false;
	}
	
	//@}
	
	
	
	/** \name Playback
	*	Methods to Play/Stop Effect
	*	Generally, SEffectManager.Play methods are used instead of Start
	*/
	//@{
	
	/**
	\brief Plays all elements this effects consists of
		\note Is called by SEffectManager.Play methods
	*/
    void Start()
    {
		// It is already playing!
		if (IsPlaying())
			return;
		
		Event_OnStarted();
		// I can't call this from inside the method with same name
		// because that method is often overriden without super......
		Event_OnStarted.Invoke(this);		
	}
	
	/**
	\brief Validation whether an effect truly started playing or if the Effect should stop as none is present
		\note Override this when inheriting to create own validation check
		\note Is called from Event_OnStarted invoker after Event_OnStarted has been performed
	*/
	void ValidateStart()
	{
		
	}
	
	/**
	\brief Stops all elements this effect consists of
		\note Alternatively use SEffectManager.Stop( effect_id )
	*/
    void Stop()
    {
		// It is not playing!
		if (!IsPlaying())
			return;
		
		Event_OnStopped();
		// Yes, that event is new, but let's keep up some consistency
		Event_OnStopped.Invoke(this);		
    }
	
	/**
	\brief Returns true when the Effect is playing, false otherwise
	*/
	bool IsPlaying()
	{
		return m_IsPlaying;
	}
	
	//@}
	
	
	
	/** \name Destroy
		Methods regarding automatic cleanup
	*/
	//@{
	
	/**
	\brief Cleans up the Effect, including unregistering if needed
		\note Will stop the Effect and queue up the deletion in the callqueue
		\note Is intended for usage from within the Effect itself, use SEffectManager.DestroyEffect when working from a pointer
	*/
	protected void Destroy()
	{
		// Already queued
		if (IsPendingDeletion())
			return;
		
		// Mark it to prevent queuing it up multiple times or get stuck in a call loop
		m_IsPendingDeletion = true;
		
		// Stop it, so that the effects can clean up themselves
		// Since if for example this is EffectParticle and the particle is looping
		// It NEEDS to be stopped to clean up the Particle
		Stop();
		
		// Queue up the destroying, as we should not do it while we are accessing it here
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(SEffectManager.DestroyEffect, this);
		}
	}
	
	/**
	\brief Sets whether Effect automatically cleans up when it stops
		\note This means that it will be unregistered from SEffectManager as well
		\param auto_destroy \p bool Whether Effect automatically cleans up when it stops
	*/
	void SetAutodestroy(bool auto_destroy)
	{
		m_IsAutodestroy = auto_destroy;
	}
	
	/**
	\brief Get whether Effect automatically cleans up when it stops
		\return \p bool Whether Effect automatically cleans up when it stops
	*/
	bool IsAutodestroy()
	{
		return m_IsAutodestroy;
	}
	
	/**
	\brief Get whether the Effect is queued up for being cleaned up
		\return \p bool Whether the Effect is queued up for being cleaned up
	*/
	bool IsPendingDeletion()
	{
		return m_IsPendingDeletion;
	}
	
	//@}
	
	
	
	/**
	\brief Enable Event_OnFrameUpdate for the effect
		\note Read SEffectManager.Event_OnFrameUpdate for more info
	*/
	void SetEnableEventFrame(bool enable)
	{
		if ( enable )
		{
			SEffectManager.Event_OnFrameUpdate.Insert(Event_OnFrameUpdate);			
		}
		else
		{
			SEffectManager.Event_OnFrameUpdate.Remove(Event_OnFrameUpdate);
		}
	}
	
	
	
	/** \name Events
		Various events that can be overriden for custom behaviour
	*/
	//@{
	
	/**
	\brief Event used when Start was called
	*/
	void Event_OnStarted()
	{
		// Override this method for own use
	}
	
	/**
	\brief Event used when Stop was called
	*/
	void Event_OnStopped()
	{
		// Override this method for own use
	}
	
	/**
	\brief Event used when the actual effect started playing
	*/
	void Event_OnEffectStarted()
	{
		m_IsPlaying = true;
		
		Event_OnEffectStarted.Invoke(this);
	}
	
	/**
	\brief Event used when the actual effect stopped playing
	*/
	void Event_OnEffectEnded()
	{
		m_IsPlaying = false;
		
		Event_OnEffectEnded.Invoke(this);
		
		if ( IsAutodestroy() )
		{
			Destroy();
		}
	}
	
	/**
	\brief Event called on frame when enabled by SetEnableEventFrame(true)
		\note Called from SEffectManager.Event_OnFrameUpdate in MissionGameplay.OnUpdate
		\param time_delta \p float Time passed since the previous frame
	*/
	void Event_OnFrameUpdate(float time_delta)
	{
		// Override this method for own use
	}
	
	/**
	\brief Event called from SEffectManager when the Effect is registered
		\note Should only ever be called by SEffectManager!
		\param id \p int ID registered in SEffectManager
	*/
	void Event_OnRegistered(int id)
	{
		SetID(id);
		m_IsRegistered = true;
	}
	
	/**
	\brief Event called from SEffectManager when the Effect is unregistered
		\note Should only ever be called by SEffectManager!
	*/
	void Event_OnUnregistered()
	{
		SetID(SEffectManager.INVALID_ID);
		m_IsRegistered = false;
	}
	
	/**
	\brief Event used when EffectParticle.CheckLifeSpan was called (DEPRECATED)
		\note So this is EffectParticle specific...
		\note EffectParticle.CheckLifeSpan is currently not in use
	*/
	void OnCheckUpdate()
	{

	}
	
	//@}
	
	
	
	/** \name Generic API
		Setters and getters for generic data and properties
	*/
	//@{
	
	/**
	\brief Set parent of the Effect
		\note Same as SetAttachmentParent, but more generic name
		\warning Only sets the cached variable, for immediate effect use SetCurrent variant
		\param parent_obj \p Object The parent for the Effect
	*/
	void SetParent(Object parent_obj)
	{
		m_ParentObject = parent_obj;
	}
	
	/**
	\brief Get parent of the Effect
		\note Same as GetAttachmentParent, but more generic name
		\warning Only gets the cached variable, for immediate effect use GetCurrent variant
		\return \p Object The parent of the Effect
	*/
	Object GetParent()
	{
		return m_ParentObject;
	}
	
	/**
	\brief Set current parent of the managed effect
		\note Same as SetAttachmentParent, but more generic name
		\param parent_obj \p Object The parent for the Effect
		\param updateCached \p bool Whether to update the cached variable
	*/
	void SetCurrentParent( Object parent_obj, bool updateCached = true )
	{
		if (updateCached)
			SetParent(parent_obj);
	}
	
	/**
	\brief Get the current parent of the managed Effect
		\return \p Object The current parent of the managed Effect
	*/
	Object GetCurrentParent()
	{
		return null;
	}
	
	/**
	\brief Set the world position of the Effect
		\warning Only sets the cached variable, for immediate effect use SetCurrent variant
		\param pos \p vector The world position for the Effect
	*/
	void SetPosition( vector pos )
    {
        m_Position = pos;
    }

	/**
	\brief Get the world position of the Effect
		\warning Only gets the cached variable, for immediate effect use GetCurrent variant
		\return \p vector The world position of the Effect
	*/
	vector GetPosition()
	{
		return m_Position;
	}
	
	/**
	\brief Set the current world position of the managed effect
		\param pos \p vector The current world position for the Effect
		\param updateCached \p bool Whether to update the cached variable
	*/
	void SetCurrentPosition( vector pos, bool updateCached = true )
    {
		if (updateCached)
        	SetPosition(pos);
    }
	
	/**
	\brief Get the current world position of the managed effect
		\return \p vector The current world position of the managed effect
	*/
	vector GetCurrentPosition()
	{
		return vector.Zero;
	}
	
	/**
	\brief Set the local position of the Effect
		\warning Only sets the cached variable, for immediate effect use SetCurrent variant
		\param pos \p vector The local position for the Effect
	*/
	void SetLocalPosition( vector pos )
    {
		m_LocalPos = pos;
    }

	/**
	\brief Get the local position of the Effect
		\warning Only gets the cached variable, for immediate effect use GetCurrent variant
		\return \p vector The lcoal position of the Effect
	*/
	vector GetLocalPosition()
	{
		return m_LocalPos;
	}
	
	/**
	\brief Set the current local position of the managed effect
		\param pos \p vector The current local position for the managed effect
		\param updateCached \p bool Whether to update the cached variable
	*/
	void SetCurrentLocalPosition( vector pos, bool updateCached = true )
    {
		if (updateCached)
        	SetLocalPosition(pos);
    }

	/**
	\brief Get the current local position of the managed effect
		\return \p vector The current local position of the managed effect
	*/
	vector GetCurrentLocalPosition()
	{
		return vector.Zero;
	}
	
	//@}
	
	
	
	/** \name Effect ID
		The ID of the effect when registered in SEffectManager
	*/
	//@{
	
	/**
	\brief Set the ID registered in SEffectManager
		\note Should only ever be called by Event_OnRegistered!
		\param id \p int ID registered in SEffectManager
	*/
	protected void SetID(int id)
	{
		m_ID = id;
	}
	
	/**
	\brief Get the ID registered in SEffectManager
		\return \p int ID registered in SEffectManager, or 0 (SEffectManager.INVALID_ID) when not registered
	*/
	int GetID()
	{
		return m_ID;
	}
	
	/**
	\brief Get whether this Effect is registered in SEffectManager
		\return \p bool Whether this Effect is registered in SEffectManager
	*/
	bool IsRegistered()
	{
		return m_IsRegistered;
	}
	
	//@}
	
	
	
	/** \name Attachment API
	*	Data to attach an Effect to a parent
	*	Mostly replaced by equivalents without 'Attachment' in name
	*	Mildly deprecated, exist for backwards compatibility
	*/
	//@{
	
	/**
	\brief Set parent for the Effect
		\param obj \p Object The parent for the Effect
	*/
	void SetAttachmentParent(Object obj)
	{
		SetParent(obj);
	}
		
	/**
	\brief Get the parent set by SetAttachmentParent
		\return \p Object The parent set by SetAttachmentParent
	*/
	Object GetAttachmentParent()
	{
		return GetParent();
	}
	
	/**
	\brief Set local pos for the Effect relative to the parent
		\param pos \p vector The local pos relative to the parent
	*/
	void SetAttachedLocalPos(vector pos)
	{
		SetLocalPosition(pos);
	}
	
	/**
	\brief Get the local pos set by SetAttachedLocalPos
		\return \p vector The local pos set by SetAttachedLocalPos
	*/
	vector GetAttachedLocalPos()
	{
		return GetLocalPosition();
	}
	
	/**
	\brief Set local orientation for the Effectparticle to attach to when the Effect is started
		\warning Only caches it into a variable to be used by Start, does not live update when called afterwards
		\note Overrides the orientation set by EffectParticle.SetOrientation
		\param ori \p vector The local orientation to use on Start
	*/
	void SetAttachedLocalOri(vector ori)
	{
		m_LocalOri = ori;
	}
	
	/**
	\brief Get the local orientation set by SetAttachedLocalOri
		\warning Is not necessarily the current local orientation
		\return \p vector The local orientation set by SetAttachedLocalOri
	*/
	vector GetAttachedLocalOri()
	{
		return m_LocalOri;
	}
	
	//@}
}