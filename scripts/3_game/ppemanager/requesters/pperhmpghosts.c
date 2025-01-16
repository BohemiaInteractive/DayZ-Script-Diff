class PPERequester_HMPGhosts extends PPERequester_GameplayBase
{
	protected bool m_PulseActive;
	protected float m_PulseProgress;
	
	protected ref array<float> m_ChannelWeights = { 0, 0, 0, 0 };
	protected ref array<float> m_ColorMod = { 1, 1, 1, 1 };
	
	protected bool m_Stopping;
	protected bool m_StopNext;
	protected float m_FadeOutTimeTarget;
	protected float m_FadingTimeElapsed;
	protected float m_FadingProgress;
	protected float m_ElapsedTime;
	
	override protected void OnStart(Param par = null)
	{
		super.OnStart(par);
		
		m_Stopping = false;
		m_StopNext = false;
		m_ElapsedTime = 0.0;
		m_FadingTimeElapsed = 0.0;
		m_FadingProgress = 0.0;
		
		//following changes only performed once, on start
		//noise
		float nScaleX = 0.05; //inverse scaling!
		float nScaleY = 0.05; //inverse scaling!
		//ghosts
		float gOffsetX = 30;
		float gOffsetY = 13;
		float gScaleX = 1;	//inverse scaling!
		float gScaleY = 1;	//inverse scaling!
		//color modulation
		m_ColorMod[0] = 0.8;
		m_ColorMod[1] = 0.8;
		m_ColorMod[2] = 0.8;
		m_ColorMod[3] = 0.85;
		
		SetTargetValueFloat(PostProcessEffectType.Ghost,PPEGhost.PARAM_NOISE_SCALE_X,false, nScaleX,PPEGhost.L_4_HMP,PPOperators.LOWEST);
		SetTargetValueFloat(PostProcessEffectType.Ghost,PPEGhost.PARAM_NOISE_SCALE_Y,false, nScaleY,PPEGhost.L_5_HMP,PPOperators.LOWEST);
		SetTargetValueFloat(PostProcessEffectType.Ghost,PPEGhost.PARAM_GHOST_OFFSET_X,false, gOffsetX,PPEGhost.L_6_HMP,PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Ghost,PPEGhost.PARAM_GHOST_OFFSET_Y,false, gOffsetY,PPEGhost.L_7_HMP,PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Ghost,PPEGhost.PARAM_GHOST_SCALE_X,false, gScaleX,PPEGhost.L_8_HMP,PPOperators.LOWEST);
		SetTargetValueFloat(PostProcessEffectType.Ghost,PPEGhost.PARAM_GHOST_SCALE_Y,false, gScaleY,PPEGhost.L_9_HMP,PPOperators.LOWEST);
		SetTargetValueColor(PostProcessEffectType.Ghost,PPEGhost.PARAM_GHOST_COLOR_MOD,m_ColorMod,PPEGhost.L_1_HMP,PPOperators.SET);
	}
	
	override protected void OnUpdate(float delta)
	{
		if (m_StopNext)
		{
			if (m_IsRunning)
				Stop();
			return;
		}
		
		if (m_IsRunning)
			ProcessSimulation(delta);
		
		super.OnUpdate(delta);
		
		if (m_IsRunning)
			SetRequesterUpdating(true);
	}
	
	protected void ProcessSimulation(float delta)
	{
		if (m_Stopping)
			ProcessFading(delta);
		
		if (m_PulseActive)
		{
			//channel weights
			float time = m_ElapsedTime * 0.65;
			SampleChannels(time);
			ReSampleChannels(time);
			
			//noise
			float nOffsetX = (Math.Sin( time * 0.29 ) * 0.5 + 0.5) * 0.3;
			float nOffsetY = (Math.Cos( time * 0.17 ) * 0.5 + 0.5) * 0.3;
			
			//channel remapping
			float animLo = (Math.Sin( time ) * 0.5 + 0.5) * 1;
			float animHi = (Math.Sin( time ) * 0.5 + 0.5) * 1;
			float rmpLoAll = Math.Lerp(0.1,0.3,animLo);
			float rmpHiAll = Math.Lerp(0.9,1.2,animHi);
			
			SetTargetValueColor(PostProcessEffectType.Ghost,PPEGhost.PARAM_NOISE_CHANNEL_WEIGHT,m_ChannelWeights,PPEGhost.L_1_HMP,PPOperators.SET);
			SetTargetValueFloat(PostProcessEffectType.Ghost,PPEGhost.PARAM_NOISE_OFFSET_X,false, nOffsetX,PPEGhost.L_2_HMP,PPOperators.ADD);
			SetTargetValueFloat(PostProcessEffectType.Ghost,PPEGhost.PARAM_NOISE_OFFSET_Y,false, nOffsetY,PPEGhost.L_3_HMP,PPOperators.ADD);
			SetTargetValueColor(PostProcessEffectType.Ghost,PPEGhost.PARAM_NOISE_REMAP_LO,{rmpLoAll,rmpLoAll,rmpLoAll,rmpLoAll},PPEGhost.L_1_HMP,PPOperators.SET);
			SetTargetValueColor(PostProcessEffectType.Ghost,PPEGhost.PARAM_NOISE_REMAP_HI,{rmpHiAll,rmpHiAll,rmpHiAll,rmpHiAll},PPEGhost.L_1_HMP,PPOperators.SET);
			
			m_PulseActive = m_PulseProgress < 1.0;
		}
		
		m_ElapsedTime += delta;
	}
	
	protected void SampleChannels(float time)
	{
		float offset;
		// luckily 4 channels * 90 fit into 360 nicely, so no need to distribute weights other way
		for (int i = 0; i < 4; ++i)
		{
			offset = i * Math.PI_HALF;
			m_ChannelWeights[i] = Math.Sin( time + offset );
		}
	}
	
	protected void ReSampleChannels(float time)
	{
		float rampingCoef = 1.1;
		float w = Math.Clamp((Math.Sin(m_PulseProgress * Math.PI) * rampingCoef),0,1);
		w = Math.Lerp(0,w,1 - m_FadingProgress);
		
		for (int i = 0; i < 4; ++i)
			m_ChannelWeights[i] = w * m_ChannelWeights[i];
	}
	
	protected void ProcessFading(float delta)
	{
		m_FadingTimeElapsed += delta;
		m_FadingProgress = m_FadingTimeElapsed/m_FadeOutTimeTarget;
		
		m_StopNext = m_FadingProgress >= 1.0;
	}
	
	void SetPulseProgress(float progress) //0..1
	{
		m_PulseActive = true;
		m_PulseProgress = progress;
	}
	
	void FadeOutEffect(float targetTime)
	{
		m_Stopping = true;
		m_FadeOutTimeTarget = targetTime;
		m_FadingTimeElapsed = 0.0;
	}
}