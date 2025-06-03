class PPERequester_UnconEffects extends PPERequester_GameplayBase
{
	protected const float FADING_TIME_DEFAULT = 3.0;
	
	protected float m_Intensity;
	protected float m_FadeoutStartIntensity;
	protected bool m_Stopping;
	protected bool m_StopNext;
	protected float m_FadeOutTimeTarget;
	protected float m_FadingTimeElapsed;
	protected float m_FadingProgress;
	
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		m_Intensity = Param1<float>.Cast(par).param1;
		m_Stopping = false;
		m_StopNext = false;
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTE,false,m_Intensity,PPEGlow.L_25_UNCON,PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTECOLOR,{0.0,0.0,0.0,0.0},PPEGlow.L_26_UNCON,PPOperators.LOWEST);
	}
	
	override protected void OnStop(Param par = null)
	{
		super.OnStop(par);
		
		m_FadingProgress = 0.0;
		m_Stopping = false;
		m_StopNext = false;
	}
	
	override protected void OnUpdate(float delta)
	{
		if (m_StopNext)
		{
			if (m_IsRunning)
				Stop();
			return;
		}
		
		if (m_IsRunning && m_Stopping)
			ProcessFading(delta);
		
		super.OnUpdate(delta);
		
		if (m_IsRunning && m_Stopping)
			SetRequesterUpdating(true);
	}
	
	void FadeOutEffect(float targetTime = FADING_TIME_DEFAULT)
	{
		m_Stopping = true;
		m_FadeoutStartIntensity = m_Intensity;
		m_FadeOutTimeTarget = targetTime;
		m_FadingTimeElapsed = 0.0;
		
		if (m_IsRunning)
			SetRequesterUpdating(true);
	}
	
	protected void ProcessFading(float delta)
	{
		m_FadingTimeElapsed += delta;
		m_FadingProgress = m_FadingTimeElapsed/m_FadeOutTimeTarget;
		
		m_StopNext = m_FadingProgress >= 1.0;
		
		m_Intensity = Math.Lerp(0,m_FadeoutStartIntensity,Easing.EaseInOutQuad(1 - m_FadingProgress));
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTE,false,m_Intensity,PPEGlow.L_25_UNCON,PPOperators.HIGHEST);
	}
}