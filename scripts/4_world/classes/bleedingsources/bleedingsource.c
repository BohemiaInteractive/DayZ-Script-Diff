enum eBleedingSourceType
{
	NORMAL,
	CONTAMINATED,
}

class BleedingSource
{
	vector m_Position;
	Particle m_BloodParticle;
	PlayerBase m_Player;
	int m_Bit;
	string m_Bone;
	ref EffectParticle m_BleedingEffect;
	vector m_Orientation;
	Shape m_DebugShape;
	Shape m_DebugShape1;
	ref Timer m_DebugTick;
	vector m_Offset;
	float m_FlowModifier;
	float m_ActiveTime;
	float m_MaxTime;
	string m_ParticleName;
	bool m_DeleteRequested;
	eBleedingSourceType m_Type = eBleedingSourceType.NORMAL;
	
	void BleedingSource(PlayerBase player, int bit, string bone, vector orientation, vector offset,int max_time, float flow_modifier, string particle_name)
	{
		//m_Position = position;
		m_Player = player;
		m_Bit = bit;
		m_Bone = bone;
		m_Orientation = orientation;
		m_Offset = offset;
		m_FlowModifier = flow_modifier;
		m_MaxTime = max_time;
		m_ParticleName = particle_name;
		
		//CreateBleedSymptom();
		if (!GetGame().IsDedicatedServer())
		{	
			CreateParticle();
			StartSourceBleedingIndication();

		}
	}

	void ~BleedingSource()
	{
		if (m_BloodParticle)
			RemoveParticle();
		
		if (m_DebugShape)
		{
			Debug.RemoveShape(m_DebugShape);
		}		
		
		if (m_DebugShape1)
		{
			Debug.RemoveShape(m_DebugShape1);
		}
		
		StopSourceBleedingIndication(!m_Player || !m_Player.IsAlive());
	}
	
	void SetType(eBleedingSourceType type)
	{
		m_Type = type;
	}
	
	eBleedingSourceType GetType()
	{
		return m_Type;
	}
	
	int GetActiveTime()
	{
		return m_ActiveTime;
	}
	
	void SetActiveTime(int time)
	{
		m_ActiveTime = time;
	}
	
	int GetBit()
	{
		return m_Bit;
	}
	
	void CreateParticle()
	{
		int boneIdx = m_Player.GetBoneIndexByName(m_Bone);
		m_BleedingEffect = EffectParticle.Cast(m_ParticleName.ToType().Spawn());
		if (m_BleedingEffect)
		{
			SEffectManager.PlayInWorld( m_BleedingEffect, "0 0 0" );
			m_BloodParticle = m_BleedingEffect.GetParticle();
			m_BloodParticle.SetOrientation(m_Orientation);
			vector pos;
			pos += m_Offset;
			m_BloodParticle.SetPosition(pos);
			float time = Math.RandomFloat01() * 2;
			//time = time;
			m_BloodParticle.SetParameter(-1, EmitorParam.CURRENT_TIME, time);
			//m_BloodParticle.SetParameter(1, EmitorParam.CURRENT_TIME, time);
			
			m_Player.AddChild(m_BloodParticle, boneIdx);
			return;
		}
		else
		{
			Error("bleeding source: failed to spawn the particle: "+m_ParticleName);
		}
	}
	
	void RemoveParticle()
	{
		SEffectManager.DestroyEffect(m_BleedingEffect);
	}

	void OnUpdateServer(float deltatime, float blood_scale, bool no_blood_loss )
	{
		m_ActiveTime += deltatime;
		
		if(m_ActiveTime >= m_MaxTime)
		{
			if(m_Player.GetBleedingManagerServer() && !m_DeleteRequested)
			{
				m_Player.GetBleedingManagerServer().RequestDeletion(GetBit());//add yourself to a list of sources to be deleted
				m_DeleteRequested = true;
			}
		}
		if( !no_blood_loss )
		{
			float flow = m_FlowModifier;
			switch( m_Type )
			{
				case eBleedingSourceType.NORMAL:
				{
					//do nothing
					break;
				}
				case eBleedingSourceType.CONTAMINATED:
				{
					flow *= PlayerConstants.BLEEDING_SOURCE_BURN_MODIFIER;
				}
			}
			m_Player.AddHealth("GlobalHealth","Blood", (PlayerConstants.BLEEDING_SOURCE_BLOODLOSS_PER_SEC * blood_scale * deltatime * flow) );
		}
	}
	
	void StartSourceBleedingIndication()
	{
		if (m_Player.IsControlledPlayer())
		{
			#ifdef DEVELOPER
			if (DbgBleedingIndicationStaticInfo.m_DbgEnableBleedingIndication)
			{
			#endif
				Param4<bool,int,vector,bool> par = new Param4<bool,int,vector,bool>(true,m_Bit,"0 0 0",false);
				GetGame().GetMission().GetEffectWidgets().AddActiveEffects({EffectWidgetsTypes.BLEEDING_LAYER});
				GetGame().GetMission().GetEffectWidgets().UpdateWidgets(EffectWidgetsTypes.BLEEDING_LAYER,0,par);
			#ifdef DEVELOPER
			}
			#endif
		}
	}
	
	void StopSourceBleedingIndication(bool instant = false)
	{
		if ( m_Player && m_Player.IsControlledPlayer() && GetGame() && (!GetGame().IsDedicatedServer()) )
		{
			Param4<bool,int,vector,bool> par = new Param4<bool,int,vector,bool>(false,m_Bit,"0 0 0",instant);
			GetGame().GetMission().GetEffectWidgets().UpdateWidgets(EffectWidgetsTypes.BLEEDING_LAYER,0,par);
		}
	}
	
	void DrawDebugShape()
	{
		if(m_DebugShape)
		{
			Debug.RemoveShape(m_DebugShape);
		}
		
		if(m_DebugShape1) 
		{
			Debug.RemoveShape(m_DebugShape1);
		}

		Particle p = m_BleedingEffect.GetParticle();
		vector pos = p.GetPosition();
		m_DebugShape = Debug.DrawSphere(pos, 0.009, COLOR_BLUE, ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE|ShapeFlags.NOZBUFFER);
		vector arrow_to = m_BloodParticle.GetOrientation();
		arrow_to = arrow_to.AnglesToVector();
		arrow_to = -arrow_to * 0.3;
		arrow_to = pos + arrow_to;
		
		m_DebugShape1 = Debug.DrawArrow(pos, arrow_to, 0.1, COLOR_GREEN);
	}
}
