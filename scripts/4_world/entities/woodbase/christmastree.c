class ChristmasTree extends House
{
	XmasTreeLight 		m_TreeLight;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "ChristmasMusic_SoundSet";
	
	void ChristmasTree()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			m_TreeLight = XmasTreeLight.Cast( ScriptedLightBase.CreateLight( XmasTreeLight, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "action" );
		}
	}
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
class ChristmasTree_Green extends ChristmasTree {};