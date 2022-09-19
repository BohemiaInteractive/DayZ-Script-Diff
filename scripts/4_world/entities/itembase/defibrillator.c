class Defibrillator extends ItemBase
{
	static const string	CHARGING_SOUND = 			"defibrillator_charge_SoundSet";
	static const string	CHARGED_AND_READY_SOUND = 	"defibrillator_ready_SoundSet";
	static const string	SHOCK_SOUND = 				"defibrillator_shock_SoundSet";
	
	bool m_IsCharged = false;
	
	static float m_ChargeTime = 5;
	static float m_EnergyNeededToCharge = 20; 
	
	ref Timer m_ChargingTimer;
	EffectSound m_ChargedAlarm;
	EffectSound m_ChargingSound;
	
	void Defibrillator()
	{
		// Read all config parameters
		//m_ChargeTime = GetTimeNeededToCharge();
		m_EnergyNeededToCharge = GetEnergyNeededToCharge();
	}
	
	void ~Defibrillator()
	{
		SEffectManager.DestroyEffect(m_ChargedAlarm);
		SEffectManager.DestroyEffect(m_ChargingSound);		
	}
	
	float GetTimeNeededToCharge()
	{
		string cfg_path = "CfgVehicles " + GetType() + " ";
		return GetGame().ConfigGetFloat (cfg_path + "defibChargeTime");
	}
	
	float GetEnergyNeededToCharge()
	{
		string cfg_path = "CfgVehicles " + GetType() + " ";
		return GetGame().ConfigGetFloat (cfg_path + "defibEnergyNeededPerCharge");
	}
	
	override void OnWorkStart()
	{
		if ( !GetGame().IsDedicatedServer() )
			m_ChargingSound = SEffectManager.PlaySoundOnObject(CHARGING_SOUND, this, 0, 0.15);
		
		float energy_needed = m_EnergyNeededToCharge / m_ChargeTime;
		GetCompEM().SetEnergyUsage(energy_needed);
		
		if (!m_ChargingTimer)
			m_ChargingTimer = new Timer( CALL_CATEGORY_GAMEPLAY );
		
		if ( !m_ChargingTimer.IsRunning() )
		{
			m_ChargingTimer.Run(m_ChargeTime, this, "OnIsCharged", NULL, false);
		}
	}
	
	override void OnWorkStop()
	{
		GetCompEM().SwitchOff();
		GetCompEM().ResetEnergyUsage();
		StopChargingTimer();
		StopChargedAlarm();
		StopChargingSound();
		
		m_IsCharged = false;
	}
	
	void OnIsCharged()
	{
		if ( GetCompEM().IsWorking() )
		{
			if (!GetGame().IsMultiplayer()  ||  GetGame().IsClient())
			{
				//m_ChargedAlarm = PlaySoundLoop(CHARGED_AND_READY_SOUND, 40);
				m_ChargedAlarm = SEffectManager.PlaySoundOnObject(CHARGED_AND_READY_SOUND, this);
				m_ChargingSound.SoundStop();
			}
			GetCompEM().ResetEnergyUsage();
			m_IsCharged = true;
		}
	}
	
	void StopChargedAlarm()
	{
		if (m_ChargedAlarm)
		{
			//GetGame().ObjectDelete(m_ChargedAlarm);
			m_ChargedAlarm.SoundStop();
			m_ChargedAlarm = NULL;
		}
	}	
	
	void StopChargingSound()
	{
		if(m_ChargingSound)
		{
			m_ChargingSound.SoundStop();
		}
	}
	
	void StopChargingTimer()
	{
		if (m_ChargingTimer)
		{
			m_ChargingTimer.Stop();
			m_ChargingTimer = NULL;
		}
	}
	
	bool IsCharged()
	{
		return m_IsCharged;
	}
	
	void DischargeServer(PlayerBase victim)
	{
		/*
		bool has_heart_attack = victim.m_ModifiersManager.IsModifierActive(eModifiers.MDF_HEART_ATTACK);
		
		if ( has_heart_attack )
		{
			victim.m_ModifiersManager.DeactivateModifier ( eModifiers.MDF_HEART_ATTACK );
		}
		else
		{
			victim.m_ModifiersManager.ActivateModifier ( eModifiers.MDF_HEART_ATTACK );
		}
		*/
		/*
		if (!GetGame().IsMultiplayer()  ||  GetGame().IsClient())
		{
			SEffectManager.PlaySoundOnObject(SHOCK_SOUND, this);
		}*/
		
		victim.SetPulseType(!victim.GetPulseType());
		victim.SetHealth("","Shock",0);

		GetCompEM().SwitchOff();
	}
	
	void DischargeClient(PlayerBase victim)
	{
		SEffectManager.PlaySoundOnObject(SHOCK_SOUND, this);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
		AddAction(ActionDefibrilateTarget);
		AddAction(ActionDefibrilateSelf);
	}
}