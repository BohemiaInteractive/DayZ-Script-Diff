//! limit function
float 	Limit(float pV, float pMin, float pMax)
{
	if (pV >= pMin && pV <= pMax)
	{
		return pV;
	}
	else if (pV < pMin)
	{
		return pMin;
	}
	else
	{
		return pMax;
	}	
}


float fixAngle_PI_PI(float pAngle)
{
	while (pAngle > Math.PI)
	{

		pAngle -=  Math.PI2;
	}

	while (pAngle < -Math.PI)
	{

		pAngle +=  Math.PI2;
	}

	return pAngle;
}

float fixAngle_180_180(float pAngle)
{
	while (pAngle > 180)
	{

		pAngle -=  360;
	}

	while (pAngle < -180)
	{

		pAngle +=  360;
	}

	return pAngle;
}


enum NVTypes
{
	NONE = 0,
	NV_GOGGLES,
	NV_GOGGLES_OFF,
	NV_OPTICS_ON,
	NV_OPTICS_OFF,
	NV_PUMPKIN,
	NV_OPTICS_KAZUAR_DAY,
	NV_OPTICS_KAZUAR_NIGHT,
	NV_OPTICS_STARLIGHT_DAY,
	NV_OPTICS_STARLIGHT_NIGHT,
	MAX
}


class DayZPlayerCameraBase extends DayZPlayerCamera
{
	static const float 	CONST_NEARPLANE_OPTICS_MIN = 0.04; //Minimal safe near plane value, artifacts on the far plane otherwise!
	
	protected 	Weapon_Base		m_weaponUsed;
	protected 	ItemOptics 		m_opticsUsed;
	protected	ref CameraShake		m_CameraShake;
	
	//! constructor must be same 
	void 	DayZPlayerCameraBase(DayZPlayer pPlayer, HumanInputController pInput)
	{
		m_fLRAngleVel[0] = 0;
		m_fUDAngleVel[0] = 0;
		m_fFovAbsVel[0] = 0;
		m_WeaponSwayModifier = 1;
		
		if (m_pPlayer.IsEyeZoom())
		{
			m_fFovAbsolute		= GameConstants.DZPLAYER_CAMERA_FOV_EYEZOOM;
		}
		else
		{
			m_fFovAbsolute		= g_Game.GetUserFOV();
		}
		
		//!
		/*
		{
			int perItemCamUD = pPlayer.GetCurrentPerItemCameraUD();
			string 		a	= "Per Item Camera User Data: " + perItemCamUD.ToString();
			Print(a);
		}
		*/
		m_CameraPPDelay = 0;

		m_CommandWeapons = pPlayer.GetCommandModifier_Weapons();
		Init();
	};
	
	
	void ProcessCameraShake(float delta, out float leftRight, out float upDown)
	{
		if (m_CameraShake)
		{
			float x,y;
			m_CameraShake.Update(delta, x, y);
			leftRight += x;
			upDown += y;
		}
	}
	
	void Init()
	{
		PlayerBase player = PlayerBase.Cast(m_pPlayer);
		if ( player && player.IsNVGWorking() != IsCameraNV() )
		{
			SetCameraNV(player.IsNVGWorking());
			SetCameraNVType(player.GetNVType());
		}
	}

	float UpdateUDAngle(out float pAngle, out float pAngleAdd, float pMin, float pMax, float pDt)
	{
		if (Math.AbsFloat(pAngleAdd) > 0.001)
		{
			float aimingUDAngle = m_CommandWeapons.GetBaseAimingAngleUD();

			//!	pAngle + pAngleAdd == aimingUDAngle + aimingUDAdd
			//! now we set pAngleAdd to be the differenc from aimingAngle and we change camera angle to be the aiming angle

			// override actual angle
			pAngleAdd 	= pAngle + pAngleAdd - aimingUDAngle;
			pAngle 		= aimingUDAngle;

			// Print("Angle: " + pAngle.ToString() + " Aim: " + actualUDAngle.ToString() );
		}

		//! lr angle
		if( m_pInput.CameraIsFreeLook() )	
		{
			pAngleAdd 	+= m_pInput.GetAimChange()[1] * Math.RAD2DEG;
			pAngleAdd	= Limit(pAngleAdd, pMin, pMax);

			m_fUDAngleVel[0]	= 0;
		}
		else
		{
			if( m_pInput.CameraIsTracking() )
			{
				pAngleAdd 	= m_pInput.GetTracking()[1] * Math.RAD2DEG;
				pAngleAdd	= Limit(pAngleAdd, pMin, pMax);

				m_fUDAngleVel[0]	= 0;
			}
			else
			{	
				//! update it in degrees
				pAngle 	+= m_pInput.GetAimChange()[1] * Math.RAD2DEG;
				pAngle 	= Limit(pAngle, pMin, pMax);

				pAngleAdd	= Math.SmoothCD(pAngleAdd, 0, m_fUDAngleVel, 0.14, 1000, pDt);
			}
		}
		
		/*{
			float change = m_pInput.GetAimChange()[1] * Math.RAD2DEG;

			Print ("Script: Camera ud angle: " +  pAngle.ToString() + " change: " + change.ToString() );
		}*/		

		return Limit(pAngle + pAngleAdd, pMin, pMax);
	}

