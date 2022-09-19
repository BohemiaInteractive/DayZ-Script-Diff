enum WaveKind 
{
	WAVEEFFECT,
	WAVEEFFECTEX,
	WAVESPEECH,
	WAVEMUSIC,
	WAVESPEECHEX,
	WAVEENVIRONMENT,
	WAVEENVIRONMENTEX,
	WAVEWEAPONS,
	WAVEWEAPONSEX,
	WAVEATTALWAYS,
	WAVEUI
}

class AbstractSoundScene
{
	private void AbstractSoundScene() {}
	private void ~AbstractSoundScene() {}
	
	proto native AbstractWave Play2D(SoundObject soundObject, SoundObjectBuilder soundBuilder);
	proto native AbstractWave Play3D(SoundObject soundObject, SoundObjectBuilder soundBuilder);
	proto native SoundObject BuildSoundObject(SoundObjectBuilder soundObjectbuilder);

	proto native float GetRadioVolume();
	proto native void SetRadioVolume(float vol, float time);

	proto native float GetSpeechExVolume();
	proto native void SetSpeechExVolume(float vol, float time);

	proto native float GetMusicVolume();
	proto native void SetMusicVolume(float vol, float time);

	proto native float GetSoundVolume();
	proto native void SetSoundVolume(float vol, float time);

	proto native float GetVOIPVolume();
	proto native void SetVOIPVolume(float vol, float time);
	
	proto native float GetSilenceThreshold();
	proto native float GetAudioLevel();
}


class SoundObjectBuilder
{
	void SoundObjectBuilder(SoundParams soundParams);	
	
	SoundObject BuildSoundObject()
	{
		return GetGame().GetSoundScene().BuildSoundObject(this);
	}

	proto native void Initialize(SoundParams soundParams);
	proto native void UpdateEnvSoundControllers(vector position);
	proto native void SetVariable(string name, float value);
}


class SoundObject
{
	void SoundObject(SoundParams soundParams);
	
	proto native void SetPosition(vector position);
	proto native vector GetPosition();
	proto native void SetOcclusionObstruction(float occlusion, float obstruction);
	proto native void SetKind(WaveKind kind);
	proto native void Initialize(SoundParams soundParams);
}

//soundsys.hpp
class SoundParams
{
	void SoundParams(string name);
		
	proto native bool Load(string name);	
	proto native bool IsValid();
	proto string GetName();
}

class AbstractWaveEvents
{
	ref ScriptInvoker Event_OnSoundWaveStarted = new ScriptInvoker();
	ref ScriptInvoker Event_OnSoundWaveStopped = new ScriptInvoker();
	ref ScriptInvoker Event_OnSoundWaveLoaded = new ScriptInvoker();
	ref ScriptInvoker Event_OnSoundWaveHeaderLoaded = new ScriptInvoker();
	ref ScriptInvoker Event_OnSoundWaveEnded = new ScriptInvoker();
}

class AbstractWave
{
	private void InitEvents()
	{
		AbstractWaveEvents events = new AbstractWaveEvents();
		SetUserData(events);
	}

	#ifdef DIAG_DEVELOPER
	private void AbstractWave() { InitEvents(); }
	private void ~AbstractWave() {}
	#else
	void AbstractWave() { InitEvents(); }
	#endif
	
	proto void SetUserData(Managed inst);
	proto Managed GetUserData();
	
	proto void Play();
	
	void PlayWithOffset(float offset)
	{
		Play();
		SetStartOffset(offset);
	}
	//proto native void Mute();
	proto void Stop();
	proto void Restart();
	proto void SetStartOffset(float offset);
	//! WARNING: Blocking! Waits for header to load
	proto float GetLength();
	//! Current position in percentage of total length
	proto float GetCurrPosition();
	proto void Loop(bool setLoop);
	proto float GetVolume();
	proto void SetVolume(float value);
	proto void SetVolumeRelative(float value);
	proto void SetFrequency(float value);
	proto float GetFrequency();
	proto void SetPosition(vector position);
	proto void SetFadeInFactor(float volume);
	proto void SetFadeOutFactor(float volume);
	proto void Skip(float timeSec);
	proto bool IsHeaderLoaded();
	
	AbstractWaveEvents GetEvents()
	{
		return AbstractWaveEvents.Cast(GetUserData());
	}
	
	void OnPlay()
	{
		GetEvents().Event_OnSoundWaveStarted.Invoke(this);
	}
	
	void OnStop()
	{
		GetEvents().Event_OnSoundWaveStopped.Invoke(this);
	}
		
	void OnLoad()
	{
		GetEvents().Event_OnSoundWaveLoaded.Invoke(this);
	}
	
	void OnHeaderLoad()
	{
		GetEvents().Event_OnSoundWaveHeaderLoaded.Invoke(this);
	}
	
	void OnEnd()
	{
		GetEvents().Event_OnSoundWaveEnded.Invoke(this);
	}
}