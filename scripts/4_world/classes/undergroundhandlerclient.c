enum EUndergroundPresence
{
	NONE,//player is not interacting with underground at any level
	OUTER,//player is on the outskirts of the underdound, some effects are already in effect, while others might not be
	TRANSITIONING,//player has entered underground and is in the process of screen darkening transition
	FULL//the player is now fully entered underground
}

class UndergroundHandlerClient
{
	const float LIGHT_BLEND_SPEED_IN = 5;
	const float LIGHT_BLEND_SPEED_OUT = 1.75;
	const float MAX_RATIO = 0.9;//how much max ratio between 0..1 can a single breadcrumb occupy 
	const float RATIO_CUTOFF = 0;//what's the minimum ratio a breadcrumb needs to have to be considered when calculatiing accommodation
	const float DISTANCE_CUTOFF = 5;//we ignore breadcrumbs further than this distance
	const float ACCO_MODIFIER = 1;//when we calculate eye accommodation between 0..1 based on the breadcrumbs values and distances, we multiply the result by this modifier to get the final eye accommodation value
	const float DEFAULT_INTERPOLATION_SPEED = 7;
	const string UNDERGROUND_LIGHTING = "dz\\data\\lighting\\lighting_underground.txt";
	private ref AnimationTimer 	m_AnimTimerLightBlend;
	
	private PlayerBase 				m_Player;
	private PPERUndergroundAcco 	m_Requester;
	private PPERequester_CameraNV 	m_NVRequester;
	private ref set<UndergroundTrigger> m_InsideTriggers = new set<UndergroundTrigger>();
	
	private float 		m_EyeAccoTarget = 1;
	private float 		m_AccoInterpolationSpeed;
	private float 		m_EyeAcco = 1;
	private float 		m_LightingLerpTarget;
	private float 		m_LightingLerp;
	private EffectSound m_AmbientSound;
	
	private UndergroundTrigger m_TransitionalTrigger;
	
	void UndergroundHandlerClient(PlayerBase player)
	{
		GetGame().GetWorld().LoadUserLightingCfg(UNDERGROUND_LIGHTING, "Underground");
		m_Player = player;
		m_NVRequester = PPERequester_CameraNV.Cast(PPERequesterBank.GetRequester( PPERequesterBank.REQ_CAMERANV));
	}
	
	void ~UndergroundHandlerClient()
	{
		if (GetGame())
		{
			GetGame().GetWorld().SetExplicitVolumeFactor_EnvSounds2D(1, 0.5);
			GetGame().GetWeather().SuppressLightningSimulation(false);
			GetGame().GetWorld().SetUserLightingLerp(0);
			if (m_AmbientSound)
				m_AmbientSound.Stop();
		}
	}
	
	private PPERUndergroundAcco GetRequester()
	{
		if (!m_Requester)
		{
			m_Requester = PPERUndergroundAcco.Cast(PPERequesterBank.GetRequester( PPERequesterBank.REQ_UNDERGROUND));
			m_Requester.Start();
		}
		return m_Requester;
	}
	
	void OnTriggerEnter(UndergroundTrigger trigger)
	{
		m_InsideTriggers.Insert(trigger);
		OnTriggerInsiderUpdate();
		
	}
	
	void OnTriggerLeave(UndergroundTrigger trigger)
	{
	 	int index = m_InsideTriggers.Find(trigger);
		if (index != -1)
		{
			m_InsideTriggers.Remove(index);
		}
		OnTriggerInsiderUpdate();
	}
	
