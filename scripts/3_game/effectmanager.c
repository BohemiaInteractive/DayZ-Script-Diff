/**
\brief Manager class for managing Effect (EffectParticle, EffectSound)
	\warning Keeps a ref to any Effect registered (Created/Played), make sure to perform the necessary cleanup
*/
class SEffectManager
{
	//! Static map of all registered effects <id, Effect>
	protected static ref map<int, ref Effect> m_EffectsMap;
	//! Static array of IDs that were previously used, but freed up by unregistering
	protected static ref array<int> m_FreeEffectIDs;
	//! Counter for quickly getting the next ID if FreeEffectIDs array is empty
	protected static int m_HighestFreeEffectID = 1;
	//! As the counter starts at 1, Effect ID can never be 0
	static const int INVALID_ID = 0;
	//! Bool to check whether Cleanup is happening, which means that the maps should no longer be accessed
	protected static bool m_IsCleanup;
	//! Bool to check whether Init was called
	protected static bool m_IsInitialized;
	
	//! Static map of cached sound params, to prevent having to recreate them
	protected static ref map<string, ref SoundParams> m_ParamsMap;
	
	//! Static invoker for the SEffectManager.Event_OnFrameUpdate called form MissionGameplay.OnUpdate
	static ref ScriptInvoker Event_OnFrameUpdate;
	
	
	
	/** \name Generic playback
 		Methods for playing Effect
		\note Since 1.15, these should work on EffectSound as well
	*/
	//@{
	
	/**
	*\brief Play an Effect
	*	\warning As the Effect is automatically registered, it will not be freed automatically (because of the ref)
	*				Unless 'SetAutodestroy(true)' is called on the created 'Effect', which will clean it up when the sound stop
	*				Alternatively, SEffectManager.DestroyEffect can be called manually, which will also unregister and cleanup
	*	\param eff \p Effect The Effect to play
	*	\param pos \p vector The position to play the Effect
	*	\return \p int The registered ID of the Effect
	*/
	static int PlayInWorld(notnull Effect eff, vector pos)
	{
		// Stop the effect first, just in case
		eff.Stop();
		
		int id = EffectRegister(eff);
		
		eff.SetPosition( pos );
		eff.Start();	
		
		return id;
	}
	
	/**
	*\brief Play an Effect
	*	\warning Read PlayInWorld warning
	*	\param eff \p Effect The Effect to play
	*	\param obj \p Object The parent of the Effect
	*	\param local_pos \p vector The local position to play the Effect in relation to the parent (Optional)
	*	\param local_ori \p vector The local orientation to play the Effect in relation to the parent (Optional)
	*	\param force_rotation_relative_to_world \p bool Whether to force the orientation to stay in WS (Optional)
	*	\return \p int The registered ID of the Effect
	*/
	static int PlayOnObject(notnull Effect eff, Object obj, vector local_pos = "0 0 0", vector local_ori = "0 0 0", bool force_rotation_relative_to_world = false)
	{
		// Stop the effect first, just in case
		eff.Stop();
		
		int id = EffectRegister(eff);
		
		if (!obj)
		{
			ErrorEx("Parent object is null.", ErrorExSeverity.WARNING);
			eff.SetPosition(local_pos);
		}
		else
		{
			eff.SetPosition(obj.GetPosition());
		}
			
		eff.SetParent(obj);
		eff.SetLocalPosition(local_pos);
		eff.SetAttachedLocalOri(local_ori);
		
		if (force_rotation_relative_to_world)
		{
			EffectParticle eff_particle = EffectParticle.Cast(eff);
			
			if (eff_particle)
			{
				eff_particle.ForceParticleRotationRelativeToWorld(force_rotation_relative_to_world);
			}
		}
		
		eff.Start();
		
		return id;
	}
	
	/**
	\brief Stops the Effect
		\param effect_id \p int The ID of the Effect to Stop
	*/
	static void Stop(int effect_id)
	{
		Effect eff = m_EffectsMap.Get(effect_id);
		
		if (eff)
		{
			eff.Stop();
		}
		else
		{
			ErrorEx(string.Format("Failed to stop Effect with ID %1. The ID is not registered in m_EffectsMap!", effect_id));
		}
	}
	
	//@}
	

	
	/** \name Create/Play sound
 		Methods for playing/creating sound
	*/
	//@{
	
