class ChristmasTree extends House
{
	XmasTreeLight 		m_TreeLight;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "ChristmasMusic_SoundSet";
	
	void ChristmasTree()
	{
		if ( !g_Game.IsDedicatedServer() )
		{
			m_TreeLight = XmasTreeLight.Cast( ScriptedLightBase.CreateLight( XmasTreeLight, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "action" );
		}
	}
	
	override void EEInit()
	{
		if ( !g_Game.IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !g_Game.IsDedicatedServer() )
		{
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
class ChristmasTree_Green extends ChristmasTree {};