	private void CalculateEyeAccoTarget()
	{
		if (m_TransitionalTrigger && m_TransitionalTrigger.m_Data.Breadcrumbs.Count() >= 2)
		{
			float closestDist = float.MAX;
			float acco;
			array<float> distances = new array<float>();
			array<float> distancesInverted = new array<float>();

			
			int excludeMask = 0;
			foreach (int indx, auto crumb:m_TransitionalTrigger.m_Data.Breadcrumbs)
			{
				if (indx > 32)//error handling for exceeding this limit is handled elsewhere
					break;
				
				float dist = vector.Distance(m_Player.GetPosition(), crumb.GetPosition());
				float crumbRadius = m_TransitionalTrigger.m_Data.Breadcrumbs[indx].Radius;
				float maxRadiusAllowed = DISTANCE_CUTOFF;
				
				if (crumbRadius != -1)
				{
					maxRadiusAllowed = crumbRadius;
				}
				if (dist > maxRadiusAllowed)
				{
					excludeMask = (excludeMask | (1 << indx));
				}
				else if (m_TransitionalTrigger.m_Data.Breadcrumbs[indx].UseRaycast)
				{
					int idx = m_Player.GetBoneIndexByName("Head");
					vector rayStart = m_Player.GetBonePositionWS(idx);
					vector rayEnd = crumb.GetPosition();
					vector hitPos, hitNormal;
					float hitFraction;
					Object hitObj;

					if (DayZPhysics.RayCastBullet(rayStart, rayEnd,PhxInteractionLayers.TERRAIN | PhxInteractionLayers.ROADWAY| PhxInteractionLayers.BUILDING, null, hitObj, hitPos, hitNormal, hitFraction))
					{
						excludeMask = (excludeMask | (1 << indx));
					}
				}
				
				distances.Insert(dist);
				
					#ifdef DEVELOPER
				if ( DiagMenu.GetBool(DiagMenuIDs.DM_UNDERGROUND_SHOW_BREADCRUMB) )
				{
					Debug.DrawSphere(crumb.GetPosition(),0.1, COLOR_RED, ShapeFlags.ONCE);
				}
				#endif
			}
			float baseDst = distances[0];
			float sum = 0;
			//Print(excludeMask);
			foreach (float dst:distances)
			{
				if (dst == 0) 
					dst = 0.1;
				float dstInv = (baseDst / dst) * baseDst;
				sum += dstInv;
				distancesInverted.Insert(dstInv);
			}
			float sumCheck = 0;
			float eyeAcco = 0;
			foreach (int i, float dstInvert:distancesInverted)
			{
				/*
				//Print(m_TransitionalTrigger.m_Data.Breadcrumbs[i].EyeAccommodation);
				//Print(m_TransitionalTrigger.m_Data.Breadcrumbs.Count());
				*/
				if ((1 << i) & excludeMask)
					continue;
				float ratio = dstInvert / sum;
				if (ratio > MAX_RATIO)
					ratio = MAX_RATIO;
				if (ratio > RATIO_CUTOFF)
				{
						#ifdef DEVELOPER
					if (DiagMenu.GetBool(DiagMenuIDs.DM_UNDERGROUND_SHOW_BREADCRUMB) )
					{
						float intensity = (1-ratio) * 255;
						Debug.DrawLine(GetGame().GetPlayer().GetPosition() + "0 1 0", m_TransitionalTrigger.m_Data.Breadcrumbs[i].GetPosition(),ARGB(0,255,intensity,intensity),ShapeFlags.ONCE);
					}
					#endif
					eyeAcco += ratio * m_TransitionalTrigger.m_Data.Breadcrumbs[i].EyeAccommodation;
					
				}

			}
			m_EyeAccoTarget = eyeAcco * ACCO_MODIFIER;
		}
	}
	
	private void ProcessEyeAcco(float timeSlice)
	{
		CalculateEyeAccoTarget();
		bool reachedTarget = CalculateEyeAcco(timeSlice);
		ApplyEyeAcco();
		if(reachedTarget && !m_Player.m_UndergroundPresence)
		{
			GetRequester().Stop();
			m_NVRequester.SetUndergroundExposureCoef(1.0);
			m_Player.KillUndergroundHandler();
		}

	}
	
	private void ProcessLighting(float timeSlice)
	{
		#ifdef DEVELOPER
		if (!DiagMenu.GetBool(DiagMenuIDs.DM_UNDERGROUND_DISABLE_DARKENING) )
		{
			GetGame().GetWorld().SetUserLightingLerp(m_LightingLerp);
		}
		else
		{
			GetGame().GetWorld().SetUserLightingLerp(0);
		}
		#else
		GetGame().GetWorld().SetUserLightingLerp(m_LightingLerp);
		#endif
	}
	
