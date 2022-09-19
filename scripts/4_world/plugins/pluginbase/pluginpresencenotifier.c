enum EPresenceNotifierNoiseEventType
{
	LAND_LIGHT,
	LAND_HEAVY,
}

class PresenceNotifierNoiseEvent
{
	protected float m_TimerLength;
	protected int m_Value;
	
	void PresenceNotifierNoiseEvent(float pValue, float pLength)
	{
		m_Value = pValue;
		m_TimerLength = pLength;
	}

	float GetTimerLength()
	{
		return m_TimerLength;
	}
	
	int GetValue()
	{
		return m_Value;
	}
}

class PresenceNotifierNoiseEvents
{
	protected int m_Value;

	protected static ref map<EPresenceNotifierNoiseEventType, ref PresenceNotifierNoiseEvent> 	m_PresenceNotifierNotifierEvents;
	protected ref Timer	m_CooldownTimer;
	
	void PresenceNotifierNoiseEvents()
	{
		m_Value = 0;
		m_CooldownTimer = new Timer(CALL_CATEGORY_SYSTEM);
		m_PresenceNotifierNotifierEvents = new ref map<EPresenceNotifierNoiseEventType, ref PresenceNotifierNoiseEvent>;
	}

	void RegisterEvent(EPresenceNotifierNoiseEventType pEventType, int pValue, float pLength)
	{
		PresenceNotifierNoiseEvent pnne = new PresenceNotifierNoiseEvent(pValue, pLength);
		m_PresenceNotifierNotifierEvents.Insert(pEventType, pnne);
	}
	
	void ProcessEvent(EPresenceNotifierNoiseEventType pEventType)
	{
		PresenceNotifierNoiseEvent pnne;
		
		pnne = m_PresenceNotifierNotifierEvents.Get(pEventType);
		
		if (m_CooldownTimer.IsRunning())
		{
			m_CooldownTimer.Stop();
		}

		m_Value = pnne.GetValue();		
		m_CooldownTimer.Run(pnne.GetTimerLength(), this, "ResetEvent", null);
	}
	
	int GetValue()
	{
		return m_Value;
	}

	protected void ResetEvent()
	{
		m_Value = 0;
	}
}


class PluginPresenceNotifier extends PluginBase
{
	//! dbgUI settings
	const int windowPosX = 0;
	const int windowPosY = 10;
	const int mainPanelSizeX = 200;
	const int mainPanelSizeY = 1;
	const int margin = 10;
	
	
	//! noise limits
	const int NOISE_LEVEL_MIN			= 0;
	const int NOISE_LEVEL_MAX			= 5;
	
	//! noise component from surfaces
	const int SURFACE_NOISE_LVL0		= 0;
	const int SURFACE_NOISE_LVL1		= 1;
	const int SURFACE_NOISE_LVL2		= 2;
	
	const float SURFACE_LVL2_THRESHOLD 	= 1.0;
	const float SURFACE_LVL1_THRESHOLD 	= 0.5;
	
	//! land noise
	const int LAND_NOISE_LVL1			= 2;
	const int LAND_NOISE_LVL2			= 3;
	
	//! --------------------------------------
	
	protected PlayerBase 		m_pPlayer;
	
	protected ref PresenceNotifierNoiseEvents m_PresenceNotifierNoiseEvents;

	void PluginPresenceNotifier()
	{
		m_PresenceNotifierNoiseEvents = new PresenceNotifierNoiseEvents();
		m_PresenceNotifierNoiseEvents.RegisterEvent(EPresenceNotifierNoiseEventType.LAND_LIGHT, LAND_NOISE_LVL1, 0.25);
		m_PresenceNotifierNoiseEvents.RegisterEvent(EPresenceNotifierNoiseEventType.LAND_HEAVY, LAND_NOISE_LVL2, 0.25);
	}
	
	void ~PluginPresenceNotifier() {}
	
	void Init(PlayerBase player)
	{
		m_pPlayer = player;
	}

	void EnableDebug(bool pEnabled)
	{		
		ShowCoefsDbg(pEnabled);
	}
	
