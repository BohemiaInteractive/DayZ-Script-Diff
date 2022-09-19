/* 
DayZPlayerImplement

this file is implemenation of dayzPlayer in script 
- logic of movement 
- camera switching logic

*/

enum eSwayStates
{
	DEFAULT,
	HOLDBREATH_IN,
	HOLDBREATH_STABLE,
	HOLDBREATH_EXHAUSTED,
	MAX
}

class PlayerSwayConstants
{
	static const float SWAY_MULTIPLIER_DEFAULT = 1.0;
	static const float SWAY_MULTIPLIER_STABLE = 0.05;
	static const float SWAY_MULTIPLIER_EXHAUSTED = 0.6;
	static const float SWAY_TIME_IN = 1.5;
	static const float SWAY_TIME_STABLE = 3.0;
	static const float SWAY_TIME_EXHAUSTED = 1.5;
	static const float SWAY_TIME_OUT = 0.5;
	static const float SWAY_ROLL = 3;
}

class DayZPlayerImplementAiming
{

	//-------------------------------------------------------------
	//!
	//! This HeadingModel - Clamps heading
	//!
	
	protected const float SWAY_WEIGHT_SCALER = 1;//use this to scale the sway effect up/down
	protected float m_HorizontalNoise;
	protected float m_HorizontalTargetValue;
	protected float m_HorizontalNoiseVelocity[1] = {0};
	protected DayZPlayerImplement	m_PlayerDpi;
	protected PlayerBase m_PlayerPb;
	protected float m_TotalTime;
	protected float m_ReferenceTime = 0;
	protected float m_SwayStateStartTime;
	//protected float m_SwayStateStartTime[eSwayStates.MAX];
	protected float m_LastSwayMultiplier = PlayerSwayConstants.SWAY_MULTIPLIER_DEFAULT;
	protected float m_StateStartSwayMultiplier;
	protected float m_HorizontalNoiseXAxisOffset;
	protected float m_BreathingXAxisOffset;
	protected float m_BreathingYAxisOffset;
	protected bool m_HoldingBreathSet;
	protected bool	m_AimNoiseAllowed = true;	
	protected bool	m_ProceduralRecoilEnabled = true;	
	protected ref RecoilBase m_CurrentRecoil;
	protected int m_ShakeCount;
	protected float m_SwayWeight;
	protected float m_MaxVelocity;
	protected ref KuruShake m_KuruShake;
	protected float m_CamShakeX;
	protected float m_CamShakeY;
	protected vector m_SwayModifier = "1 1 1";//"max_speed_noise radius_noise overall_speed"
	protected int m_SwayState = -1;
	
	protected float m_StealthAimY_Last;
	protected float m_FilterVelocityStealthAimY[1] = {0};
	
	protected static float	m_AimXClampRanges[] = { -180, -20, 90, 	0, -50, 90,  180, -20, 90 };
	
	void DayZPlayerImplementAiming(DayZPlayerImplement player)
	{
		m_PlayerDpi = player;
		Class.CastTo(m_PlayerPb, player);
		UpdateSwayState(eSwayStates.DEFAULT);
	}

	void SetRecoil( Weapon_Base weapon )
	{
		if( m_ProceduralRecoilEnabled )
		{
			m_CurrentRecoil = weapon.SpawnRecoilObject();
		}
	}
	
	void RequestKuruShake(float amount)
	{
		if(!m_KuruShake)
			m_KuruShake = new KuruShake(m_PlayerPb, amount);
	}
	
	void OnRaiseBegin(DayZPlayerImplement player)
	{
		Weapon_Base weapon = Weapon_Base.Cast(player.GetHumanInventory().GetEntityInHands());
		if(weapon)
		{
			m_SwayModifier = weapon.GetPropertyModifierObject().m_SwayModifiers;
		}
	}
	
	void OnFinisherBegin(float currentAimY)
	{
		m_StealthAimY_Last = currentAimY;
		m_FilterVelocityStealthAimY[0] = 0;
	}
	
	void OnSwayStateChange(int state)
	{
		switch (state)
		{
			case eSwayStates.HOLDBREATH_EXHAUSTED:
				m_PlayerPb.OnHoldBreathExhausted();
			break;
			
			default:
			break;
		}
	}
	
	float GetSwayWeight()
	{
		return m_SwayWeight;		
	}
	
	void SetAimNoiseAllowed(bool state)
	{
		m_AimNoiseAllowed = state;	
	}

	void SetProceduralRecoilEnabled(bool state)
	{
		m_ProceduralRecoilEnabled = state;
	}
	
	void SetCamShakeValues(float x_axis, float y_axis)
	{
		m_CamShakeX = x_axis;
		m_CamShakeY = y_axis;
	}