	private void ProcessSound(float timeSlice)
	{
		GetGame().GetWorld().SetExplicitVolumeFactor_EnvSounds2D(m_EyeAcco, 0);
		if (m_AmbientSound)
		{
			m_AmbientSound.SetSoundVolume(1-m_EyeAcco);
			//Print(m_AmbientSound.GetSoundVolume());
		}
	}
	
	void Tick(float timeSlice)
	{
		if (!m_Player.IsAlive())
			return;
		
		ProcessEyeAcco(timeSlice);
		ProcessLighting(timeSlice);
		ProcessSound(timeSlice);
		
		#ifdef DEVELOPER
		if ( DiagMenu.GetBool(DiagMenuIDs.DM_UNDERGROUND_SHOW_BREADCRUMB) )
		{
			DisplayDebugInfo(GetGame().GetWorld().GetEyeAccom(), m_LightingLerp);
		}
		#endif
		
	}
	
	private void ApplyEyeAcco()
	{
		#ifdef DEVELOPER
		if (!DiagMenu.GetBool(DiagMenuIDs.DM_UNDERGROUND_DISABLE_DARKENING) )
		{
			GetRequester().SetEyeAccommodation(m_EyeAcco);
		}
		else
		{
			GetRequester().SetEyeAccommodation(1);
		}
		#else
		GetRequester().SetEyeAccommodation(m_EyeAcco);
		#endif
		
		float undergrounNVExposureCoef = m_EyeAcco;
		if (m_LightingLerp >= 1.0 || GetDayZGame().GetWorld().IsNight())
		{
			undergrounNVExposureCoef = 1.0;
		}
		m_NVRequester.SetUndergroundExposureCoef(undergrounNVExposureCoef);
	}
	
	private bool CalculateEyeAcco(float timeSlice)
	{
		if (m_TransitionalTrigger || !m_Player.m_UndergroundPresence || (m_EyeAccoTarget == 1))
		{
			float accoDiff = m_EyeAccoTarget - m_EyeAcco;
			float increase = accoDiff * m_AccoInterpolationSpeed * timeSlice;
			m_EyeAcco += increase;
			if (Math.AbsFloat(accoDiff) < 0.01)
			{
				m_EyeAcco = m_EyeAccoTarget;
				return true;
			}
		}
		else
		{
			m_EyeAcco = m_EyeAccoTarget;
		}
		return false;

	}
	
	
	
	private void OnTriggerInsiderUpdate()
	{
		EUndergroundTriggerType bestType = EUndergroundTriggerType.UNDEFINED;
		m_TransitionalTrigger = null;
		UndergroundTrigger bestTrigger;
		m_EyeAccoTarget = 1;
		m_AccoInterpolationSpeed = DEFAULT_INTERPOLATION_SPEED;

		foreach (auto t:m_InsideTriggers)
		{
			if (t.m_Type > bestType)
			{
				bestTrigger = t;
				bestType = t.m_Type; 
			}
		}
		//Print(m_InsideTriggers.Count());
		//Print(bestType);
		if (bestTrigger)
		{
			if (bestTrigger.m_Type == EUndergroundTriggerType.TRANSITIONING)
			{
				m_TransitionalTrigger = bestTrigger;
			}
			m_EyeAccoTarget = bestTrigger.m_Accommodation;
			if (bestTrigger.m_InterpolationSpeed != -1 && bestTrigger.m_InterpolationSpeed != 0)
				m_AccoInterpolationSpeed = bestTrigger.m_InterpolationSpeed;
		}
		
		SetUndergroundPresence(bestTrigger);
	}
	
	
	private void SetUndergroundPresence(UndergroundTrigger trigger)
	{
		EUndergroundPresence newPresence = EUndergroundPresence.NONE;
		EUndergroundPresence oldPresence = m_Player.m_UndergroundPresence;
		
		if (trigger)
		{
			if (trigger.m_Type == EUndergroundTriggerType.OUTER)
			{
				newPresence = EUndergroundPresence.OUTER;
			}
			else if (trigger.m_Type == EUndergroundTriggerType.TRANSITIONING)
			{
				newPresence = EUndergroundPresence.TRANSITIONING;
			}
			else if (trigger.m_Type == EUndergroundTriggerType.INNER)
			{
				newPresence = EUndergroundPresence.FULL;
			}
		}
		
		if (newPresence != oldPresence)//was there a change ?
		{
			OnUndergroundPresenceUpdate(newPresence,oldPresence);
			m_Player.SetUnderground(newPresence);
		}
				
		
	}
	