	protected void ShowCoefsDbg(bool pEnabled)
	{
		DbgUI.BeginCleanupScope();

		if (pEnabled && m_pPlayer)
		{
			HumanMovementState hms = new HumanMovementState();
			m_pPlayer.GetMovementState(hms);
			
			string visualAlertLevel;
			string noiseAlertLevel;

			DbgUI.Begin("Presence Notifier", windowPosX + 10, windowPosY);
			DbgUI.Panel("MinimumSize", mainPanelSizeX, mainPanelSizeY);

			DbgUI.Text("Visual: ");
			DbgUI.Text("Visibility: " + m_pPlayer.GetVisibilityCoef());
			DbgUI.Text("Speed: " + GetMovementSpeedVisualCoef());
			DbgUI.Text("Stance: " + GetMovementStanceVisualCoef());
			DbgUI.Spacer(10);
		
			DbgUI.Panel("-- Noises", mainPanelSizeX, 2);
			DbgUI.Text("Noises: ");
			DbgUI.Text("Speed:  " + NoiseAIEvaluate.GetNoiseMultiplierByPlayerSpeed(m_pPlayer));
			DbgUI.Text("Boots: " + NoiseAIEvaluate.GetNoiseMultiplierByShoes(m_pPlayer));
			DbgUI.Text("Surface: " + NoiseAIEvaluate.GetNoiseMultiplierBySurface(m_pPlayer) + " [ cfg: " + m_pPlayer.GetSurfaceNoise() + "]");
			DbgUI.Spacer(10);

			DbgUI.Panel("-- Result", mainPanelSizeX, 2);
			DbgUI.Text("Result: ");
			visualAlertLevel = "";
			for (int iv = 0; iv < GetVisualPresence(); iv++)
			{
				visualAlertLevel += "!";
			}
			DbgUI.Text("Visual level: " + GetVisualPresence() + " [" + visualAlertLevel + "]");

						
			noiseAlertLevel = "";
			for (int ia = 0; ia < GetNoisePresence(); ia++)
			{
				noiseAlertLevel += "!";
			}

			DbgUI.Text("Noise level: " + GetNoisePresence() + " [" + noiseAlertLevel + "]");
		
			DbgUI.End();
			
			//! HumanMovementState content
			DbgUI.Begin("HumanMovementState", windowPosX + 250, windowPosY);
			DbgUI.Panel("MinimumSize", mainPanelSizeX, mainPanelSizeY);
			DbgUI.Text("Command ID: " + hms.m_CommandTypeId);
			DbgUI.Text("Stance: " + hms.m_iStanceIdx);
			DbgUI.Text("Movement: " + hms.m_iMovement);
			DbgUI.End();
		}

		DbgUI.EndCleanupScope();
	}
	
	//! returns actual noise presence of player
	int GetNoisePresence()
	{		
		return ProcessNoiseComponents();
	}
	
	//! returns actual visibility presence of player
	int GetVisualPresence()
	{
		return ProcessVisualComponents();
	}

	//! processing of external one-time events (land, fire, etc.)
	void ProcessEvent(EPresenceNotifierNoiseEventType pEventType)
	{
		m_PresenceNotifierNoiseEvents.ProcessEvent(pEventType);
	}
	
	protected int ProcessVisualComponents()
	{
		float visualMean = 0;
		if (m_pPlayer)
		{
			visualMean = (m_pPlayer.GetVisibilityCoef() + GetMovementSpeedVisualCoef() + GetMovementStanceVisualCoef()) / 3;
		}
		
		return visualMean;	
	}

	protected int ProcessNoiseComponents()
	{
		float noise = 0;
		if (m_pPlayer)
		{
			noise = NoiseAIEvaluate.GetNoiseMultiplier(m_pPlayer);
			noise = Math.Round(noise * NOISE_LEVEL_MAX);
		}
		
		return Math.Clamp(noise, NOISE_LEVEL_MIN, NOISE_LEVEL_MAX);
	}
	
	
	//! Visibility
	protected float GetMovementSpeedVisualCoef()
	{
		HumanMovementState hms	= new HumanMovementState();
		float speedCoef 		= 1.0;

		m_pPlayer.GetMovementState(hms);
		//Print(hms.m_iMovement);
		switch (AITargetCallbacksPlayer.StanceToMovementIdxTranslation(hms))
		{
			case DayZPlayerConstants.MOVEMENTIDX_RUN:
				speedCoef = 0.66;
			break;
			case DayZPlayerConstants.MOVEMENTIDX_WALK:
				speedCoef = 0.33;
			break;
			case DayZPlayerConstants.MOVEMENTIDX_IDLE:
				speedCoef = 0;
			break;
		}
		
		return speedCoef;
	}
	
	protected float GetMovementStanceVisualCoef()
	{
		HumanMovementState hms	= new HumanMovementState();
		float stanceCoef 		= 1.0;

		m_pPlayer.GetMovementState(hms);
		switch (hms.m_iStanceIdx)
		{
			case DayZPlayerConstants.STANCEIDX_CROUCH:
			case DayZPlayerConstants.STANCEIDX_RAISEDCROUCH:
				stanceCoef = 0.33;
			break;
				
			case DayZPlayerConstants.STANCEIDX_PRONE:
			case DayZPlayerConstants.STANCEIDX_RAISEDPRONE:
				stanceCoef = 0.11;
			break;
		}
		
		return stanceCoef;
	}
	
	//! --------------------------------------------------------


	//!
	//! Noise related
	// Not used since 1.12
	
	//! noise component of presence based on player's speed
	//! DEPRECATED
	protected int GetMovementSpeedNoiseComponent();

	//! noise component of presence based on player's shoes
	//! DEPRECATED
	protected int GetBootsNoiseComponent();
	
	//! DEPRECATED
	protected int GetSurfaceNoiseComponent();

	//! DEPRECATED
	protected int GetExternalNoiseEventsComponent();
}