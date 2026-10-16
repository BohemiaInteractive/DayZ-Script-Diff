// Enhanced light state enum with specific states
enum CodeLockLightState
{
	OFF,								// No lights, no battery
	LOCKED,				 				// Red light solid
	UNLOCKED,			  				// Green light solid
	SETTING_MODE,		  				// Green light blinking
	BRUTE_FORCE_PROTECTION 				// Red light blinking
}
	
class CodeLockVisualManager
{
	private DigitalCodeLock m_CodeLock;
	private CodeLockComponent m_Component;
	
	// Current visual state tracking
	private CodeLockLightState m_CurrentLightState;
	private bool m_IsBlinking;
	
	// State tracking for change detection
	private float m_LastBatteryPercent;
	private bool m_LastReversedState;
	private float m_LastAnimationPhaseFront;
	private float m_LastAnimationPhaseBack;
	private int m_LastVisibleBars;
	
	// Dirty flag system
	private bool m_FullVisualUpdate;
	
	// Timers for animations
	private ref Timer m_BlinkTimer;
	private float m_BlinkTime;
	private bool m_BlinkPhase; // true = light on, false = light off
		
	// Animation constants
	private const float BLINK_INTERVAL = 0.1;
	private const float BLINK_CYCLE_TIME = 1.0;
	
	// Animation phase values
	private const float LIGHT_VISIBLE = 0.0;    // Animation phase for visible light
	private const float LIGHT_HIDDEN = 1.0;     // Animation phase for hidden light
	
	private const string CODELOCK_ANIMATED_FRONT = "CodelockFrontTranslation";
	private const string CODELOCK_ANIMATED_BACK = "CodelockBackTranslation";
	private const string CODELOCK_ANIMATED_BACK_R = "CodelockBackRotation";
	private const string CODELOCK_ANIMATED_R = "CodelockRotation";
	const string CODELOCK_ANIMATED_ZIP = "CodelockZipHide";
	const string CODELOCK_ANIMATED_RED_LIGHT = "CodelockRedLightOnHide";
	const string CODELOCK_ANIMATED_GREEN_LIGHT = "CodelockGreenLightOnHide";
	const string CODELOCK_ANIMATED_BUTTON_GREEN_LIGHT = "CodelockButtonGreenLightOnHide";
	const string CODELOCK_ANIMATED_BUTTON_RED_LIGHT = "CodelockButtonRedLightOnHide";
	const string CODELOCK_ANIMATED_BATTERY_LIGHT = "CodelockBatteryLightOnHide";
	
	private const ref array<string> BATTERY_BAR_SELECTIONS = {
		"CodelockBatteryBar1Hide",
		"CodelockBatteryBar2Hide", 
		"CodelockBatteryBar3Hide",
		"CodelockBatteryBar4Hide",
		"CodelockBatteryBar5Hide",
		"CodelockBatteryBar6Hide"
	};
	
	void CodeLockVisualManager(DigitalCodeLock codeLock, CodeLockComponent component)
	{
		m_CodeLock = codeLock;
		m_Component = component;
		m_CurrentLightState = CodeLockLightState.OFF;
		m_IsBlinking = false;
		m_BlinkTime = 0;
		m_BlinkPhase = false;
		m_FullVisualUpdate = false;
		
		// Initialize tracking variables
		m_LastBatteryPercent = 0.0;
		m_LastReversedState = false;
		m_LastAnimationPhaseFront = 0.0;
		m_LastAnimationPhaseBack = 0.0;
		m_LastVisibleBars = 0;
	}
	
	void ~CodeLockVisualManager()
	{
		StopBlinking();
	}
	
	void MarkDirty()
	{
		m_FullVisualUpdate = true;
	}
	
