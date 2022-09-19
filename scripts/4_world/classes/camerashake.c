class CameraShake
{
	const float MIN_PLAYER_DISTANCE = 40;
	
	float m_Radius;
	float m_RadiusDecaySpeed;
	float m_RandomAngle;
	float m_Time;
	float m_InitLR;
	float m_InitUD;
	bool m_ToDelete;
	float m_Smoothness;
	float m_StregthFactor;
	DayZPlayerImplement m_Player;
	
	void ~CameraShake()
	{
		if(m_Player)
			m_Player.GetAimingModel().SetCamShakeValues(0, 0);

	}
	
	void CameraShake( float strength_factor, float radius, float smoothness, float radius_decay_speed )
	{
		/*
		
		Print("-----------ON CREATE------------");
		Print(camera_offset);
		Print("-----------ON CREATE END------------");
		*/
		//m_Player = DayZPlayerImplement.Cast(player);
		m_Player = DayZPlayerImplement.Cast(GetGame().GetPlayer());
		m_StregthFactor = strength_factor;
		//m_InitLR = lr_angle;
		//m_InitUD = ud_angle;
		m_Radius = radius;
		m_RadiusDecaySpeed = radius_decay_speed;
		m_Smoothness = smoothness;
	}
	
	void Update(float delta_time, out float x_axis, out float y_axis)
	{
		if(m_ToDelete)
			delete this;
		
		m_Radius -= delta_time * m_RadiusDecaySpeed; //diminish radius each frame
		
		if( m_RandomAngle >= 0 )
		{
			m_RandomAngle = -m_Radius + (Math.RandomFloat( -m_Radius / m_Smoothness, m_Radius / m_Smoothness));
		}
		else
		{
			m_RandomAngle = m_Radius + (Math.RandomFloat( -m_Radius / m_Smoothness, m_Radius / m_Smoothness));
		}
		
		x_axis = m_RandomAngle * m_StregthFactor;
		y_axis = m_RandomAngle * m_StregthFactor;

		//Print(x_axis);
		//Print(y_axis);
		
		if( m_Radius < 0.01 )
		{
			m_ToDelete = true;
		}
	}
}

