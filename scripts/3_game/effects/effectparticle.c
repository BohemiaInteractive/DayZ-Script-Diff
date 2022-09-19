/**
\brief Wrapper class for managing particles through SEffectManager
*/
class EffectParticle : Effect
{
	//! The main Particle effect that this Effect wrapper manages
	protected Particle 			m_ParticleObj;
	
	/** \name Generic data
	*	Generic data for the particle
	*/
	//@{
	//! The ID in the ParticleList to create Particle from
	protected int				m_ParticleID;
	//! Orientation set by SetOrientation
	protected vector			m_Orientation;
	//! Orientation setting to be used by the effect when the Effect starts
	protected bool    			m_ForceRotationRelativeToWorld;
	//@}
		
	/** \name DEPRECATED
	*	Simply exist because of backwards compatibility, might have never been used
	*/
	//@{
	protected vector    		m_ParticleOrientation;
	protected Object 			m_Object;
	//@}
	
	
	
	/**
	\brief ctor
	*/
	void EffectParticle()
	{
		
	}
	
	/**
	\brief dtor
	*/
	void ~EffectParticle()
	{

	}
		
	/**
	\brief init
	*/
	override void InitEffect()
	{
		super.InitEffect();
		
		// Would be neat, but since particles are often already playing
		// BEFORE they are even registered as the particle for the Effect
		// Better to just keep that one I guess..
		// Event_OnStarted.Remove(Event_OnEffectStarted);
		
		// Will be called by the particle events
		Event_OnStopped.Remove(Event_OnEffectEnded);
	}
	
	
	/**
	\brief Override when getting debug information
	*/
	override string GetDebugName()
	{
		string identifier;
		if (GetParticle())
		{
			identifier = GetParticle().GetDebugNameNative();
		}
		else
		{
			identifier = "NO_PARTICLE";
		}
		
		return string.Format("%1:%2:%3", super.GetDebugName(), m_ParticleID, identifier);
	}
	
	/**
	\brief Validation whether an effect truly started playing or if the Effect should stop as none is present
		\note Override this when inheriting to create own validation check
		\note Is called from Event_OnStarted invoker after Event_OnStarted has been performed
	*/
	override void ValidateStart()
	{
		if (!GetParticle())
		{
			ErrorEx(string.Format("No Particle started playing, stopping EffectParticle: %1", GetDebugName()), ErrorExSeverity.WARNING);
			Stop();
		}
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
		return EffectType.PARTICLE;
	}
	
	/**
	\brief Check whether the Effect is EffectParticle without casting
		\return \p bool Whether the Effect is EffectParticle
	*/
	override bool IsParticle()
	{
		return true;
	}
	
	//@}
	
	
	
	/** \name Main particle
		Set or get the main particle which this Effect will manage
	*/
	//@{
	
	/**
	\brief Sets the main particle which this Effect will manage
		\param p \p Particle Main particle which this Effect will manage
	*/
	void SetParticle(Particle p)
	{
		// Unregister the events on the old
		if (m_ParticleObj)
		{
			ParticleEvents ope = m_ParticleObj.GetEvents();
			ope.Event_OnParticleStart.Remove(Event_OnEffectStarted);
			ope.Event_OnParticleStop.Remove(Event_OnEffectEnded);
		}
		
		// Assign the new main Particle
		m_ParticleObj = p;
		
		// Register the events on the new
		if (m_ParticleObj)
		{
			ParticleEvents npe = m_ParticleObj.GetEvents();
			npe.Event_OnParticleStart.Insert(Event_OnEffectStarted);
			// We will use Stop instead of End, as old particles were destroyed when they stopped
			// And this system kinda relies on that
			npe.Event_OnParticleStop.Insert(Event_OnEffectEnded);
		}
	}
	
	/**
	\brief Gets the main particle which this Effect is managing
		\return \p Particle Main particle which this Effect is managing
	*/
	Particle GetParticle()
	{
		return m_ParticleObj;
	}
	
	//@}
	
	
	
	/** \name Playback
	*	Methods to Play/Stop Effect
	*	Generally, SEffectManager.Play methods are used instead of Start
	*/
	//@{
	
	/**
	\brief Plays all elements this effect consists of
		\note Is called by SEffectManager.Play methods
	*/
    override void Start()
    {		
		if (m_ParticleID > 0)
		{
			vector pos = GetPosition();			
			vector ori = GetOrientation();
			
			if (m_ParentObject)
			{
				pos = GetLocalPosition();
				ori = GetAttachedLocalOri();
			}		
			
			SetParticle(ParticleManager.GetInstance().CreateParticle(m_ParticleID, pos, true, GetParent(), ori, IsParticleRotationRelativeToWorld()));
		}

		super.Start();
	}
	