	float UpdateLRAngle(float pAngle, float pMin, float pMax, float pDt)
	{
		//! lr angle
		if (m_pInput.CameraIsFreeLook() || m_bForceFreeLook)	
		{
			//!
			pAngle	+= m_pInput.GetAimChange()[0] * Math.RAD2DEG;
			pAngle	= Limit(pAngle, pMin, pMax);

			m_fLRAngleVel[0]	= 0;	// reset filter

		}
		else
		{
			if( m_pInput.CameraIsTracking() )
			{
				pAngle = m_pInput.GetTracking()[0] * Math.RAD2DEG;
				pAngle	= Limit(-pAngle, pMin, pMax);
				
				m_fLRAngleVel[0]	= 0;	// reset filter
			}
			else
			{	
				// smooth value back to 0 
				pAngle		= Math.SmoothCD(pAngle, 0, m_fLRAngleVel, 0.14, 1000, pDt);
				// m_fLeftRightAngle	= 0.9 * (1.0 - pDt);			
			}
		}

		return pAngle;
	}

	

	void 	StdFovUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
		//! change abs FOV for naked eye zoom
		if (m_pPlayer.IsEyeZoom())
		{
			m_fFovAbsolute = Math.SmoothCD(m_fFovAbsolute, GameConstants.DZPLAYER_CAMERA_FOV_EYEZOOM, m_fFovAbsVel, 0.1, 1000, pDt);
		}
		else
		{
			m_fFovAbsolute = Math.SmoothCD(m_fFovAbsolute, g_Game.GetUserFOV(), m_fFovAbsVel, 0.1, 1000, pDt);
		}

		pOutResult.m_fFovAbsolute = m_fFovAbsolute;

