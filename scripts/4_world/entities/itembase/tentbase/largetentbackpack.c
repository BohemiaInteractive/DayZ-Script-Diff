class LargeTentBackPack extends Clothing
{		
	ref protected EffectSound 	m_RepackingLoopSound;	
	
	void LargeTentBackPack()
	{
		m_RepackingLoopSound  = new EffectSound;	
	}
	
	void ~LargeTentBackPack()
	{
		SEffectManager.DestroyEffect( m_RepackingLoopSound );	
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type,ParamsReadContext  ctx) 
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		Param1<bool> p = new Param1<bool>(false);
				
		if (!ctx.Read(p))
			return;
		
		bool play = p.param1;
		
		switch (rpc_type)
		{
			case SoundTypeTent.REPACK:
			
				if ( play )
				{
					PlayRepackingLoopSound();
				}
				else
				{
					StopRepackingLoopSound();
				}
			
			break;
		}
	}
	
	void PlayRepackingLoopSound()
	{
		if ( !m_RepackingLoopSound || !m_RepackingLoopSound.IsSoundPlaying() )
		{
			m_RepackingLoopSound = SEffectManager.PlaySound( "largetent_deploy_SoundSet", GetPosition(), 0.5, 0.5 );
		}
	}
	
	void StopRepackingLoopSound()
	{
		m_RepackingLoopSound.SetSoundFadeOut(0.5);
		m_RepackingLoopSound.SoundStop();
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		//AddAction(ActionRepackTent);
	}
};
