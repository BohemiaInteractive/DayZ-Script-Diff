/*
Please remember that:
-Lights work only on client side!
-Lights with Brightness or Radius of 0 (or less) are automatically deleted
-Lights are very performance heavy. Especially if they cast shadows. Use them carefully!

Script author: Boris Vacula
*/

class ScriptedLightBase extends EntityLightSource
{
	float 		m_LifetimeStart;
	float 		m_LifetimeEnd = -1; // -1 makes this light permanent
	float 		m_FadeOutTime = -1;
	float 		m_FadeInTime = -1;
	float 		m_Radius;
	float 		m_RadiusTarget;
	float 		m_Brightness;
	float 		m_BrightnessPulse; // flicker effect
	float 		m_BrightnessPulseSpeed;
	float 		m_BrightnessPulseAmplitudeMax;
	float 		m_BrightnessPulseAmplitudeMin;
	float 		m_BrightnessTarget;
	float 		m_BrightnessSpeedOfChange = 1;
	float 		m_RadiusSpeedOfChange = 1;
	float 		m_OptimizeShadowsRadius = 0; // Within this range between the light source and camera the shadows will be automatically disabled to save on performance
	float 		m_DancingShadowsAmplitude;
	float 		m_DancingShadowsSpeed;
	bool 		m_IsDebugEnabled = false;
	
	Object 		m_Parent; // Attachment parent
	vector 		m_LocalPos; // Local position to my attachment parent
	vector 		m_LocalOri; // Local orientation to my attachment parent
	vector 		m_DancingShadowsLocalPos;
	
	ref Timer 	m_DeleteTimer;
	
	static ref set<ScriptedLightBase> m_NightTimeOnlyLights = new set<ScriptedLightBase>();
	
	//! Constructor. Everything here is executed before the constructor of all children.
	void ScriptedLightBase()
	{
		m_LifetimeStart = GetGame().GetTime();
		SetEnabled(true);
		SetEventMask(EntityEvent.FRAME);
		SetEventMask(EntityEvent.INIT);
	}
	
	void ~ScriptedLightBase()
	{
		if (m_NightTimeOnlyLights)
		{
			int index = m_NightTimeOnlyLights.Find(this);
			if (index != -1)
			{
				m_NightTimeOnlyLights.Remove(index);
			}
		}
	}
	
	override void EOnInit(IEntity other, int extra)
	{
		if (!IsVisibleDuringDaylight())
		{
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if (player && player.m_UndergroundPresence)
				SetVisibleDuringDaylight(true);
			m_NightTimeOnlyLights.Insert(this);
		}
	}
	
	override bool IsScriptedLight()
	{
		return true;
	}
	
	void UpdateMode()
	{
		ItemBase item = ItemBase.Cast(m_Parent);
		if (item)
		{
			InventoryLocation il = new InventoryLocation;
			item.GetInventory().GetCurrentInventoryLocation( il );
			string slotName;
			if (il.GetType() == InventoryLocationType.GROUND)
			{
				slotName = "Ground";
			}
			else if (il.GetSlot() != -1)
			{
				slotName = InventorySlots.GetSlotName(il.GetSlot());
			}
			UpdateLightMode(slotName);
			
		}
	}
	
	private void UpdateLightMode(string slotName);
	
	//! Correct way of deleting light from memory. It is necesarry to have this delay due to hierarchy.
	private void DeleteLightWithDelay()
	{
		DetachFromParent(); // This is the reason for the delay
		
		if (!m_DeleteTimer)
			m_DeleteTimer = new Timer( CALL_CATEGORY_SYSTEM );
			
		m_DeleteTimer.Run( 0.03 , this, "DeleteLightNow", NULL, true);
	}
	
	// Deletes light now. Do not call this directly. Call Destroy() instead. Otherwise you might get errors related to hierarchy.
	private void DeleteLightNow()
	{
		GetGame().ObjectDelete(this);
	}
	
	//! Attaches this light on the parent entity, with optional position and orientation offset.
	void AttachOnObject(Object parent, vector local_pos = "0 0 0", vector local_ori = "0 0 0")
	{
		if (!parent)
		{
			if (m_Parent)
			{
				m_Parent.RemoveChild(this);
			}
			
			return;
		}
		else
		{
			if (m_Parent)
			{
				m_Parent.RemoveChild(this);
			}
		}
		
		m_Parent = parent;
		m_LocalPos = local_pos;
		m_LocalOri = local_ori;
		SetOrientation(local_ori);
		SetPosition(local_pos);
		parent.AddChild(this, -1);
		parent.Update();
	}
	