	bool ProcessStealthFilters(float pDt, SDayZPlayerAimingModel pModel)
	{		
		m_StealthAimY_Last = Math.SmoothCD(m_StealthAimY_Last, 0, m_FilterVelocityStealthAimY, 0.3, 1000, pDt);
		pModel.m_fAimYMouseShift = -(pModel.m_fCurrentAimY - m_StealthAimY_Last);
		return true;
	}
	
	bool ProcessAimFilters(float pDt, SDayZPlayerAimingModel pModel, int stance_index)
	{
		float breathing_offset_x;
		float breathing_offset_y;
		
		float noise_offset_x;
		float noise_offset_y;		
		
		float shake_offset_x;
		float shake_offset_y;
		
		float recoil_offset_mouse_x;
		float recoil_offset_mouse_y;
		
		float recoil_offset_hands_x;
		float recoil_offset_hands_y;
		
		float kuru_offset_x;
		float kuru_offset_y;
		
		//float player_stamina = m_PlayerPb.GetStaminaHandler().GetStaminaNormalized();
		float player_stamina = m_PlayerPb.GetStaminaHandler().GetSyncedStaminaNormalized();
		
		#ifdef DEVELOPER
		DbgPrintAimingImplement("Player: " + m_PlayerPb + " | ProcessAimFilters | timestamp: " + m_PlayerPb.GetSimulationTimeStamp());
		#endif
		
		//negates stamina effect during hold breath
		if ( m_PlayerPb.IsHoldingBreath() )
		{
			player_stamina = 1;
		}
		float speed = (((1.0 - player_stamina) * 3.0) + 1.0) * m_SwayModifier[2];
		m_TotalTime += pDt * speed;
		
		if (m_PlayerPb.IsHoldingBreath() && !m_HoldingBreathSet)
		{
			m_ReferenceTime = m_TotalTime;
		}
		else if (!m_PlayerPb.IsHoldingBreath() && m_HoldingBreathSet)
		{
			m_ReferenceTime = m_TotalTime;
		}
		
		float adjusted_sway_multiplier = CalculateSwayMultiplier();
		m_LastSwayMultiplier = adjusted_sway_multiplier;
		
		m_SwayWeight = CalculateWeight(	stance_index, player_stamina, 0.5/*m_PlayerPb.m_CameraSwayModifier*/, m_PlayerPb.IsHoldingBreath() ) * adjusted_sway_multiplier;

		//! get sway
		ApplyBreathingPattern(breathing_offset_x, breathing_offset_y, 3.0, m_TotalTime, m_SwayWeight);
		ApplyHorizontalNoise(noise_offset_x, noise_offset_y, 0.2, 0.5, 3.0 * m_SwayModifier[0], speed, 3 * m_SwayModifier[1], m_SwayWeight, pDt);
		
		int shake_level = m_PlayerPb.GetShakeLevel();
		if (shake_level != 0)
		{
			ApplyShakes(shake_offset_x, shake_offset_y, shake_level);
		}

		//! get recoil
		if( m_CurrentRecoil )
		{
			m_CurrentRecoil.Update(pModel, recoil_offset_mouse_x, recoil_offset_mouse_y, recoil_offset_hands_x, recoil_offset_hands_y, pDt);
		}
		
		if( m_KuruShake )
		{
			m_KuruShake.Update(pDt, kuru_offset_x, kuru_offset_y);
		}
		
		//! hands offset
		pModel.m_fAimXHandsOffset = breathing_offset_x + noise_offset_x + recoil_offset_hands_x + shake_offset_x + kuru_offset_x;
		pModel.m_fAimYHandsOffset = breathing_offset_y + noise_offset_y + recoil_offset_hands_y + shake_offset_y + kuru_offset_y;
		
		#ifdef DEVELOPER
		DbgPrintAimingImplement("breathing_offset_y: " + breathing_offset_y);
		DbgPrintAimingImplement("noise_offset_y: " + noise_offset_y);
		DbgPrintAimingImplement("recoil_offset_hands_y: " + recoil_offset_hands_y);
		DbgPrintAimingImplement("shake_offset_y: " + shake_offset_y);
		DbgPrintAimingImplement("kuru_offset_y: " + kuru_offset_y);
		DbgPrintAimingImplement("pModel.m_fAimYHandsOffset: " + pModel.m_fAimYHandsOffset);
		#endif
		//! cam offset
		pModel.m_fAimXCamOffset = -shake_offset_x - recoil_offset_hands_x - kuru_offset_x + m_CamShakeX;
		pModel.m_fAimYCamOffset	= -shake_offset_y - recoil_offset_hands_y - kuru_offset_y + m_CamShakeY;
		
		
		#ifdef DEVELOPER
		DbgPrintAimingImplement("m_CamShakeY: " + m_CamShakeY);
		DbgPrintAimingImplement("pModel.m_fAimYCamOffset: " + pModel.m_fAimYCamOffset);
		#endif
		
		//! clamp aim ranges 
		if (stance_index == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
		{			
			float newVal = DayZPlayerUtils.LinearRangeClamp(pModel.m_fCurrentAimX, pModel.m_fCurrentAimY, m_AimXClampRanges);
			pModel.m_fAimYHandsOffset += newVal - pModel.m_fCurrentAimY;
		}
		
		if( m_PlayerDpi.IsInOptics() && m_KuruShake )
		{
			//TODO - do not offset mouse
		}
		//! mouse offset
		pModel.m_fAimXMouseShift = recoil_offset_mouse_x -kuru_offset_x / 10;
		pModel.m_fAimYMouseShift = recoil_offset_mouse_y + kuru_offset_y / 10;

		#ifdef DEVELOPER
		DbgPrintAimingImplement("recoil_offset_mouse_y: " + recoil_offset_mouse_y);
		DbgPrintAimingImplement("pModel.m_fAimYMouseShift: " + pModel.m_fAimYMouseShift);
		#endif
		
		if ( m_PlayerPb.IsHoldingBreath() && !m_HoldingBreathSet)
		{
			m_HoldingBreathSet = true;
			m_HorizontalNoiseXAxisOffset = noise_offset_x;
			m_BreathingXAxisOffset = breathing_offset_x;
			m_BreathingYAxisOffset = breathing_offset_y;
		}
		else if(!m_PlayerPb.IsHoldingBreath() && m_HoldingBreathSet)
		{
			m_HoldingBreathSet = false;
		}
		
		if(!m_PlayerPb.IsHoldingBreath() && m_LastSwayMultiplier == PlayerSwayConstants.SWAY_MULTIPLIER_DEFAULT && m_HorizontalNoiseXAxisOffset != 0)
		{
			m_HorizontalNoiseXAxisOffset = 0;
			m_BreathingXAxisOffset = 0;
			m_BreathingYAxisOffset = 0;
		}
		#ifdef DEVELOPER
		DbgPrintAimingImplement("----------------------------");
		#endif
		return true;
	}
	
	protected float CalculateSwayMultiplier()
	{
		float max;
		float time;
		float time_clamped;
		float ret;
		
		if (m_PlayerPb.IsHoldingBreath())
		{
			time = m_TotalTime - m_ReferenceTime;
			
			if (time < PlayerSwayConstants.SWAY_TIME_IN)
			{
				UpdateSwayState(eSwayStates.HOLDBREATH_IN);
				max = PlayerSwayConstants.SWAY_TIME_IN;
				time_clamped = Math.Clamp((m_TotalTime - m_SwayStateStartTime),0,max);
				ret = Math.Lerp(m_LastSwayMultiplier,PlayerSwayConstants.SWAY_MULTIPLIER_STABLE,time_clamped/max);
			}
			else if (time >= PlayerSwayConstants.SWAY_TIME_IN && time < (PlayerSwayConstants.SWAY_TIME_IN + PlayerSwayConstants.SWAY_TIME_STABLE))
			{
				UpdateSwayState(eSwayStates.HOLDBREATH_STABLE);
				ret = PlayerSwayConstants.SWAY_MULTIPLIER_STABLE;
			}
			else
			{
				UpdateSwayState(eSwayStates.HOLDBREATH_EXHAUSTED);
				max = PlayerSwayConstants.SWAY_TIME_EXHAUSTED;
				time_clamped = Math.Clamp((m_TotalTime - m_SwayStateStartTime),0,max);
				ret = Math.Lerp(PlayerSwayConstants.SWAY_MULTIPLIER_STABLE,PlayerSwayConstants.SWAY_MULTIPLIER_EXHAUSTED,(time_clamped/max));
			}
		}
		else
		{
			UpdateSwayState(eSwayStates.DEFAULT);
			max = PlayerSwayConstants.SWAY_TIME_OUT;
			time_clamped = Math.Clamp((m_TotalTime - m_SwayStateStartTime),0,max);
			ret = Math.Lerp(m_LastSwayMultiplier,1,time_clamped/max);
		}
		return ret;
	}
	
	protected bool UpdateSwayState(int state)
	{
		if (state != m_SwayState)
		{
			m_SwayState = state;
			m_SwayStateStartTime = m_TotalTime;
			m_StateStartSwayMultiplier = m_LastSwayMultiplier;
			OnSwayStateChange(state);
			return true;
		}
		
		return false;
	}
	
	int GetCurrentSwayState()
	{
		return m_SwayState;
	}

	protected void ApplyBreathingPattern(out float x_axis, out float y_axis, float pAmplitude, float pTotalTime, float weight)
	{

		float multiplier = Math.Lerp(PlayerSwayConstants.SWAY_MULTIPLIER_DEFAULT,0,m_LastSwayMultiplier); //TODO revise
		#ifdef DEVELOPER
		DbgPrintAimingImplement("m_LastSwayMultiplier: " + m_LastSwayMultiplier);
		DbgPrintAimingImplement("pAmplitude: " + pAmplitude);
		DbgPrintAimingImplement("pTotalTime: " + pTotalTime);
		DbgPrintAimingImplement("weight: " + weight);
		DbgPrintAimingImplement("multiplier: " + multiplier);
		#endif
		
		x_axis = (Math.Sin(pTotalTime) * pAmplitude / 4) * weight;
		y_axis = (Math.Sin((pTotalTime) * 0.8 + 0.6 ) * pAmplitude) * weight;
		#ifdef DEVELOPER 
		DbgPrintAimingImplement("y_axis_midproduct: " + y_axis); 
		#endif
		x_axis += m_BreathingXAxisOffset * multiplier;
		y_axis += m_BreathingYAxisOffset * multiplier;
	}

	protected void ApplyHorizontalNoise(out float x_axis, out float y_axis, float smooth_time,float max_velocity_low, float max_velocity_high, float velocity_modifier,  float max_distance, float weight, float pDt)
	{
		if ( Math.AbsFloat(m_HorizontalTargetValue - m_HorizontalNoise) < 0.01)
		{
			//acquire new target
			m_MaxVelocity = m_PlayerPb.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAimingModel, max_velocity_low, max_velocity_high);
			
			m_HorizontalTargetValue = (Math.RandomFloat01() - 0.5) * 2 * max_distance;
			m_HorizontalNoiseVelocity[0] = 0;
		}

		m_HorizontalNoise = Math.SmoothCD( m_HorizontalNoise, m_HorizontalTargetValue, m_HorizontalNoiseVelocity, smooth_time, m_MaxVelocity * velocity_modifier, pDt);
		x_axis = m_HorizontalNoise * weight;
		float multiplier = Math.Lerp(PlayerSwayConstants.SWAY_MULTIPLIER_DEFAULT,0,m_LastSwayMultiplier); //TODO revise
		x_axis += m_HorizontalNoiseXAxisOffset * multiplier;
	}
	
