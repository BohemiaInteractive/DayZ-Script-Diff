class DayZIntroScenePC : DayZIntroScene
{
	protected bool		m_EnableClick;
	protected bool 		m_RotatingCharacter;
	protected int 		m_RotatingCharacterMouseX;
	protected int 		m_RotatingCharacterMouseY;
	protected float 	m_RotatingCharacterRot;
	protected float 	m_Radius;
	protected float 	m_Radius_original;
	protected float 	m_DiffX;
	protected float 	m_DeltaX;
	protected float 	m_DeltaZ;
	protected float 	m_Angle;
	protected float 	m_Angle_offset = 0;
	protected float 	m_NewX = 0;
	protected float 	m_NewZ = 0;
	protected float 	m_BlurValue;

	protected ref OptionsMenu m_optmenu;

	// ------------------------------------------------------------
	void DayZIntroScenePC()
	{
		SetClickEnable( true );

		m_DeltaX = Math.AbsFloat(m_CharacterPos[0] - m_Camera.GetPosition()[0]);
		m_DeltaZ = Math.AbsFloat(m_CharacterPos[2] - m_Camera.GetPosition()[2]);
		
		if (!m_Radius || m_Radius == 0)
		{
			m_Radius = Math.Sqrt (Math.Pow(m_DeltaX, 2) + Math.Pow(m_DeltaZ, 2));
			m_Radius_original = m_Radius;
		}
	}
	
	protected override void SetInitPostprocesses()
	{
		super.SetInitPostprocesses();
		
		PPERequester_MenuEffects requester;
		Class.CastTo(requester,PPERequesterBank.GetRequester(PPERequester_MenuEffects));
		
		requester.SetVignetteIntensity(0.5);
	}

	// ------------------------------------------------------------
	void SetClickEnable( bool enable )
	{
		m_EnableClick = enable;
	}
	
	// ------------------------------------------------------------
	bool IsClickEnabled()
	{
		return m_EnableClick;
	}
	
	// ------------------------------------------------------------
	void CharacterRotationStart()
	{
		m_RotatingCharacter = true;
		g_Game.GetMousePos(m_RotatingCharacterMouseX, m_RotatingCharacterMouseY);
		
		if (m_Character) 
		{
			m_RotatingCharacterRot = m_CharacterRot[0];
		}
	}
	
	// ------------------------------------------------------------
	void CharacterRotationStop()
	{
		if (m_RotatingCharacter)
		{
			m_RotatingCharacter = false;
		}
	}
	
	// ------------------------------------------------------------
	bool IsRotatingCharacter()
	{
		return m_RotatingCharacter;
	}
	
	// ------------------------------------------------------------
	void CharacterRotate()
	{
		if (m_Character && m_Character.GetCharacterObj())
		{
			int actual_mouse_x;
			int actual_mouse_y;
			float coef;
			g_Game.GetMousePos(actual_mouse_x, actual_mouse_y);
		
			m_DiffX = m_RotatingCharacterMouseX - actual_mouse_x;
			
			coef = ( m_RotatingCharacterRot + (m_DiffX * 0.5) ) / 360;
			coef = coef - Math.Floor(coef);
			m_CharacterRot[0] = coef * 360;
			
			m_Character.GetCharacterObj().SetOrientation(m_CharacterRot);
		}
	}
	
	// ------------------------------------------------------------
	void Update()
	{
		if (m_Character && m_RotatingCharacter)
		{
			CharacterRotate();
		}
	}		
};
