//! Manages eye veins effects
class GameplayEffectsMetaDataEyeVeins : GameplayEffectsData
{
	protected const float TOP_PART_IRRITATION_MIN_Y_POSITION 		= -512; //! [px]
	protected const float TOP_PART_IRRITATION_MAX_Y_POSITION 		= 0; 	//! [px]
	protected const float BOTTOM_PART_IRRITATION_MIN_Y_POSITION 	= -512; //! [px]
	protected const float BOTTOM_PART_IRRITATION_MAX_Y_POSITION 	= 0; 	//! [px]

	protected bool m_Initialized;
	protected bool m_Visible; 			//!overall visibility
	protected bool m_IsActive;
	
	// Passed from modifier -> symptom -> effect
	protected float m_Amount;

	protected float m_TargetAmount = 0.0;
	protected float m_CurrentAmount = 0.0;
	
	protected Widget m_EyeFrame;
	protected ImageWidget m_TopPartIrritation;
	protected ImageWidget m_BottomPartIrritation;
	protected ImageWidget m_VeinsLayer;
	protected int m_LidsColorCorrection;
	protected int m_VeinsColorCorrection;
	
	protected static float m_MaskSmoothVelocity[1];
	protected static float m_TransitionWidthSmoothVelocity[1];
	
	void ~GameplayEffectsMetaDataEyeVeins()
	{
		m_Visible = false;
	    m_VeinsLayer.SetMaskProgress(0.0);
		m_VeinsLayer.SetMaskTransitionWidth(0.0);
	}
	
	override void Init(array<ref Widget> input, int type, Widget layout_root, int user_override = -1)
	{
		super.Init(input, type, layout_root, user_override);
		
		m_EyeFrame = m_LayoutRoot.FindAnyWidgetById(EffectWidgetsTypes.EYE_VEINS);
		m_EyeFrame.Show(m_Visible);

		m_VeinsLayer = ImageWidget.Cast(m_EyeFrame.FindWidget("veinsLayer"));
	    m_VeinsLayer.SetMaskProgress(0.0);
		m_VeinsLayer.SetMaskTransitionWidth(0.0);
		m_VeinsLayer.Show(false);

		m_TopPartIrritation = ImageWidget.Cast(m_EyeFrame.FindWidget("topPartIrritation"));
		m_TopPartIrritation.Show(false);

		m_BottomPartIrritation = ImageWidget.Cast(m_EyeFrame.FindWidget("bottomPartIrritation"));
		m_BottomPartIrritation.Show(false);
		
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
				m_Amount 		= params.param1;
				m_TargetAmount 	= Math.Clamp(m_Amount, 0.0, 1.0);
				m_IsActive 		= m_Amount > 0.0 || m_TargetAmount > 0.0;
	            UpdateVeins(timeSlice);
			}
		}
	}
	
	protected void UpdateVeins(float dt)
	{
	    m_CurrentAmount = Math.Lerp(m_CurrentAmount, m_TargetAmount, dt);
	
	    SetVeinsAmount(m_CurrentAmount, dt);
		SetIrritation(m_CurrentAmount, dt);
		SetVeinsColor();
	}
	
	protected void SetVeinsAmount(float amount, float timeSlice)
	{
	    if (m_VeinsLayer)
		{
			m_VeinsLayer.Show(m_IsActive);

			float maskProgress = amount;
			maskProgress = Math.SmoothCD(maskProgress, amount + Math.RandomFloatInclusive(-0.5, 0.0), m_MaskSmoothVelocity, 0.4, 10000, timeSlice);	
			maskProgress = Math.Clamp(maskProgress, 0.0, 0.4);
	        m_VeinsLayer.SetMaskProgress(maskProgress);
			
			
			float transitionWidth = Easing.EaseInOutSine(amount);
			transitionWidth = 1;
			m_VeinsLayer.SetMaskTransitionWidth(transitionWidth);
		}
	}
	
	protected void SetIrritation(float amount, float timeSlice)
	{
		const float ALPHA_MULTIPLIER_MIN = 0.1;
		const float ALPHA_MULTIPLIER_MAX = 0.25;

		int dayTime = g_Game.GetMission().GetWorldData().GetDaytime();
		float dayTimeValueOverride = 1.0;
		switch (dayTime)
		{
			case WorldDataDaytime.DUSK:
			case WorldDataDaytime.NIGHT:
			case WorldDataDaytime.EVENING:
				dayTimeValueOverride = 0.1;
				break;
		}
		float alphaMultiplier = Math.Lerp(ALPHA_MULTIPLIER_MIN, ALPHA_MULTIPLIER_MAX, amount);
		m_LidsColorCorrection = ARGBF(1.0 * alphaMultiplier, 0.45 * dayTimeValueOverride, 0.0, 0.0);

		m_TopPartIrritation.Show(m_IsActive);
        m_TopPartIrritation.SetPos(0, Math.Lerp(TOP_PART_IRRITATION_MIN_Y_POSITION, TOP_PART_IRRITATION_MAX_Y_POSITION, amount));
		m_TopPartIrritation.SetColor(m_LidsColorCorrection);

		m_BottomPartIrritation.Show(m_IsActive);
        m_BottomPartIrritation.SetPos(0, Math.Lerp(BOTTOM_PART_IRRITATION_MIN_Y_POSITION, BOTTOM_PART_IRRITATION_MAX_Y_POSITION, amount));
		m_BottomPartIrritation.SetColor(m_LidsColorCorrection);
	}
	
	protected void SetVeinsColor()
	{
		const float HDR_MIN = 0.005;
		const float HDR_MAX = 1.0;
		const float COLOR_MULTIPLIER_MIN = 0.1;
		const float COLOR_MULTIPLIER_MAX = 0.5;
		
		float hdrMultiplier = GetSceneHDRMul(0);
		hdrMultiplier = Math.Clamp(hdrMultiplier ,HDR_MIN, HDR_MAX);
		hdrMultiplier = Math.InverseLerp(HDR_MIN, HDR_MAX, hdrMultiplier);
		hdrMultiplier = Math.Lerp(COLOR_MULTIPLIER_MAX, COLOR_MULTIPLIER_MIN, hdrMultiplier);
		m_VeinsColorCorrection = ARGBF(1.0 * hdrMultiplier, 1.0 * hdrMultiplier, 1.0 * hdrMultiplier, 1.0 * hdrMultiplier); //grayscaling of the image
		m_VeinsLayer.SetColor(m_VeinsColorCorrection);
	}
	
	override void UpdateVisibility(bool state)
	{
		Widget w = m_EyeFrame;
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