	protected void ApplyShakes(out float x_axis, out float y_axis, int level)
	{
		float weight = level / PlayerBase.SHAKE_LEVEL_MAX;
		m_ShakeCount++;
		int shakes_threshold = Math.Round(m_PlayerPb.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAimingModel, 2, 4));
		if (m_ShakeCount > shakes_threshold)
		{
			m_ShakeCount = 0;

			float modifier = m_PlayerPb.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAimingModel, 0.45, 0.9);
			x_axis = modifier * weight * m_PlayerPb.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAimingModel, 0, 1);
			y_axis = modifier * weight * m_PlayerPb.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAimingModel, 0, 1);
		}
	}

	protected float CalculateWeight(int stance_index, float current_stamina, float camera_sway_modifier, bool holding_breath)
	{
		if (m_PlayerDpi.GetCommand_Move() && m_PlayerDpi.GetCommand_Move().IsInRoll())//when the player is rolling, set a constant and disregard everything else
		{
			return PlayerSwayConstants.SWAY_ROLL;
		}
		float stance_modifier;
		//float scale = SWAY_WEIGHT_SCALER;
		switch ( stance_index )
		{
			case DayZPlayerConstants.STANCEIDX_RAISEDERECT:
				stance_modifier = 0.5;
			break;
			case DayZPlayerConstants.STANCEIDX_RAISEDCROUCH:
				stance_modifier = 0.75;
			break;
			case DayZPlayerConstants.STANCEIDX_RAISEDPRONE:
				stance_modifier = 0.9;
			break;
			default:
				stance_modifier = 0.75;
				//Debug.LogError("stance mask out of definition");
			break;
		}
		
		#ifdef DEVELOPER
		DbgPrintAimingImplement("current_stamina: " + current_stamina);
		DbgPrintAimingImplement("camera_sway_modifier: " + camera_sway_modifier);
		DbgPrintAimingImplement("holding_breath: " + holding_breath);
		#endif
		
		return (1 - stance_modifier) * m_AimNoiseAllowed * camera_sway_modifier * SWAY_WEIGHT_SCALER;
	}
	
	void DbgPrintAimingImplement(string val)
	{
		#ifdef DEVELOPER
		if (GetDayZGame().IsAimLogEnabled())
			Print("DayZPlayerImplementAiming | " + val);
		#endif
	}
}