	/**
	*\brief Create an EffectSound
	*	\warning Read PlayInWorld warning
	*	\param sound_set \p string The sound set name of the sound
	*	\param position \p vector The position to play the sound
	*	\param play_fade_in \p float The fade in duration of the sound (Optional)
	*	\param stop_fade_out \p float The fade out duration of the sound (Optional)
	*	\param loop \p bool Whether the sound should loop (Optional)
	*	\param enviroment \p bool Whether to set environment variables (Optional)
	*	\return \p EffectSound The created EffectSound
	*/
	static EffectSound CreateSound(string sound_set, vector position, float play_fade_in = 0, float stop_fade_out = 0, bool loop = false, bool enviroment = false)
	{
		EffectSound effect_sound = new EffectSound();
		effect_sound.SetSoundSet(sound_set);
		effect_sound.SetPosition(position);
		effect_sound.SetSoundFadeIn(play_fade_in);
		effect_sound.SetSoundFadeOut(stop_fade_out);
		effect_sound.SetSoundLoop(loop);
		effect_sound.SetEnviromentVariables(enviroment);
		
		EffectRegister( effect_sound );
		
		return effect_sound;
	}
	
	/**
	*\brief Create and play an EffectSound
	*	\warning Calls CreateSound, read CreateSound warning
	*	\param sound_set \p string The sound set name of the sound
	*	\param position \p vector The position to play the sound
	*	\param play_fade_in \p float The fade in duration of the sound (Optional)
	*	\param stop_fade_out \p float The fade out duration of the sound (Optional)
	*	\param loop \p bool Whether the sound should loop (Optional)
	*	\return \p EffectSound The created EffectSound
	*/
	static EffectSound PlaySound(string sound_set, vector position, float play_fade_in = 0, float stop_fade_out = 0, bool loop = false)
	{
		EffectSound effect_sound = CreateSound(sound_set, position, play_fade_in, stop_fade_out, loop, false);
				
		effect_sound.SoundPlay();
		
		return effect_sound;
	}
	
	/**
	*\brief Create and play an EffectSound
	*	\warning Calls CreateSound, read CreateSound warning
	*	\param params \p SoundParams Params to create the sound with
	*	\param position \p vector The position to play the sound
	*	\param play_fade_in \p float The fade in duration of the sound (Optional)
	*	\param stop_fade_out \p float The fade out duration of the sound (Optional)
	*	\param loop \p bool Whether the sound should loop (Optional)
	*	\return \p EffectSound The created EffectSound
	*/
	static EffectSound PlaySoundParams(notnull SoundParams params, vector position, float play_fade_in = 0, float stop_fade_out = 0, bool loop = false)
	{
		EffectSound effect_sound = CreateSound(params.GetName(), position, play_fade_in, stop_fade_out, loop, false);

		effect_sound.SoundPlayEx(params);
		
		return effect_sound;
	}
	
	/**
	*\brief Create and play an EffectSound, using or creating cached SoundParams
	*	\warning Calls CreateSound, read CreateSound warning
	*	\param sound_set \p string The sound set name of the sound
	*	\param position \p vector The position to play the sound
	*	\param play_fade_in \p float The fade in duration of the sound (Optional)
	*	\param stop_fade_out \p float The fade out duration of the sound (Optional)
	*	\param loop \p bool Whether the sound should loop (Optional)
	*	\return \p EffectSound The created EffectSound
	*/
	static EffectSound PlaySoundCachedParams(string sound_set, vector position, float play_fade_in = 0, float stop_fade_out = 0, bool loop = false)
	{
		SoundParams params = GetCachedSoundParam(sound_set);
		
		EffectSound effect_sound = CreateSound(params.GetName(), position, play_fade_in, stop_fade_out, loop, false);

		effect_sound.SoundPlayEx(params);
		
		return effect_sound;
	}
	
	/**
	*\brief Create and play an EffectSound, updating environment variables
	*	\warning Calls CreateSound, read CreateSound warning
	*	\param sound_set \p string The sound set name of the sound
	*	\param position \p vector The position to play the sound
	*	\param play_fade_in \p float The fade in duration of the sound (Optional)
	*	\param stop_fade_out \p float The fade out duration of the sound (Optional)
	*	\param loop \p bool Whether the sound should loop (Optional)
	*	\return \p EffectSound The created EffectSound
	*/
	static EffectSound PlaySoundEnviroment(string sound_set, vector position, float play_fade_in = 0, float stop_fade_out = 0, bool loop = false)
	{
		EffectSound effect_sound = CreateSound(sound_set, position, play_fade_in, stop_fade_out, loop, true);
				
		effect_sound.SoundPlay();
		
		return effect_sound;
	}
	