	private void EnableLights(bool enable)
	{
		foreach (ScriptedLightBase light:ScriptedLightBase.m_NightTimeOnlyLights)
		{
			light.SetVisibleDuringDaylight(enable);
		}
	}
	
	void OnUpdateTimerEnd();
	
	void OnUpdateTimerIn()
	{
		if (!m_AnimTimerLightBlend)
			return;
		float value01 = m_AnimTimerLightBlend.GetValue();
		float result = Easing.EaseInQuint(value01);
		m_LightingLerp = result;
		
	}
	
	void OnUpdateTimerOut()
	{
		if (!m_AnimTimerLightBlend)
			return;
		float value01 = m_AnimTimerLightBlend.GetValue();
		float result = Easing.EaseOutCubic(value01);
		m_LightingLerp = result;
	}
	
	
	private void OnUndergroundPresenceUpdate(EUndergroundPresence newPresence, EUndergroundPresence oldPresence)
	{
		//Print("-----> On Undeground Presence update " + EnumTools.EnumToString(EUndergroundPresence, newPresence) + " " + EnumTools.EnumToString(EUndergroundPresence, oldPresence));
		if (newPresence > EUndergroundPresence.NONE)
		{
			if (oldPresence == EUndergroundPresence.NONE)
			{
				EnableLights(true);
			}
			if (newPresence > EUndergroundPresence.OUTER && oldPresence <= EUndergroundPresence.OUTER)
			{
				GetGame().GetWeather().SuppressLightningSimulation(true);
				m_Player.PlaySoundSetLoop(m_AmbientSound, "Underground_SoundSet",3,3);
			}
			if (newPresence == EUndergroundPresence.FULL)
			{
				m_AnimTimerLightBlend = new AnimationTimer();
				m_AnimTimerLightBlend.Run(1, this, "OnUpdateTimerIn", "OnUpdateTimerEnd",0, false, LIGHT_BLEND_SPEED_IN);
			}
		}
		if (newPresence < EUndergroundPresence.FULL && oldPresence == EUndergroundPresence.FULL)
		{
			m_AnimTimerLightBlend = new AnimationTimer();
			m_AnimTimerLightBlend.Run(0, this, "OnUpdateTimerOut", "OnUpdateTimerEnd",m_LightingLerp, false, LIGHT_BLEND_SPEED_OUT);
		}
		if (newPresence <= EUndergroundPresence.OUTER && oldPresence > EUndergroundPresence.OUTER)
		{
			GetGame().GetWeather().SuppressLightningSimulation(false);
			if (m_AmbientSound)
				m_Player.StopSoundSet(m_AmbientSound);
		}
		if (newPresence == EUndergroundPresence.NONE && oldPresence >= EUndergroundPresence.OUTER)
		{
			GetGame().GetWorld().SetUserLightingLerp(0);
			EnableLights(false);
		}
	}
	
	#ifdef DEVELOPER
	protected void DisplayDebugInfo(float acco, float lighting)
	{
		if (acco < 0.0001)
			acco = 0;
		DbgUI.Begin(String("Underground Areas"), 20, 20);
		DbgUI.Text(String("Eye Accomodation: " + acco.ToString()));
		DbgUI.Text(String("Lighting lerp: " + lighting.ToString()));
		DbgUI.End();
	}
	#endif
} 