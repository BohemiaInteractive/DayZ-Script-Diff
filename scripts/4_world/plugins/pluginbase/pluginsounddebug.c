class PluginSoundDebug extends PluginBase
{
	override void OnInit()
	{
		m_TickTimer = new Timer();
		m_TickTimer.Run(0.1, this, "OnGUITimer", NULL, true);
	}
	
	override void OnUpdate(float delta_time)
	{
		;
	}
	
	override void OnDestroy()
	{
		m_TickTimer = NULL;
	}
	
	void Show()
	{
		m_TickTimer = new Timer();
		m_TickTimer.Run(0.1, this, "OnGUITimer", NULL, true);
	}
	
	void Hide()
	{
		m_TickTimer = NULL;
	}
	
	void OnGUITimer()
	{
		DbgUI.BeginCleanupScope();
		DbgUI.Begin("Sound debug", 10, 10);
		
		DbgUI.PushID_Str("SoundParams");
		DbgUI.Text("SoundParams: ");
		DbgUI.SameLine();
		string soundsetName = "BearGrowl_SoundSet";
		DbgUI.InputText("", soundsetName, 200);
		DbgUI.PopID();
		
		DbgUI.PushID_Str("Offset");
		DbgUI.Text("Offset pos: ");
		vector posOffset = "0 0 0";
		float posVal;
		
		DbgUI.SameLine();
		DbgUI.PushID_Int(1);
			DbgUI.InputFloat("", posVal, 80);
		DbgUI.PopID();
		posOffset[0] = posVal;
		
		DbgUI.SameLine();
		DbgUI.PushID_Int(2);
			DbgUI.InputFloat("", posVal, 80);
		DbgUI.PopID();
		posOffset[1] = posVal;
		
		DbgUI.SameLine();
		DbgUI.PushID_Int(3);
			DbgUI.InputFloat("", posVal, 80);
		DbgUI.PopID();
		posOffset[2] = posVal;
		DbgUI.PopID();
		
		if(DbgUI.Button("Create"))
		{
			m_soundParams = new SoundParams(soundsetName);
			m_soundBuilder = new SoundObjectBuilder(m_soundParams);
			m_soundObject = m_soundBuilder.BuildSoundObject();
			m_soundObject.SetPosition(GetGame().GetPlayer().GetPosition() + posOffset);
		}
		
		DbgUI.Text("SoundObjectBuilder: ");
		
		DbgUI.Text("SoundObject: ");
		
		DbgUI.Text("AbstractWave: ");
		
		vector posOffset2 = "0 10 0";
		
		if(m_soundParams != NULL)
		{
			DbgUI.Text("FadeInFactor: ");
			DbgUI.SameLine();
			DbgUI.PushID_Str("fadeIn");
			DbgUI.InputFloat("", fadeInVolume, 80);
			DbgUI.PopID();
			
			DbgUI.Text("FadeOutFactor: ");
			DbgUI.SameLine();
			DbgUI.PushID_Str("fadeOut");
			DbgUI.InputFloat("", fadeOutVolume, 80);
			DbgUI.PopID();
			
			DbgUI.PushID_Str("Offset2");
			DbgUI.Text("Offset2 pos: ");
			float posVal2;

			DbgUI.SameLine();
			DbgUI.PushID_Int(100);
				posVal2 = posOffset2[0];
				DbgUI.InputFloat("", posVal2, 80);
			DbgUI.PopID();
			posOffset2[0] = posVal2;

			DbgUI.SameLine();
			DbgUI.PushID_Int(101);
				posVal2 = posOffset2[1];
				DbgUI.InputFloat("", posVal2, 80);
			DbgUI.PopID();
			posOffset2[1] = posVal2;

			DbgUI.SameLine();
			DbgUI.PushID_Int(102);
				posVal2 = posOffset2[2];
				DbgUI.InputFloat("", posVal2, 80);
			DbgUI.PopID();
			posOffset2[2] = posVal2;
			DbgUI.PopID();
			
			DbgUI.Text("skip: ");
			DbgUI.SameLine();
			float skip = 0.0;
			DbgUI.PushID_Int(200);
			DbgUI.InputFloat("", skip, 80);
			DbgUI.PopID();
					
			if(DbgUI.Button("Create and play"))
			{
				m_wave = GetGame().GetSoundScene().Play3D(m_soundObject, m_soundBuilder);
				m_wave.SetFadeInFactor(fadeInVolume);
				m_wave.Skip(skip);
				m_wave.SetPosition(GetGame().GetPlayer().GetPosition() + posOffset2);
			}
		}
		
		if(m_wave != NULL)
		{
			DbgUI.Text("Wave length: " + m_wave.GetLength() + "s");
			
			//DbgUI.Text("Wave position: " + Math.Round(m_wave.GetCurrPosition() * 100) + "%");
			DbgUI.Text("Volume: ");
			DbgUI.SameLine();
			float volume = 1.0;
			DbgUI.InputFloat(" ", volume, 80);
			DbgUI.SameLine();
			if(DbgUI.Button("SetVolume"))
				m_wave.SetVolume(volume);
			DbgUI.SameLine();
			if(DbgUI.Button("SetVolumeRelative"))
				m_wave.SetVolumeRelative(volume);
			
			if(DbgUI.Button("Play"))
				m_wave.Play();
			if(DbgUI.Button("Stop"))
				m_wave.Stop();
			if(DbgUI.Button("Restart"))
				m_wave.Restart();
			if(DbgUI.Button("Repeat"))
				m_wave.Loop(true);
			if(DbgUI.Button("StopRepeat"))
				m_wave.Loop(false);
			if(DbgUI.Button("Set position"))
				m_wave.SetPosition(GetGame().GetPlayer().GetPosition() + posOffset2);
		}
		
		DbgUI.End();
		DbgUI.EndCleanupScope();
	}
	
	
	ref Timer m_TickTimer;
	
	ref SoundParams m_soundParams;
	ref SoundObjectBuilder m_soundBuilder;
	ref SoundObject m_soundObject;
	float fadeInVolume = 1.1;
	float fadeOutVolume = 0.9;
	AbstractWave m_wave;
}