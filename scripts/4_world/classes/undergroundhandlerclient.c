enum EUndergroundPresence
{
	NONE,//player is not interacting with underground at any level
	OUTER,//player is on the outskirts of the underdound, some effects are already in effect, while others might not be
	TRANSITIONING,//player has entered underground and is in the process of screen darkening transition
	FULL//the player is now fully entered underground
}

class UndergroundHandlerClient
{
	const float MAX_RATIO = 0.9;//how much max ratio between 0..1 can a single breadcrumb occupy 
	const float RATIO_CUTOFF = 0;//what's the minimum ratio a breadcrumb needs to have to be considered when calculatiing accommodation
	const float DISTANCE_CUTOFF = 5;//we ignore breadcrumbs further than this distance
	const float ACCO_MODIFIER = 1;//when we calculate eye accommodation between 0..1 based on the breadcrumbs values and distances, we multiply the result by this modifier to get the final eye accommodation value
	const float DEFAULT_INTERPOLATION_SPEED = 7;

	
	PlayerBase m_Player;
	PPERUndergroundAcco m_Requester;
	ref set<UndergroundTrigger> m_InsideTriggers = new set<UndergroundTrigger>();
	
	float 		m_EyeAccoTarget = 1;
	float 		m_InterpolationSpeed;
	float 		m_EyeAcco;
	bool 		m_EyeAccoAdjust;
	EffectSound m_AmbientSound;
	
	
	UndergroundTrigger m_TransitionalTrigger;
	
	void UndergroundHandlerClient(PlayerBase player)
	{
		//Print("constructor called");
		m_Player = player;
	}
	
	void ~UndergroundHandlerClient()
	{
		//Print("destructor called");
		if (m_Player)
			OnUndergroundPresenceUpdate(EUndergroundPresence.NONE, m_Player.m_UndergroundPresence);
	}
	
	PPERUndergroundAcco GetRequester()
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
	
	void Tick(float timeSlice)
	{
		if (!m_EyeAccoAdjust)
			return;
		if (!m_Player.IsAlive())
			return;
		
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
			//Print(sumCheck);
			//Print(eyeAcco);

			
		}
		#ifdef DEVELOPER
		if (!DiagMenu.GetBool(DiagMenuIDs.DM_UNDERGROUND_DISABLE_DARKENING) )
		{
			ApplyAccommodation(timeSlice);
		}
		else
		{
			GetRequester().SetEyeAccommodation(1);
		}
		#else
			ApplyAccommodation(timeSlice);
		#endif
	}
	
	void ApplyAccommodation(float timeSlice)
	{
		float finalAcco = m_EyeAccoTarget;
		if (m_TransitionalTrigger || !m_Player.m_UndergroundPresence || (m_EyeAccoTarget == 1))
		{
			float currentAcco = GetGame().GetWorld().GetEyeAccom();
			float accoDiff = m_EyeAccoTarget - currentAcco;
			float increase = accoDiff * m_InterpolationSpeed * timeSlice;
			
			if (!m_Player.m_UndergroundPresence && Math.AbsFloat(accoDiff) < 0.01)
			{
				GetRequester().SetEyeAccommodation(m_EyeAccoTarget);
				m_EyeAccoAdjust = false;
				GetRequester().Stop();
			}
			
			finalAcco = currentAcco + increase;
		}
		//Print(m_EyeAccoTarget);
		//Print(finalAcco);
		
		#ifdef DEVELOPER
		if ( DiagMenu.GetBool(DiagMenuIDs.DM_UNDERGROUND_SHOW_BREADCRUMB) )
		{
			DisplayDebugInfo(finalAcco);
		}
		#endif
		
		GetRequester().SetEyeAccommodation(finalAcco);
	}
	
	void OnTriggerInsiderUpdate()
	{
		EUndergroundTriggerType bestType = EUndergroundTriggerType.UNDEFINED;
		m_TransitionalTrigger = null;
		UndergroundTrigger bestTrigger;
		m_EyeAccoTarget = 1;
		m_InterpolationSpeed = DEFAULT_INTERPOLATION_SPEED;

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
				m_InterpolationSpeed = bestTrigger.m_InterpolationSpeed;
		}
		
		SetUndergroundPresence(bestTrigger);
		m_EyeAccoAdjust = true;
	}
	
	
	void SetUndergroundPresence(UndergroundTrigger trigger)
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
	
	void EnableLights(bool enable)
	{
		foreach (ScriptedLightBase light:ScriptedLightBase.m_NightTimeOnlyLights)
		{
			light.SetVisibleDuringDaylight(enable);
		}
	}
	
	/*
	void ResetEnviroSettings()
	{
		if (GetGame())
		{
			GetGame().GetWorld().SetExplicitVolumeFactor_EnvSounds2D(1, 0.5);
			GetGame().GetWeather().SuppressLightningSimulation(false);
		}
	}*/
	
	void OnUndergroundPresenceUpdate(EUndergroundPresence newPresence, EUndergroundPresence oldPresence)
	{
		//Print("-----> On Undeground Presence update " + EnumTools.EnumToString(EUndergroundPresence, newPresence) + " " + EnumTools.EnumToString(EUndergroundPresence, oldPresence));
		if (newPresence > EUndergroundPresence.NONE)
		{
			if (oldPresence == EUndergroundPresence.NONE)
			{
				//Print("Enable lights");
				EnableLights(true);
			}
			if (newPresence > EUndergroundPresence.OUTER && oldPresence <= EUndergroundPresence.OUTER)
			{
				//Print("ON Enable everything else");
				GetGame().GetWorld().SetExplicitVolumeFactor_EnvSounds2D(0, 0.5);
				GetGame().GetWeather().SuppressLightningSimulation(true);
				m_Player.PlaySoundSetLoop(m_AmbientSound, "Underground_SoundSet",3,3);
			}
			
		}
		if (newPresence <= EUndergroundPresence.OUTER && oldPresence > EUndergroundPresence.OUTER)
		{
			//Print("OFF Disable everythiing else");
			GetGame().GetWorld().SetExplicitVolumeFactor_EnvSounds2D(1, 0.5);
			GetGame().GetWeather().SuppressLightningSimulation(false);
			if (m_AmbientSound)
				m_Player.StopSoundSet(m_AmbientSound);
		}
		if (newPresence == EUndergroundPresence.NONE && oldPresence >= EUndergroundPresence.OUTER)
		{
			//Print("Disable lights");
			EnableLights(false);
		}
	}
	
	#ifdef DEVELOPER
	protected void DisplayDebugInfo(float acco)
	{
		if (acco < 0.0001)
			acco = 0;
		DbgUI.Begin(String("Underground Areas"), 20, 20);
		DbgUI.Text(String("Eye Accomodation: " + acco.ToString()));
		DbgUI.End();
	}
	#endif
} 