	void UpdateVisuals()
	{
		CodeLockLightState newLightState = DetermineRequiredLightState();
		#ifdef CODELOCKCOMPONENT_LOG
		if (m_CurrentLightState != newLightState)
			ErrorEx(string.Format("Current light state=%1 | New light state=%2", typename.EnumToString(CodeLockLightState, m_CurrentLightState), typename.EnumToString(CodeLockLightState, newLightState)), ErrorExSeverity.INFO);
		#endif
		
		bool lightStateChanged = m_FullVisualUpdate || (newLightState != m_CurrentLightState);
		bool animationsChanged = m_FullVisualUpdate || HasAnimationsChanged();
		bool batteryLevelChanged = m_FullVisualUpdate || HasBatteryLevelChanged();

		// Only update if something actually changed or we're marked dirty
		if (!m_FullVisualUpdate && !lightStateChanged && !animationsChanged && !batteryLevelChanged)
		{
			#ifdef CODELOCKCOMPONENT_LOG
			ErrorEx("No visual changes detected, skipping update of visuals..", ErrorExSeverity.INFO);
			#endif
			return;
		}
		
		if (lightStateChanged)
		{
			SetLightState(newLightState);
		}
		
		if (animationsChanged)
		{
			UpdateAnimations();
		}
		
		if (batteryLevelChanged)
		{
			UpdateBatteryIndicator();
		}
		
		m_FullVisualUpdate = false;
	}
	
	// Check if animations need updating
	protected bool HasAnimationsChanged()
	{
		if (!m_CodeLock.HasValidParent())
		{
			// Detached state - check if we were previously attached
			return (m_LastAnimationPhaseFront != 0.0 || m_LastAnimationPhaseBack != 0.0);
		}
		else
		{
			// Attached state - check for changes
			bool reversed = m_CodeLock.GetReversedState();
			if (reversed != m_LastReversedState)
				return true;
			
			EntityAI hierarchyParent = m_CodeLock.GetHierarchyRoot();
			float newFrontPhase = GetAnimationPhaseForFrontPart(hierarchyParent, reversed);
			float newBackPhase = GetAnimationPhaseForBackPart(hierarchyParent, reversed);
			
			if (newFrontPhase != m_LastAnimationPhaseFront || newBackPhase != m_LastAnimationPhaseBack)
				return true;
		}
		
		return false;
	}
	
	// Check if battery level changed enough to warrant visual update
	bool HasBatteryLevelChanged()
	{
		if (!m_CodeLock.HasValidParent())
		{
			if (m_LastVisibleBars != 0)
				return true;
		}
		else
		{
			float currentPercent = m_CodeLock.GetBatteryEnergy();
			int currentBars = CalculateVisibleBars(currentPercent);
			if (currentBars != m_LastVisibleBars)
				return true;
		}
		
		return false;
	}
		
	// Determine what the light state should be based on current conditions
	protected CodeLockLightState DetermineRequiredLightState()
	{
		// No battery = everything off
		if (m_CodeLock.NeedBattery())
		{
			float energy = m_CodeLock.GetBatteryEnergy();
			if (energy <= 0 || !m_CodeLock.HasValidParent())
				return CodeLockLightState.OFF;
		}
		
		if (m_CodeLock.HasValidParent())
		{
			// Handle brute force protection
			if (m_Component.HasBruteForceProtection() || m_Component.ShouldHandleBruteForce())
				return CodeLockLightState.BRUTE_FORCE_PROTECTION;
				
			// Check if we're in setting mode
			CodeLockStage stage = m_Component.GetStage();
			if (stage == CodeLockStage.SETTING)
				return CodeLockLightState.SETTING_MODE;
			
			// If we have a PIN, show lock state
			if (m_Component.HasPIN())
			{
				if (m_Component.IsLocked())
					return CodeLockLightState.LOCKED;
				else
					return CodeLockLightState.UNLOCKED;
			}
		}
		
		// Default state with battery but no PIN
		return CodeLockLightState.OFF;
	}
		
	// Set the light state and handle state transitions
	void SetLightState(CodeLockLightState newState)
	{
		if (m_CurrentLightState == newState)
			return;
	
		// Stop any current blinking
		StopBlinking();
		
		m_CurrentLightState = newState;
		
		switch(newState)
		{
			case CodeLockLightState.OFF:
				SetAllLights(LIGHT_HIDDEN);					// Red off, Green off
				break;
			
			case CodeLockLightState.LOCKED:
				SetMainLights(LIGHT_VISIBLE, LIGHT_HIDDEN); // Red on, Green off
				SetButtonLights(LIGHT_HIDDEN, LIGHT_VISIBLE); // Green off, Red on
				break;
			
			case CodeLockLightState.UNLOCKED:
				SetMainLights(LIGHT_HIDDEN, LIGHT_VISIBLE);
				SetButtonLights(LIGHT_VISIBLE, LIGHT_HIDDEN); // Green on, Red off
				break;
			
			case CodeLockLightState.SETTING_MODE:
				SetMainLights(LIGHT_HIDDEN, LIGHT_HIDDEN);	// Red off, Green off
				SetButtonLights(LIGHT_HIDDEN, LIGHT_HIDDEN); // Both button lights start off (will blink green)
				StartBlinking(CodeLockLightState.SETTING_MODE);
				break;
			
			case CodeLockLightState.BRUTE_FORCE_PROTECTION:
				SetMainLights(LIGHT_HIDDEN, LIGHT_HIDDEN);	// Red off, Green off
				SetButtonLights(LIGHT_HIDDEN, LIGHT_HIDDEN); // Both button lights start off (will blink red)
				StartBlinking(CodeLockLightState.BRUTE_FORCE_PROTECTION);
				break;
		}
	}
	