	/**
	*\brief Create and play an EffectSound
	*	\warning Calls CreateSound, read CreateSound warning
	*	\param sound_set \p string The sound set name of the sound
	*	\param parent_object \p Object The parent Object for the sound to follow
	*	\param play_fade_in \p float The fade in duration of the sound (Optional)
	*	\param stop_fade_out \p float The fade out duration of the sound (Optional)
	*	\param loop \p bool Whether the sound should loop (Optional)
	*	\return \p EffectSound The created EffectSound
	*/
	static EffectSound PlaySoundOnObject(string sound_set, Object parent_object, float play_fade_in = 0, float stop_fade_out = 0, bool loop = false)
	{
		EffectSound effect_sound = CreateSound(sound_set, parent_object.GetPosition(), play_fade_in, stop_fade_out, loop);
		
		effect_sound.SetParent( parent_object );
		effect_sound.SetLocalPosition( vector.Zero );
		effect_sound.SoundPlay();
		
		return effect_sound;
	}
	
	//@}
	
	
	
	/** \name Generic API
		General methods used for SEffectManager
	*/
	//@{
	
	/**
	\brief Unregisters, stops and frees the Effect
		\param effect_sound \p EffectSound The EffectSound to free
	*/
	static void DestroyEffect(Effect effect)
	{
		if (effect)
		{
			// Functionality already happens in dtor of Effect to be safe
			delete effect;
		}
	}
	
	/**
	\brief Checks whether an Effect ID is registered in SEffectManager
		\param effect_id \p int The Effect ID to check
		\return \p bool Whether there is an Effect registered for this ID
	*/
	static bool IsEffectExist( int effect_id )
	{
		if (!m_IsCleanup)
			return m_EffectsMap[effect_id] != null;
		else
			return false;
	}
			
	/**
	\brief Gets the Effect with the given registered Effect ID
		\param effect_id \p int The Effect ID
		\return \p Effect The Effect registered to the ID or null
	*/
	static Effect GetEffectByID(int effect_id)
	{
		if (!m_IsCleanup)
			return m_EffectsMap[effect_id];
		else
			return null;
	}
	
	/**
	\brief Registers Effect in SEffectManager
		\note Already handled in SEffectManager Create/Play methods
		\note This will make SEffectManager hold a strong ref for the Effect
		\param effect \p Effect The Effect to register
		\return \p int The Effect ID
	*/
	static int EffectRegister(Effect effect)
	{
		if (effect.IsRegistered())
		{
			ErrorEx(string.Format("Attempted to register Effect '%1' which was already registered.", effect.GetDebugName()), ErrorExSeverity.INFO);			
			return effect.GetID();
		}
		
		int id;
		
		if (!m_IsCleanup)
		{
			id = GetFreeEffectID();
			m_EffectsMap.Insert(id, effect);
			effect.Event_OnRegistered(id);
		}
		else
			ErrorEx("Attempted to register Effect while SEffectManager is cleaning up, request ignored.", ErrorExSeverity.WARNING);
		
		return id;
	}
	
	/**
	\brief Unregisters Effect in SEffectManager
		\note Will automatically occur on stop when the Effect is AutoDestroy
		\note ID can be gotten from the Effect by calling Effect.GetID
		\note Generic Play methods will also return the ID
		\param id \p int The ID of the Effect to unregister
	*/
	static void EffectUnregister(int id)
    {
		if (m_IsCleanup)
			return; // No error needed, since it will have been unregistered anyways after cleanup is finished
		
		Effect effect;
		if ( m_EffectsMap.Find(id, effect) )
		{
			effect.Event_OnUnregistered();
			m_EffectsMap.Remove(id);
		}		
		
		if ( m_FreeEffectIDs.Find(id) == -1 )
		{
			m_FreeEffectIDs.Insert(id);
		}
    }
	
	/**
	\brief Unregisters Effect in SEffectManager
		\param effect \p Effect The Effect to unregister
	*/
	static void EffectUnregisterEx(Effect effect)
    {
		EffectUnregister(effect.GetID());
    }
	