	//! Returns attachment parent
	Object GetAttachmentParent()
	{
		return m_Parent;
	}
	
	//! Attaches this light on the parent entity's memory point, with optional direction target memory point.
	void AttachOnMemoryPoint(Object parent, string memory_point_start, string memory_point_target = "")
	{
		if (parent.MemoryPointExists(memory_point_start))
		{
			m_LocalPos = parent.GetMemoryPointPos(memory_point_start);
			vector local_ori;
			
			if (memory_point_target != "" )
			{
				if (parent.MemoryPointExists(memory_point_target))
				{
					vector target_pos = parent.GetSelectionPositionLS(memory_point_target);
					target_pos = vector.Direction(m_LocalPos, target_pos);
					local_ori = target_pos.VectorToAngles();
				}
				else
				{
					ErrorEx("memory point 'memory_point_target' not found when attaching light");
				}
			}
			AttachOnObject(parent, m_LocalPos, local_ori);	
			UpdateMode();
		}
		else
		{
			ErrorEx("memory point 'memory_point_start' not found when attaching light");
		}
		
	}
	
	//! Detaches this light from its parent entity.
	void DetachFromParent()
	{
		if (!m_Parent)
		{
			m_Parent = Object.Cast( GetParent() );
		}
		
		if (m_Parent)
		{
			if ( !m_Parent.ToDelete()  &&  !ToDelete() )
			{
				m_Parent.RemoveChild(this);
			}
		}
		
		m_Parent = null;
		m_LocalPos = Vector(0,0,0);
		m_LocalOri = Vector(0,0,0);
	}
	
	static ScriptedLightBase CreateLightAtObjMemoryPoint(typename name, notnull Object target, string memory_point_start, string memory_point_target = "", vector global_pos = "0 0 0", float fade_in_time_in_s = 0)
	{
		ScriptedLightBase light;
		if (target.MemoryPointExists(memory_point_start))
		{
			light = CreateLight(name, global_pos, fade_in_time_in_s);
			light.AttachOnMemoryPoint(target, memory_point_start, memory_point_target);
		}
		return light;
	}
	
	//! Creates an instance of light on the given position. Optionally, use fade_in_time_in_s parameter to make the light fade into existence.
	static ScriptedLightBase CreateLight(typename name, vector global_pos = "0 0 0", float fade_in_time_in_s = 0)
	{
		ScriptedLightBase light_instance;
		
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			light_instance = ScriptedLightBase.Cast( GetGame().CreateObject(name.ToString(), global_pos, true) );
			
			if (!light_instance)
			{
				Error("Error! Light entity of name " + name.ToString() + " cannot be spawned! This name is incorrect or not inherited from ScriptedLightBase." );
				return null;
			}
			
			if (fade_in_time_in_s != 0)
			{
				light_instance.FadeIn(fade_in_time_in_s);
			}
		}
		else // Server side
		{
			if ( GetGame().IsDebug() )
			{
				Error("An instance of ScriptedLightBase was attempted to spawn on server side! Lights are CLIENT SIDE ONLY!");
			}
		}
		