	/**
	\brief Stops all elements this effect consists of
		\note Alternatively use SEffectManager.Stop( effect_id )
	*/
    override void Stop()
    {
		if ( GetParticle() )
		{
			GetParticle().Stop();
			SetParticle(null);
		}
		
		super.Stop();
    }
	
	//@}
	
	
	
	/** \name Attach
		Helper methods for attaching to prent
	*/
	//@{
	
	/**
	\brief Read Particle.AddAsChild
	*/
	void AttachTo(Object obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0", bool force_rotation_to_world = false)
	{
		// Update the cached variables...
		SetParent(obj);
		SetLocalPosition(local_pos);
		SetAttachedLocalOri(local_ori);
		ForceParticleRotationRelativeToWorld(force_rotation_to_world);
		
		// Now attach it
		AddAsChild(obj, local_pos, local_ori, force_rotation_to_world);
	}
	
	/**
	\brief Helper method to attach to parent using stored settings
	*/
	void ReAttach()
	{
		// Skip the updating, as we are going to reuse what was set before
		AddAsChild( GetParent(), GetLocalPosition(), GetAttachedLocalOri(), IsParticleRotationRelativeToWorld());
	}
	
	/**
	\brief Helper method to attach to parent
	*/
	protected void AddAsChild(Object obj, vector local_pos, vector local_ori, bool force_rotation_to_world)
	{
		Particle p = GetParticle();		
		if (p)
		{
			p.AddAsChild(obj, local_pos, local_ori, force_rotation_to_world);
		}
	}
	
	//@}
	
	
	
	/** \name Events
		Various events that can be overriden for custom behaviour
	*/
	//@{
	
	/**
	\brief Event which just simply exists (DEPRECATED)
		\warning Never called or used
		\note Use Event_OnStarted instead
	*/
	void Event_OnPlayStart()
	{
		
	}
	
	/**
	\brief Event which just simply exists (DEPRECATED)
		\warning Never called or used
		\note Use Event_OnStarted instead
	*/
	void Event_OnPlayStarted()
	{

	}

	//@}
	
	
	
	/** \name Generic API
		Setters and getters for generic data and properties
	*/
	//@{
	
	/**
	\brief Sets the id of the particle to be used
		\note Only changes the cached variable, for immediate, use SetCurrentParticleID
		\param id \p int Particle ID registered in ParticleList
	*/
	void SetParticleID( int id )
    {
        m_ParticleID = id;
    }
	
	/**
	\brief Gets the id of the particle to be used
		\warning Only gets the cached variable, for immediate effect use GetCurrent variant
		\return \p int Particle ID registered in ParticleList
	*/
	int GetParticleID()
    {
        return m_ParticleID;
    }
	
	/**
	\brief Sets the id of the particle to be used
		\note Particle will not update immediately, but ParticleSource will
		\param id \p int Particle ID registered in ParticleList
	*/
	void SetCurrentParticleID( int id )
    {
        m_ParticleID = id;
		
		Particle p = GetParticle();
		if (p)
		{
			p.SetSource(id);
		}
    }
	
	/**
	\brief Gets the current id of the managed Particle
		\return \p int Particle ID registered in ParticleList
	*/
	int GetCurrentParticleID()
    {
		Particle p = GetParticle();
		if (p)
		{
			return p.GetParticleID();
		}
		else
		{
			return ParticleList.INVALID;
		}
    }
	
	/**
	\brief Set current parent of the managed Particle
		\param parent_obj \p Object The parent for the Particle
		\param updateCached \p bool Whether to update the cached variable
	*/
	override void SetCurrentParent( Object parent_obj, bool updateCached = true )
	{
		super.SetCurrentParent(parent_obj, updateCached);
		
		ReAttach();
	}
	
	/**
	\brief Get the current parent of the managed Particle
		\return \p Object The currrent parent of the Particle
	*/
	override Object GetCurrentParent()
	{
		Particle p = GetParticle();
			
		if (p)
			return Object.Cast(p.GetParent());
		else
			return super.GetParent();
	}
	
	/**
	\brief Set the current world position of the managed Particle
		\param pos \p vector The current world position for the Particle
		\param updateCached \p bool Whether to update the cached variable
	*/
	override void SetCurrentPosition( vector pos, bool updateCached = true )
	{
		super.SetCurrentPosition(pos, updateCached);
		
		Particle p = GetParticle();
		
		if (p)
			p.SetPosition(pos);
	}
	
	/**
	\brief Get the current world position of the managed Particle
		\return \p vector The current world position of the managed Particle
	*/
	override vector GetCurrentPosition()
	{
		Particle p = GetParticle();	
		
		if (p)
			return p.GetPosition();
		else
			return super.GetPosition();
	}
	
	/**
	\brief Set the current local position of the managed Particle
		\param pos \p vector The current local position for the managed Particle
		\param updateCached \p bool Whether to update the cached variable
	*/
	override void SetCurrentLocalPosition( vector pos, bool updateCached = true )
    {
		super.SetCurrentLocalPosition(pos, updateCached);
		
		Particle p = GetParticle();			
		if (p)
		{
			Object parent = GetParent();
			
			if (parent)
				ReAttach();
			else
				p.SetPosition(pos);
		}
    }

	/**
	\brief Get the current local position of the managed Particle
		\return \p vector The current local position of the managed Particle
	*/
	override vector GetCurrentLocalPosition()
	{
		Particle p = GetParticle(); 
		
		if (p)
		{
			Object parent = GetParent();
			
			if (parent)
				return parent.WorldToModel(p.GetPosition());
			else
				return p.GetPosition();
		}
		else
			return super.GetLocalPosition();
	}
	
	/**
	\brief Set orientation of the EffectParticle
		\warning Only sets the cached variable, for immediate effect use SetCurrent variant
		\param ori \p vector Orientation in degrees (yaw, pitch, roll)
	*/
	void SetOrientation( vector ori )
	{
		m_Orientation = ori;
	}
	
	/**
	\brief Get the orientation of the EffectParticle
		\warning Only gets the cached variable, for immediate effect use GetCurrent variant
		\return \p vector The orientation of EffectParticle
	*/
	vector GetOrientation()
	{
		return m_Orientation;
	}
	
	/**
	\brief Set the current orientation of the managed Particle
		\param ori \p vector Orientation in degrees (yaw, pitch, roll)
	*/
	void SetCurrentOrientation( vector ori, bool updateCached = true )
	{
		if ( updateCached)
			SetOrientation(ori);
		
		Particle p = GetParticle();	
				
		if (p)
			p.SetOrientation(ori);
	}
	
	/**
	\brief Get the current orientation of the managed Particle
		\return \p vector The current orientation of the managed Particle
	*/
	vector GetCurrentOrientation()
	{
		Particle p = GetParticle();	
		
		if (p)
			return p.GetOrientation();
		else
			return vector.Zero;
	}
	
	/**
	\brief Set orientation setting to be used by the effect when the Effect starts
		\warning Only caches it into a variable to be used by Start, does not live update when called afterwards
		\note There is no way to update this immediately
		\param state \p bool Whether to keep WS orientation when attaching it to parent
	*/
	void ForceParticleRotationRelativeToWorld(bool state)
	{
		m_ForceRotationRelativeToWorld = state;
	}
	
	/**
	\brief Get the orientation setting to be used by the effect when the Effect starts
		\warning Only gets the cached variable, for immediate effect use IsParticleCurrentRotationRelativeToWorld
		\return \p bool Whether to keep WS orientation when attaching it to parent
	*/
	bool IsParticleRotationRelativeToWorld()
	{
		Particle p = GetParticle();	
		
		if (p)
			return p.IsHierarchyPositionOnly();
		else
			return m_ForceRotationRelativeToWorld;
	}
	
	/**
	\brief Get the current orientation setting to be used by the managed Particle
		\return \p bool Whether the managed Particle is only updating position from parent
	*/
	bool IsParticleCurrentRotationRelativeToWorld()
	{
		Particle p = GetParticle();	
		
		if (p)
			return p.IsHierarchyPositionOnly();
		else
			return false;
	}
	
	//@}

	
	
	/** \name DEPRECATED
		Methods which exist for backwards compatibility and are no longer in use or have never been in use
	*/
	//@{
	
	/**
	\brief Was never called and probably should never be called
		\warning Emptied the functionality as it is relatively unsafe...
	*/
	void CheckLifeSpan()
	{
		/*
		if ( !m_ParticleObj )
		{
			delete this;
		}
		
		OnCheckUpdate();
		*/
	}

	void SetDecalOwner(Object o)
	{
		m_Object = o;
	}
	
	//@}
}