	/**
	\brief Helper function for EffectRegister to decide an Effect ID
		\return \p int A currently unused Effect ID
	*/
	protected static int GetFreeEffectID()
	{
		int return_id;
		
		if (m_FreeEffectIDs.Count() > 0)
		{
			return_id = m_FreeEffectIDs.Get(0);
			m_FreeEffectIDs.Remove(0);
		}
		else
		{
			return_id = m_HighestFreeEffectID;
			++m_HighestFreeEffectID;
		}
		
		return return_id;		
	}
	
	//@}
	
	
	
	/** \name Sound helpers
 		Sound specific helper methods
	*/
	//@{
	
	/**
	\brief Legacy, backwards compatibility
		\param sound_effect \p EffectSound The EffectSound to free
		\return \p bool A bool which is always true
	*/
	static bool DestroySound(EffectSound sound_effect)
	{
		DestroyEffect(sound_effect);		
		return true;
	}
	
	/**
	\brief Get or create a cached SoundParams object
		\param soundset \p string The sound set name of the sound
		\return \p SoundParams The cached SoundParams for the given soundset
	*/
	static SoundParams GetCachedSoundParam(string soundset)
	{
		SoundParams params;
		if (!m_ParamsMap.Find(soundset, params))
		{
			params = new SoundParams(soundset);
			m_ParamsMap.Insert(soundset, params);
		}
		return params;
	}
	
	//@}
	
	
	
	/** \name Events
		Various events that can be overriden for custom behaviour
	*/
	//@{
	
	/**
	\brief Event called from EffectSound.Event_OnSoundWaveEnded
		\note Every registered sound is registered to call this
		\param effect_sound \p EffectSound The EffectSound calling the event
	*/
	static void Event_OnSoundWaveEnded(EffectSound effect_sound)
	{
		
	}
	
	/**
	\brief Event called on frame
		\note Called from MissionGameplay.OnUpdate
		\note Effects register themselves by Effect.SetEnableEventFrame(true)
		\note EffectSound is automatically registered
		\param time_delta \p float Time passed since the previous frame
	*/
	static void Event_OnFrameUpdate(float time_delta)
	{
		Event_OnFrameUpdate.Invoke(time_delta);
	}
	
	//@}
	
	
	
	/** \name Lifetime
 		Creation and cleanup
	*/
	//@{
	
	/**
	\brief Initialize the containers
		\note This is done this way, to have these not exist on server
	*/
	static void Init()
	{
		m_EffectsMap = new map<int, ref Effect>;
		m_FreeEffectIDs = new array<int>;
		m_ParamsMap = new map<string, ref SoundParams>;
		Event_OnFrameUpdate = new ScriptInvoker();
		
		m_IsInitialized = true;
	}
	
	/**
	\brief Cleanup method to properly clean up the static data
		\note Will be called when MissionBase is destroyed
	*/
	static void Cleanup()
	{
		// Nothing to clean
		if (!m_IsInitialized)
			return;		
		
		m_IsCleanup = true;
		
		// There should not be anything in here on server
		if (GetGame() && GetGame().IsDedicatedServer())
		{
			if (m_ParamsMap.Count() > 0)
				ErrorEx(string.Format("SEffectManager containing SoundParams on server."), ErrorExSeverity.WARNING);
			
			if (m_EffectsMap.Count() > 0)
				ErrorEx(string.Format("SEffectManager containing Effect on server."), ErrorExSeverity.WARNING);
		}
		
		// These are intentionally cached, just clear them
		m_ParamsMap.Clear();
		
		// These might not be intentionally still here, so log how many there are
		#ifdef DEVELOPER
		Print("--- SEffectManager Cleanup dump - Begin ------------------------");
		Print(string.Format("Effect count: %1", m_EffectsMap.Count()));
		#endif
		
		// Best to call the unregister event before clearing the map
		// In case some ref is still being held elsewhere and will still be kept alive
		foreach (int id, Effect eff : m_EffectsMap)
		{
			eff.Event_OnUnregistered();
			#ifdef SFXM_DUMP
			Print(string.Format( "%1 :: %2 :: %3", eff, typename.EnumToString(EffectType, eff.GetEffectType()), eff.GetDebugName() ));
			#endif
		}
		
		#ifdef DEVELOPER
		Print("--- SEffectManager Cleanup dump - End --------------------------");
		#endif
		
		// Now we can clear it
		m_EffectsMap.Clear();
		
		// Reset the state
		m_HighestFreeEffectID = 1;		
		Event_OnFrameUpdate.Clear();
		m_IsCleanup = false;
	}
	
	//@}
}