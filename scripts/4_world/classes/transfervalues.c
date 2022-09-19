class TransferValues extends Managed
{

	const int TYPE_HEALTH 	= 1;
	const int TYPE_BLOOD 	= 2;
	
	const float VALUE_CHECK_INTERVAL	 	= 5;
	const float SENSITIVTY_PERCENTAGE 		= 1;//how much the value needs to change up/down from previous update to trigger a new update(in percent)

	const int BLOOD_THRESHOLD_LOW = 3000;
	PlayerBase m_Player;
	float m_TimeSinceLastTick = VALUE_CHECK_INTERVAL + 1;
	
	float m_LastHealthUpdate;
	float m_LastBloodUpdate;
	
	//float m_CumulatedHealthDiff;
	//float m_CumulatedBloodDiff;

	float m_HealthMaxValue;
	float m_BloodMaxValue;
	
	float m_BloodClient;
	float m_HealthClient;
	
	protected bool m_InitialSyncSent;
	
	void TransferValues(PlayerBase player)
	{
		m_Player = player;
		m_InitialSyncSent = false;
		Init();
	}
	
	void Init()
	{
		m_LastHealthUpdate 	= 0;
		m_LastBloodUpdate 	= 0;
		m_HealthMaxValue 	= m_Player.GetMaxHealth("", "Health");
		m_BloodMaxValue 	= m_Player.GetMaxHealth("", "Blood");
		m_BloodClient 		= 0;
		m_HealthClient 		= 0;
	}
	
	void OnScheduledTick(float deltatime)
	{
		#ifdef DEVELOPER
		if ( DiagMenu.GetBool(DiagMenuIDs.DM_TRANSFER_VALUES_SHOW) )
		{
			if(!GetGame().IsDedicatedServer())
				ShowDebugValues(true);
		}
		else
		{
			if(!GetGame().IsDedicatedServer())
				ShowDebugValues(false);
		}
		#endif
		
		if( GetGame().IsClient() ) return;
		
		m_TimeSinceLastTick += deltatime;
		
		if (!m_InitialSyncSent)
		{
			SendInitValues();
		}
		else if( m_TimeSinceLastTick > VALUE_CHECK_INTERVAL )
		{
			/*
			Print(m_TimeSinceLastTick.ToString());
			Print(VALUE_CHECK_INTERVAL.ToString());
			Print("--------------");
			*/
			m_TimeSinceLastTick = 0;
			CheckValues();
			
			// send sync junctures if necessary
			// TODO: !!!! event is sent too often, please fix it
			/*
			float damage = 1 - m_Player.GetHealth("", "") / 100;
			DayZPlayerSyncJunctures.SendInjury(m_Player, true, damage);
			*/
		}
	}
	
	void CheckValues()
	{
		CheckHealth();
		CheckBlood();
	}
	
	float GetBlood()
	{
		return m_BloodClient;
		
	}
	
	float GetHealth()
	{
		return m_HealthClient;
	}
	
	void CheckHealth()
	{
		float health_current = m_Player.GetHealth("","Health");
		float health_normalized = health_current / m_HealthMaxValue;
		float difference_normalized = health_normalized - m_LastHealthUpdate;
		float diff_abs = Math.AbsFloat(difference_normalized);
		
		if( diff_abs > ( SENSITIVTY_PERCENTAGE /100 ) )
		{
			SendValue(TYPE_HEALTH, health_normalized);
			m_LastHealthUpdate = health_normalized;
		}
	}
	
	/*void CheckBlood()
	{
		float blood_current = m_Player.GetHealth("","Blood");
		//float blood_normalized = blood_current / m_BloodMaxValue;
		float blood_normalized = Math.InverseLerp(BLOOD_THRESHOLD_LOW, m_BloodMaxValue, blood_current);
		blood_normalized = Math.Clamp(blood_normalized,0,1);
		float difference_normalized = blood_normalized - m_LastBloodUpdate;
		float diff_abs = Math.AbsFloat(difference_normalized);
		m_CumulatedBloodDiff += diff_abs;
		
		//Print("DbgBlood | CheckBlood | m_CumulatedBloodDiff: " + m_CumulatedBloodDiff);
		if( m_CumulatedBloodDiff > ( SENSITIVTY_PERCENTAGE /100 ) )
		{
			SendValue(TYPE_BLOOD, blood_normalized);
			m_LastBloodUpdate = blood_normalized;
			//Print("DbgBlood | CheckBlood | Sending blood: " + blood_normalized);
			m_CumulatedBloodDiff = 0;
		}
		else
		{
			//Print("DbgBlood | CheckBlood | unsent values blood: " + blood_normalized);
		}
	}*/
	
	void CheckBlood()
	{
		float blood_current = m_Player.GetHealth("","Blood");
		//float blood_normalized = blood_current / m_BloodMaxValue;
		float blood_normalized = Math.InverseLerp(BLOOD_THRESHOLD_LOW, m_BloodMaxValue, blood_current);
		blood_normalized = Math.Clamp(blood_normalized,0,1);
		float difference_normalized = blood_normalized - m_LastBloodUpdate;
		float diff_abs = Math.AbsFloat(difference_normalized);
		
		if( diff_abs > ( SENSITIVTY_PERCENTAGE /100 ) )
		{
			SendValue(TYPE_BLOOD, blood_normalized);
			m_LastBloodUpdate = blood_normalized;
		}
	}
	
	//! Sends values on object creation
	void SendInitValues()
	{
		m_InitialSyncSent = true;
		
		//HP
		float health_current = m_Player.GetHealth("","Health");
		float health_normalized = health_current / m_HealthMaxValue;
		SendValue(TYPE_HEALTH, health_normalized);
		m_LastHealthUpdate = health_normalized;
		
		//Blood
		float blood_current = m_Player.GetHealth("","Blood");
		float blood_normalized = Math.InverseLerp(BLOOD_THRESHOLD_LOW, m_BloodMaxValue, blood_current);
		blood_normalized = Math.Clamp(blood_normalized,0,1);
		//Print("DbgBlood | SendInitValues | blood_normalized: " + blood_normalized);
		SendValue(TYPE_BLOOD, blood_normalized);
		m_LastBloodUpdate = blood_normalized;
	}
	
	void SendValue(int value_type, float value)
	{
		CachedObjectsParams.PARAM2_INT_FLOAT.param1 = value_type;
		CachedObjectsParams.PARAM2_INT_FLOAT.param2 = value;
		
		GetGame().RPCSingleParam(m_Player, ERPCs.RPC_DAMAGE_VALUE_SYNC, CachedObjectsParams.PARAM2_INT_FLOAT, true, m_Player.GetIdentity());
	}

	void ReceiveValue(int value_type, float value)
	{
		if( value_type == TYPE_HEALTH )
		{
			m_HealthClient = value;
		}
		else if( value_type == TYPE_BLOOD )
		{
			m_BloodClient = value;
		}
	}

	void OnRPC(ParamsReadContext ctx)
	{
		ctx.Read(CachedObjectsParams.PARAM2_INT_FLOAT);
		
		int value_type 	= CachedObjectsParams.PARAM2_INT_FLOAT.param1;
		float value 	= CachedObjectsParams.PARAM2_INT_FLOAT.param2;
		
		ReceiveValue(value_type, value);
	}
	
	void ShowDebugValues(bool show)
	{
		#ifdef DEVELOPER
		if( show )
		{
			
			DbgUI.BeginCleanupScope();     
	        DbgUI.Begin("Values", 50, 50);
	        DbgUI.Text("Blood: " + m_BloodClient.ToString());
			DbgUI.Text("Health: " + m_HealthClient.ToString());

	        DbgUI.End();
	        DbgUI.EndCleanupScope();
		}
		else
		{
			DbgUI.BeginCleanupScope();
			DbgUI.Begin("Values", 50, 50);
			DbgUI.End();
	        DbgUI.EndCleanupScope();   
		}
		#endif
	}
	
}
