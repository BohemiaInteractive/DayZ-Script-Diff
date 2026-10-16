//! Manages eye squinting
class GameplayEffectsMetaDataSquinting : GameplayEffectsData
{
	protected const float TOP_LID_OPEN_Y_POSITION 		= -512; //! [px]
	protected const float TOP_LID_CLOSED_Y_POSITION 	= 0; 	//! [px]
	protected const float BOTTOM_LID_OPEN_Y_POSITION 	= -512; //! [px]
	protected const float BOTTOM_LID_CLOSED_Y_POSITION 	= 0; 	//! [px]

	protected bool m_Initialized;
	protected bool m_Visible; 			//!overall visibility
	protected Widget m_EyeLidFrame;
	protected ImageWidget m_TopLid;
	protected ImageWidget m_BottomLid;
	
	// Passed from modifier -> symptom -> effect
	protected float m_SquintAmount;             // How much to close the eye <0.0, 1.0>
	
	private ref PPERequester_SquintEffects m_RequesterSquint;
	
	// Squint parameter:
	float m_SquintCloseSpeed = 0.5; // seconds
	
	protected float m_SquintTargetAmount = 0.0;
	protected float m_SquintCurrentAmount = 0.0;
	protected bool m_IsSquinting;
	
	protected static float m_SmoothVelArray[1];
	
	void GameplayEffectsMetaDataSquinting(array<ref Widget> input, int type, int user_override = -1)
	{
		m_RequesterSquint = PPERequester_SquintEffects.Cast(PPERequesterBank.GetRequester(PPERequesterBank.REQ_SQUINT));
	}
	
	override void Init(array<ref Widget> input, int type, Widget layout_root, int user_override = -1)
	{
		super.Init(input, type, layout_root, user_override);
		
		m_EyeLidFrame = m_LayoutRoot.FindAnyWidgetById(EffectWidgetsTypes.SQUINTING);
		m_EyeLidFrame.Show(m_Visible);
		m_TopLid = ImageWidget.Cast(m_EyeLidFrame.FindWidget("topLid"));
		m_TopLid.Show(true);
		m_BottomLid = ImageWidget.Cast(m_EyeLidFrame.FindWidget("bottomLid"));
		m_BottomLid.Show(true);
		
		m_Initialized = true;
		m_Visible = true;
	}
	
	override bool HasDefinedHandle()
	{
		return true;
	}
	
	override bool DataInitialized()
	{
		return m_Initialized;
	}
	
	override void Update(float timeSlice = 0, Param p = null, int handle = -1)
	{
		if (p)
		{
			Param1<float> params;
			if (Class.CastTo(params, p))
			{
				m_SquintAmount 			= params.param1;
				m_SquintTargetAmount 	= Math.SmoothCD(m_SquintTargetAmount, m_SquintAmount + Math.RandomFloatInclusive(-0.39, 0.39), m_SmoothVelArray, 0.44, 10000, timeSlice);
				m_SquintTargetAmount 	= Math.Clamp(m_SquintTargetAmount, 0.0, 1.0);
				m_IsSquinting 			= m_SquintCurrentAmount > 0.0 || m_SquintTargetAmount > 0.0;
	            UpdateSquint(timeSlice);
			}
		}
	}
	
	protected void UpdateSquint(float dt)
	{
	    if (!m_IsSquinting)
		{
			m_SmoothVelArray[0] = 0.0;
	        return;
		}
	
	    float speed = m_SquintCloseSpeed;
	    m_SquintCurrentAmount = Math.Lerp(m_SquintCurrentAmount, m_SquintTargetAmount, dt / speed);
	
	    SetEyelidAmount(m_SquintCurrentAmount, dt);
	
	    if (Math.AbsFloat(m_SquintCurrentAmount - m_SquintTargetAmount) < 0.01)
	    {
	        m_SquintCurrentAmount = m_SquintTargetAmount;
	        m_IsSquinting = false;
			m_SmoothVelArray[0] = 0.0;
	    }
	}

	protected void SetEyelidAmount(float amount, float timeslice)
	{
	    if (m_TopLid)
	        m_TopLid.SetPos(0, Math.Lerp(TOP_LID_OPEN_Y_POSITION, TOP_LID_CLOSED_Y_POSITION, amount));
	    
	    if (m_BottomLid)
	        m_BottomLid.SetPos(0, Math.Lerp(BOTTOM_LID_OPEN_Y_POSITION, BOTTOM_LID_CLOSED_Y_POSITION, amount));
	}
	
	override void UpdateVisibility(bool state)
	{
		Widget w = m_EyeLidFrame;
		while (w && w != m_LayoutRoot && m_Visible == state && w.IsVisibleHierarchy() != state)
		{
			w.Show(state);
			w = w.GetParent();
		}
	}
	
	//! stops and re-sets indicators and images even out of sequence. Should still be tied to the 'player' update, though!
	override void ForceStop()
	{
		super.ForceStop();
		
		Update();
	}
}