		//! switch shooting from camera to weapon (firearms)
		if (!m_pPlayer.IsShootingFromCamera())
		{
			pOutResult.m_fShootFromCamera = 0.0;
		}
	}

	override void SpawnCameraShakeProper(float strength, float radius, float smoothness, float radius_decay_speed)
	{
		m_CameraShake = new CameraShake(strength, radius, smoothness, radius_decay_speed);
	}	

	override void OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
		if (!m_pPlayer || !PlayerBase.Cast(m_pPlayer))
		{
			Debug.Log("DayZPlayerCameraBase | OnUpdate | no player!");
			return;
		}
		
		if (PlayerBase.Cast(m_pPlayer).GetCurrentCamera() != this)
		{
			//Print("DayZPlayerCameraBase | OnUpdate | unused camera! | " + this + "/" + PlayerBase.Cast(m_pPlayer).GetCurrentCamera());
			return;
		}
		
		super.OnUpdate(pDt, pOutResult);
		StdFovUpdate(pDt, pOutResult);
		UpdateCameraNV(PlayerBase.Cast(m_pPlayer));
		InitCameraOnPlayer();
	}

	override void OnActivate(DayZPlayerCamera pPrevCamera, DayZPlayerCameraResult pPrevCameraResult)
	{
		//PrintString("OnActivate DayZPlayerCameraBase");
		InitCameraOnPlayer(true);
		SetCameraPPDelay(pPrevCamera);
		
		if (DayZPlayerCameraBase.Cast(pPrevCamera) && DayZPlayerCameraBase.Cast(pPrevCamera).IsCameraNV() && !IsCameraNV())
		{
			PPERequesterBank.GetRequester(PPERequesterBank.REQ_CAMERANV).Start( new Param1<int>(PPERequester_CameraNV.NV_TRANSITIVE) );
		}
		
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(SetCameraPP,m_CameraPPDelay*1000,false,true,this); // this takes care of weapon/optics postprocessing
		DayZPlayerCameraOptics optic_camera;
		if (DayZPlayerCamera.CastTo(optic_camera,pPrevCamera))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(PlayerBase.Cast(m_pPlayer).HideClothing,null,false);
		}
	}
	
	float GetWeaponSwayModifier()
	{
		return m_WeaponSwayModifier;
	}
	
	override string GetCameraName()
	{
		return "DayZPlayerCameraBase";
	}
	
	void SetCameraPPDelay(DayZPlayerCamera pPrevCamera)
	{
	}
	
	void SetCameraNV(bool nightvision) //TODO - revise use, may be set more often than needed
	{
		m_IsNightvision = nightvision;
	}
	
	bool IsCameraNV()
	{
		return m_IsNightvision;
	}
	
	void SetCameraNVType(int type)
	{
		m_NightvisionType = type;
	}
	
	int GetCameraNVType()
	{
		return m_NightvisionType;
	}
	
	void UpdateCameraNV(PlayerBase player)
	{
		if ( !player )
			return;
		
		if ( player.IsNVGWorking() != IsCameraNV() || player.GetNVType() != GetCameraNVType() )
		{
			SetCameraNV(player.IsNVGWorking());
			SetCameraNVType(player.GetNVType()); //TODO - ???
			SetCameraPP(true, this);
		}
	}
	
	//! by default sets camera PP to zero, regardless of parameter. Override if needed.
	void SetCameraPP(bool state, DayZPlayerCamera launchedFrom)
	{
		PPERequesterBank.GetRequester(PPERequester_CameraADS).Stop();
		
		if (IsCameraNV())
		{
			SetNVPostprocess(GetCameraNVType());
		}
		else
		{
			SetNVPostprocess(NVTypes.NONE);
		}
		
		m_weaponUsed = Weapon_Base.Cast(m_pPlayer.GetHumanInventory().GetEntityInHands());
		if (m_weaponUsed)
		{
			m_weaponUsed.HideWeaponBarrel(false);
		}
	}
	
	override float GetCurrentPitch()
	{
		return m_CurrentCameraPitch;
	}
	
	void ForceFreelook(bool state)
	{
		m_bForceFreeLook = state;
	}
	
	//TODO - better handling of distinct occluder types
	//! DayZPlayerCameraOptics handles this separately, otherwise it takes active NVType from PlayerBase::GetCameraNVType
	void SetNVPostprocess(int NVtype)
	{
		//Print("+++Setting NV type: " + NVtype + " +++");
		switch (NVtype)
		{
			case NVTypes.NONE:
			{
				PPERequesterBank.GetRequester(PPERequester_CameraNV).Stop();
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.NVG_OCCLUDER});
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.PUMPKIN_OCCLUDER});
			}
			break;
			
			//night-vision modes
			case NVTypes.NV_OPTICS_KAZUAR_NIGHT:
			case NVTypes.NV_OPTICS_STARLIGHT_NIGHT:
			case NVTypes.NV_OPTICS_ON:
			{
				PPERequesterBank.GetRequester(PPERequesterBank.REQ_CAMERANV).Start( new Param1<int>(PPERequester_CameraNV.NV_DEFAULT_OPTICS) );
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.NVG_OCCLUDER});
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.PUMPKIN_OCCLUDER});
			}
			break;
			
			//daytime filter modes; TODO!
			case NVTypes.NV_OPTICS_KAZUAR_DAY:
			case NVTypes.NV_OPTICS_STARLIGHT_DAY:
			{
				PPERequesterBank.GetRequester(PPERequesterBank.REQ_CAMERANV).Start( new Param1<int>(PPERequester_CameraNV.NV_DAYTIME_OPTICS) );
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.NVG_OCCLUDER});
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.PUMPKIN_OCCLUDER});
			}
			break;
			
			case NVTypes.NV_GOGGLES_OFF:
			case NVTypes.NV_OPTICS_OFF:
			{
				PPERequesterBank.GetRequester(PPERequesterBank.REQ_CAMERANV).Start( new Param1<int>(PPERequester_CameraNV.NV_NO_BATTERY) );
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.NVG_OCCLUDER});
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.PUMPKIN_OCCLUDER});
			}
			break;
			
			case NVTypes.NV_GOGGLES:
			{
				PPERequesterBank.GetRequester(PPERequesterBank.REQ_CAMERANV).Start( new Param1<int>(PPERequester_CameraNV.NV_DEFAULT_GLASSES) );
				GetGame().GetMission().GetEffectWidgets().AddActiveEffects({EffectWidgetsTypes.NVG_OCCLUDER});
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.PUMPKIN_OCCLUDER});
			}
			break;
			
			case NVTypes.NV_PUMPKIN:
			{
				PPERequesterBank.GetRequester(PPERequesterBank.REQ_CAMERANV).Start( new Param1<int>(PPERequester_CameraNV.NV_PUMPKIN) );
				GetGame().GetMission().GetEffectWidgets().AddActiveEffects({EffectWidgetsTypes.PUMPKIN_OCCLUDER});
				GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.NVG_OCCLUDER});
			}
			break;
		}
		
		if (PlayerBaseClient.Cast(m_pPlayer))
		{
			PlayerBaseClient.Cast(m_pPlayer).SwitchPersonalLight(NVtype < 1); //TODO 
		}
	}
	
	void InitCameraOnPlayer(bool force = false)
	{
		//Print("" + this + " | " + GetGame().GetTime());
		PlayerBase player = PlayerBase.Cast(m_pPlayer);
		if (!player.GetCurrentPlayerCamera() || (force && player.GetCurrentPlayerCamera() != this))
		{
			player.OnCameraChanged(this);
		}
	}
	
	protected float 				m_fLRAngleVel[1];
	protected float 				m_fUDAngleVel[1];
	protected float					m_fFovAbsVel[1];
	protected float					m_fFovAbsolute;
	protected bool					m_bForceFreeLook;
	protected float					m_WeaponSwayModifier;
	protected float 				m_CameraPPDelay;
	protected float 				m_CurrentCameraPitch;
	protected HumanCommandWeapons	m_CommandWeapons;
	protected bool 					m_IsNightvision;
	protected int 					m_NightvisionType;
}