		return light_instance;
	}
	
	//! Sets the brightness of the light
	void SetBrightnessTo(float value)
	{
		m_Brightness = value;
		m_BrightnessTarget = value;
		SetBrightness(m_Brightness * m_BrightnessPulse);
		CorrectLightPulseDuringDaylight();
	}
	
	//! Call this after using SetBrightness(...) to fix light's intensity during daytime
	void CorrectLightPulseDuringDaylight()
	{
		if (m_Brightness < 100)
		{
			float v = m_Brightness * 0.01;
			
			if (v > 0)
			{
				float brightness_compesation = 1 / v;
				float compenset_brightness = (m_Brightness * m_BrightnessPulse) * brightness_compesation;
				SetBrightness(compenset_brightness);
				SetPulseCoef(v);
			}
		}
		else
		{
			SetPulseCoef(1);
		}
	}
	
	//! Fades the brightness of the light to the given value.
	void FadeBrightnessTo( float value, float time_in_s )
	{
		m_BrightnessTarget = value;
		
		if (time_in_s == 0)
		{
			m_BrightnessSpeedOfChange = 9999;
		}
		else
		{
			m_BrightnessSpeedOfChange = Math.AbsFloat(m_Brightness - m_BrightnessTarget) / time_in_s;
		}
	}
	
	//! Sets the radius of the light
	void SetRadiusTo(float value)
	{
		m_Radius = value;
		m_RadiusTarget = value;
		SetRadius(m_Radius);
	}
	
	//! Fades the radius of the light to the given value.
	void FadeRadiusTo( float value, float time_in_s )
	{
		m_RadiusTarget = value;
		
		if (time_in_s == 0)
		{
			m_RadiusSpeedOfChange = 9999;
		}
		else
		{
			m_RadiusSpeedOfChange = Math.AbsFloat(m_Radius - m_RadiusTarget) / time_in_s;
		}
	}
	
	//! Switches off the light and deletes it from memory
	void Destroy()
	{
		ClearEventMask(EntityEvent.FRAME);
		SetEnabled(false);
		if (m_Parent)
			DeleteLightWithDelay();
		else
			DeleteLightNow();
	}
	
	//! Makes the light destroy itself after the given time in seconds. The light will fade out if it's set to do so with SetFadeOutTime(...)
	void SetLifetime(float life_in_s)
	{
		if(GetGame())
			m_LifetimeEnd = GetGame().GetTime() + life_in_s * 1000;
	}
	
	//! Sets the fade out time in seconds. Fade out begins automatically as the light nears the end of its life time, or when method FadeOut() is called.
	void SetFadeOutTime(float time_in_s)
	{
		m_FadeOutTime = time_in_s * 1000;
	}
	
	//! Starts the fade out process and destroys the light when its done. Optional parameter allows you to set time of this fade out in seconds. If not set, then default value (from SetFadeOutTime(...)) is used.
	void FadeOut(float time_in_s = -1)
	{
		float time_in_ms = time_in_s * 1000;
		
		if (time_in_s == -1)
		{
			float kill_time_in_s = m_FadeOutTime*0.001;
			
			FadeBrightnessTo(0, kill_time_in_s);
			FadeRadiusTo(0, kill_time_in_s);
			SetLifetime(kill_time_in_s);
		}
		else
		{
			FadeBrightnessTo(0, time_in_s);
			FadeRadiusTo(0, time_in_s);
			SetLifetime(time_in_s);
		}
	}
	
	//! Makes the light fade into existence. Works only at the moment the light is created. Consider using FadeBrightnessTo(...) and FadeRadiusTo(...) at anytime later during lifetime.
	void FadeIn(float time_in_s)
	{
		float brightness = m_Brightness;
		SetBrightnessTo(0);
		FadeBrightnessTo(brightness, time_in_s);
	}
	
	//! Prolongs the lifetime of the light in seconds. Use negative number to shorten its lifetime.
	void AddLifetime(float life_in_s)
	{
		m_LifetimeEnd += life_in_s * 1000;
	}
	
	//! Override this for custom functionality
	void OnFrameLightSource(IEntity other, float timeSlice)
	{
		// ... 
	}
	
	//! On frame event. If you want to control your light within your own rules then override the event OnFrameLightSource and put your code there.
	override void EOnFrame(IEntity other, float timeSlice)
	{
		// Control lifetime of the light
		int current_time = GetGame().GetTime();
		
		if ( CheckLifetime(current_time) )
		{
			SetRadius(m_Radius);
		}
		else
		{
			return;
		}
		
		HandleFlickering(current_time - m_LifetimeStart, timeSlice);
		HandleDancingShadows(current_time - m_LifetimeStart, timeSlice);
		CheckFadeOut(current_time);
		HandleBrightnessFadeing(timeSlice);
		HandleRadiusFadeing(timeSlice);
		
		CheckIfParentIsInCargo();
		TryShadowOptimization();
		OnFrameLightSource(other, timeSlice);
	}
	
	//! Sets the maximum range of the point light within the dancing shadows effect
	void SetDancingShadowsAmplitude(float max_deviation_in_meters)
	{
		m_DancingShadowsAmplitude = Math.AbsFloat(max_deviation_in_meters);
	}
	
	//! Sets the maximum speed of the point light within the dancing shadows effect
	void SetDancingShadowsMovementSpeed(float speed_in_meters_per_frame)
	{
		m_DancingShadowsSpeed = Math.AbsFloat(speed_in_meters_per_frame);
	}
	
	//! Returns max movement range of pointlight within the dancing shadow effect
	float GetDancingShadowsAmplitude()
	{
		return m_DancingShadowsAmplitude;
	}
	
	//! Returns max movement speed of pointlight within the dancing shadow effect
	float GetDancingShadowsMovementSpeed()
	{
		return m_DancingShadowsSpeed;
	}
	
	//! Enables some debug functionality of this light
	void EnableDebug(bool state)
	{
		m_IsDebugEnabled = state;
	}
	
	// Handles subtle movement of the light point to create the effect of dancing shadows
	void HandleDancingShadows(float time, float timeSlice)
	{
		if (m_DancingShadowsAmplitude > 0)
		{
			for (int i = 0; i < 3; i++ )
			{
				m_DancingShadowsLocalPos[i] = m_DancingShadowsLocalPos[i] +  ( Math.RandomFloat(-m_DancingShadowsSpeed,m_DancingShadowsSpeed) * timeSlice) ;
				
				if (m_DancingShadowsLocalPos[i] > m_DancingShadowsAmplitude)
					m_DancingShadowsLocalPos[i] = m_DancingShadowsAmplitude;
				
				if (m_DancingShadowsLocalPos[i] < -m_DancingShadowsAmplitude)
					m_DancingShadowsLocalPos[i] = -m_DancingShadowsAmplitude;
				
			}
			
			if (m_Parent  &&  !m_Parent.ToDelete())
			{
				// In order to move with the light, it must be detached from its parent first
				
				m_Parent.RemoveChild(this);
				SetPosition(m_LocalPos + m_DancingShadowsLocalPos);
				
				m_Parent.AddChild(this, -1);
				m_Parent.Update();
			}
			
			if (m_IsDebugEnabled)
			{
				Particle p = ParticleManager.GetInstance().PlayInWorld( ParticleList.DEBUG_DOT, GetPosition() );
				p.SetParticleParam( EmitorParam.SIZE, 0.01);
			}
		}
		else
		{
			m_DancingShadowsLocalPos = Vector(0,0,0);
		}
	}
	
	// Updates flickering light
	void HandleFlickering(float time, float timeSlice)
	{
		if (m_BrightnessPulseAmplitudeMax > 0)
		{
			m_BrightnessPulse += ( Math.RandomFloat(-m_BrightnessPulseSpeed, m_BrightnessPulseSpeed) ) * timeSlice;
			
			if (m_BrightnessPulse < m_BrightnessPulseAmplitudeMin + 1)
				m_BrightnessPulse = m_BrightnessPulseAmplitudeMin + 1;
			
			if (m_BrightnessPulse > m_BrightnessPulseAmplitudeMax + 1)
				m_BrightnessPulse = m_BrightnessPulseAmplitudeMax + 1;
			
		}
		else
		{
			m_BrightnessPulse = 1;
		}
	}
	
	//! Sets speed of light flickering (random brightness coefficient change per second)
	void SetFlickerSpeed(float speed)
	{
		m_BrightnessPulseSpeed = speed;
	}
	
	//! Sets the change coefficient of flickering light. (0.0 - 1.0 values, result of greater values are period time of light off )
	void SetFlickerAmplitude(float coef)
	{
		m_BrightnessPulseAmplitudeMax = Math.AbsFloat(coef);
		m_BrightnessPulseAmplitudeMin = -Math.AbsFloat(coef);
	}
	
	void SetFlickerAmplitudeMax(float coef)
	{
		m_BrightnessPulseAmplitudeMax = coef;
	}
	
	void SetFlickerAmplitudeMin(float coef)
	{
		m_BrightnessPulseAmplitudeMin = coef;
	}
	
	//! Returns flicker speed
	float GetFlickerSpeed()
	{
		return m_BrightnessPulseSpeed;
	}
	
	//! Returns flicker amplitude maximum
	float GetFlickerAmplitudeCoefMax()
	{
		return m_BrightnessPulseAmplitudeMax;
	}
	
	//! Returns flicker amplitude minimum
	float GetFlickerAmplitudeCoefMin()
	{
		return m_BrightnessPulseAmplitudeMin;
	}
	
	//! Optimizes shadows by disabling them on this light source while it's within the given radius around the camera.
	void TryShadowOptimization()
	{
		if (m_OptimizeShadowsRadius > 0)
		{
			float distance_to_camera = vector.Distance( GetPosition(), GetGame().GetCurrentCameraPosition() );
			
			if (distance_to_camera < m_OptimizeShadowsRadius)
			{
				SetCastShadow(false);
			}
			else
			{
				SetCastShadow(true);
			}
		}
	}
	
	//! When the light source gets within this radius (radius_in_m) around the camera, then it's shadows are disabled.
	void SetDisableShadowsWithinRadius(float radius_in_m)
	{
		m_OptimizeShadowsRadius = radius_in_m;
	}
	
	//! Returns the range you put inside SetDisableShadowsWithinRadius(...)
	float GetDisableShadowsWithinRadius()
	{
		return m_OptimizeShadowsRadius;
	}
	
	void CheckIfParentIsInCargo()
	{
		// TO DO: OPTIMIZE AND REFACTOR! THIS MUST BE HANDLED IN AN EVENT, NOT PER FRAME!
		
		if (m_Parent)
		{
			EntityAI parent_EAI = EntityAI.Cast( m_Parent );
			
			if (parent_EAI) // Check if the Cast was successfull
			{
				GameInventory GI = parent_EAI.GetInventory();
				
				if (GI) // Prevents handling of light on the parent item when it's projected in inventory as the item in inventory character's hands.
				{
					bool is_in_cargo = GI.IsInCargo();
					
					if (!is_in_cargo)
					{
						EntityAI parent2 = parent_EAI.GetHierarchyParent();
						
						if (parent2  &&  parent2.GetInventory())
						{
							is_in_cargo = parent2.GetInventory().IsInCargo();
						}
					}
					
					if ( is_in_cargo )
					{
						SetEnabled(false);
					}
					else
					{
						SetEnabled(true);
					}
				}
			}
		}
	}
	
	// Destroys this light if it's past it lifetime
	private bool CheckLifetime(int current_time)
	{
		if ( current_time > m_LifetimeEnd  &&  m_LifetimeEnd != -1 )
		{
			Destroy();
			return false;
		}
		
		return true;
	}
	
	// Handles fade out effect at the end of lifetime
	private void CheckFadeOut( int current_time)
	{
		// Control fade out of the light
		if ( m_FadeOutTime != -1  &&  m_LifetimeEnd != -1  &&  current_time > m_LifetimeEnd - m_FadeOutTime )
		{
			m_FadeOutTime = -1;
			m_FadeInTime = -1; // Stop fade in process
			float time_left_in_s = (m_LifetimeEnd - current_time) * 0.001;
			FadeBrightnessTo(0, time_left_in_s);
			FadeRadiusTo(0, time_left_in_s);
		}
	}
	
	// handles fading of brightness
	private void HandleBrightnessFadeing(float timeSlice)
	{
		if ( m_Brightness != m_BrightnessTarget )
		{
			float brightness_difference = m_Brightness - m_BrightnessTarget;
			
			if (brightness_difference > m_BrightnessSpeedOfChange*timeSlice)
				brightness_difference = m_BrightnessSpeedOfChange*timeSlice;
			
			if (brightness_difference < -m_BrightnessSpeedOfChange*timeSlice)
				brightness_difference = -m_BrightnessSpeedOfChange*timeSlice;
			
			m_Brightness -= brightness_difference;
			
			if ( m_Brightness > 0  ||  m_BrightnessTarget > 0)
			{
				SetBrightness(m_Brightness * m_BrightnessPulse);
				CorrectLightPulseDuringDaylight();
			}
			else
			{
				Destroy();
				return;
			}
		}
		else
		{
			SetBrightness(m_Brightness * m_BrightnessPulse);
			CorrectLightPulseDuringDaylight();
		}
	}
	
	// handles fading of radius
	private void HandleRadiusFadeing(float timeSlice)
	{
		if ( m_Radius != m_RadiusTarget )
		{
			float radius_difference = m_Radius - m_RadiusTarget;
			
			if (radius_difference > m_RadiusSpeedOfChange*timeSlice)
				radius_difference = m_RadiusSpeedOfChange*timeSlice;
			
			if (radius_difference < -m_RadiusSpeedOfChange*timeSlice)
				radius_difference = -m_RadiusSpeedOfChange*timeSlice;
			
			m_Radius -= radius_difference;
			
			if ( m_Radius > 0  ||  m_RadiusTarget > 0)
			{
				SetRadius(m_Radius);
			}
			else
			{
				Destroy();
				return;
			}
		}
		else
		{
			SetRadius(m_Radius);
		}
	}
};