	// Set animation phases for front light selections
	protected void SetMainLights(float redPhase, float greenPhase)
	{
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_RED_LIGHT, redPhase);
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_GREEN_LIGHT, greenPhase);
	}
	
	// Set animation phases for button light selections
	protected void SetButtonLights(float greenButtonPhase, float redButtonPhase)
	{
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_BUTTON_GREEN_LIGHT, greenButtonPhase);
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_BUTTON_RED_LIGHT, redButtonPhase);
	}
	
	protected void SetBatteryLight(float phase)
	{
		 m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_BATTERY_LIGHT, phase);
	}
		
	// Set all lights to the same phase
	protected void SetAllLights(float phase)
	{
		SetMainLights(phase, phase);
		SetButtonLights(phase, phase);
	}

	// Start blinking animation for specific states
	protected void StartBlinking(CodeLockLightState blinkState)
	{
		if (m_IsBlinking)
			return;
			
		m_IsBlinking = true;
		m_BlinkTime = 0;
		m_BlinkPhase = false;
		
		m_BlinkTimer = new Timer(CALL_CATEGORY_SYSTEM);
		m_BlinkTimer.Run(BLINK_INTERVAL, this, "UpdateBlinking", null, true);
	}
	
	// Stop blinking animation
	protected void StopBlinking()
	{
		if (m_BlinkTimer)
		{
			m_BlinkTimer.Stop();
			m_BlinkTimer = NULL;
		}
		
		m_IsBlinking = false;
		m_BlinkTime = 0;
		m_BlinkPhase = false;
	}
	
	// Update blinking animation - called by timer
	void UpdateBlinking()
	{
		if (!m_IsBlinking)
			return;
			
		m_BlinkTime += BLINK_INTERVAL;
		
		if (m_BlinkTime >= BLINK_CYCLE_TIME)
		{
			m_BlinkPhase = !m_BlinkPhase;
			m_BlinkTime = 0;
			
			ApplyBlinkState();
		}
	}
	
	// Apply the current blink state to the appropriate lights
	protected void ApplyBlinkState()
	{
		float visiblePhase, hiddenPhase;
		
		// Determine phases for blinking
		if (m_BlinkPhase)
		{
			visiblePhase = LIGHT_VISIBLE;
			hiddenPhase = LIGHT_HIDDEN;
		}
		else
		{
			visiblePhase = LIGHT_HIDDEN;
			hiddenPhase = LIGHT_HIDDEN;
		}
		
		switch(m_CurrentLightState)
		{
			case CodeLockLightState.SETTING_MODE:
			{
				// Blink green main light
				m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_GREEN_LIGHT, visiblePhase);
				
				// Blink green button light (green on, red off)
				SetButtonLights(visiblePhase, LIGHT_HIDDEN);
				break;
			}
			case CodeLockLightState.BRUTE_FORCE_PROTECTION:
			{
				// Blink red main light
				m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_RED_LIGHT, visiblePhase);
				
				// Blink red button light (green off, red on)
				SetButtonLights(LIGHT_HIDDEN, visiblePhase);
				break;
			}
			default:
				return; // No blinking for this state
		}
	}
	
	// Helper method to set button light to specific color (non-blinking)
	void SetButtonLightColor(bool isRed)
	{
		if (isRed)
		{
			SetButtonLights(LIGHT_HIDDEN, LIGHT_VISIBLE); // Green off, Red on
		}
		else
		{
			SetButtonLights(LIGHT_VISIBLE, LIGHT_HIDDEN); // Green on, Red off
		}
	}
	
	// Helper method to turn off both button lights
	void SetButtonLightsOff()
	{
		SetButtonLights(LIGHT_HIDDEN, LIGHT_HIDDEN);
	}
		
	// Update physical animations (position, rotation, etc.)
	protected void UpdateAnimations()
	{
		if (m_CodeLock.HasValidParent())
		{
			EntityAI hierarchyParent = m_CodeLock.GetHierarchyRoot();
			UpdateAttachedAnimations(hierarchyParent);
		}
		else
		{
			UpdateDetachedAnimations();
		}
	}
	
	// Update animations when attached to a structure
	protected void UpdateAttachedAnimations(EntityAI hierarchyParent)
	{
		// Handle rotation
		bool reversed = m_CodeLock.GetReversedState(); // Assuming this is accessible
		float rotationPhase;
		if (reversed)
			rotationPhase = 180.0;
		else
			rotationPhase = 0;
	
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_R, rotationPhase);
		
		// Handle position based on parent type
		m_LastAnimationPhaseFront = GetAnimationPhaseForFrontPart(hierarchyParent, reversed);
		m_LastAnimationPhaseBack = GetAnimationPhaseForBackPart(hierarchyParent, reversed);
				
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_FRONT, m_LastAnimationPhaseFront);
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_BACK, m_LastAnimationPhaseBack);
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_ZIP, 1.0);
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_BACK_R, 90.0);
	}
	
	protected float GetAnimationPhaseForFrontPart(EntityAI hierarchyParent, bool reversed)
	{
		float animationPhase = 0.0;
		if (hierarchyParent.IsInherited(House))
		{
			if (reversed)
			{
				animationPhase = 0.50;
			}
		}
		
		return animationPhase;
	}
	
	protected float GetAnimationPhaseForBackPart(EntityAI hierarchyParent, bool reversed)
	{
		float animationPhase = 0;
		if (hierarchyParent.IsInherited(House))
		{
			if (reversed)
			{
				animationPhase = 0.50;
			}
			else
			{
				animationPhase = 0.60;
			}
		}
		else if (hierarchyParent.IsInherited(BaseBuildingBase))
		{
			animationPhase = 0.92;
		}
		
		return animationPhase;
	}
	
	// Update animations when detached
	protected void UpdateDetachedAnimations()
	{
		m_LastAnimationPhaseFront = 0;
		m_LastAnimationPhaseBack = 0;
		
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_FRONT, 0);
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_BACK, 0);
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_ZIP, 0);
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_R, 0);
		m_CodeLock.SetAnimationPhase(CODELOCK_ANIMATED_BACK_R, 0);
	}
	
	// Update battery level indicator
	protected void UpdateBatteryIndicator()
	{
		float animationPhase;
		float energyPercent = m_CodeLock.GetBatteryEnergy();
		int visibleBars = CalculateVisibleBars(energyPercent);
		
		if (visibleBars > 0)
			animationPhase = 0;
		else
			animationPhase = 1.0;
		
		SetBatteryLight(animationPhase);
		
		for (int i = 0; i < BATTERY_BAR_SELECTIONS.Count(); ++i)
		{
			if (i < visibleBars)
				animationPhase = 0; // visible
			else
				animationPhase = 1.0; // hidden
			
			m_CodeLock.SetAnimationPhase(BATTERY_BAR_SELECTIONS[i], animationPhase);
		}
		
		m_LastVisibleBars = visibleBars;
		m_LastBatteryPercent = energyPercent;
	}
		
	// Calculate how many battery bars should be visible
	protected int CalculateVisibleBars(float batteryPercent)
	{
		if (!m_CodeLock.HasValidParent())
			return 0;
		
		if (batteryPercent <= 0)
			return 0;
			
		const int maxBars = 6;
		float barsFloat = Math.Ceil((batteryPercent / 100.0) * maxBars);
		return Math.Max(0, Math.Min(maxBars, barsFloat));
	}
	
	// Get current light state (for debugging/external access)
	CodeLockLightState GetCurrentLightState()
	{
		return m_CurrentLightState;
	}
	
	// Check if currently blinking
	bool IsBlinking()
	{
		return m_IsBlinking;
	}
}