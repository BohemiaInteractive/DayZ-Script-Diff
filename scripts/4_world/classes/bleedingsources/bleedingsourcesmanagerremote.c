//this is instantiated on the client for both the controlled character, as well as the remote characters
class BleedingSourcesManagerRemote extends BleedingSourcesManagerBase
{
	int m_BleedingBits;
	bool m_ShowDiag;
	bool m_ShowingDiag;
	Shape m_Point;
	bool m_EnableHitIndication = false;
	
	override protected void Init()
	{
		super.Init();
		
		if (GetGame().GetMission().GetEffectWidgets()/* && m_Player.IsControlledPlayer()*/)
		{
			Param3<bool,int,float> par = new Param3<bool,int,float>(true,0,0);
			GetGame().GetMission().GetEffectWidgets().RegisterGameplayEffectData(EffectWidgetsTypes.BLEEDING_LAYER,par);
		}
	}
	
	override protected void RegisterBleedingZoneEx(string name, int max_time, string bone = "", vector orientation = "0 0 0", vector offset = "0 0 0", float flow_modifier = 1, string particle_name = "BleedingSourceEffect", int inv_location = 0)
	{
		super.RegisterBleedingZoneEx(name,max_time,bone,orientation,offset,flow_modifier,particle_name,inv_location);
		
		if (GetGame().GetMission().GetEffectWidgets()/* && m_Player.IsControlledPlayer()*/)
		{
			Param3<bool,int,float> par = new Param3<bool,int,float>(false,m_Bit,flow_modifier);
			GetGame().GetMission().GetEffectWidgets().RegisterGameplayEffectData(EffectWidgetsTypes.BLEEDING_LAYER,par);
		}
	}
	
	void OnVariablesSynchronized(int current_bits)
	{
		if (current_bits != m_BleedingBits)
		{
			if(m_BleedingBits == 0)
			{
				m_Player.OnBleedingBegin();
			}
			OnBleedingBitsUpdate(m_BleedingBits, current_bits);
			m_BleedingBits = current_bits;
			if(m_BleedingBits == 0)
			{
				m_Player.OnBleedingEnd();
			}
		}
	}
	
	void Reload()
	{
		m_BleedingSourceZone.Clear();
		m_BitOffset = 0;
		Init();
		int bit_offset = 0;
		
		for(int i = 0; i < BIT_INT_SIZE; i++)
		{
			int bit = 1 << bit_offset;
			bit_offset++;
			if( (bit & m_BleedingBits) != 0 )
			{
				RemoveBleedingSource(bit);
				AddBleedingSource(bit);
			}
		}
	}

	void OnBleedingBitsUpdate(int old_mask, int new_mask)
	{
		for(int i = 0; i < 32; i++)
		{
			int compare_bit = 1 << i;
			int new_compare_result_bit = compare_bit & new_mask;
			int old_compare_result_bit = compare_bit & old_mask;
			
			if( new_compare_result_bit )
			{
				if( !(new_compare_result_bit & old_mask))
				{
					//a different active bit in the new mask
					AddBleedingSource(new_compare_result_bit);
				}
			}
			else
			{
				if( new_compare_result_bit != old_compare_result_bit )
				{
					RemoveBleedingSource(old_compare_result_bit);
				}
			}
		}
	}
	
	int GetBleedingSourceCountRemote()
	{
		int bleeding_source_count = 0;
		int pow = 0;
		
		for(int i = 0; i < BIT_INT_SIZE ; i++)
		{
			int bit = Math.Pow(2, pow);
			pow++;
			if( (m_BleedingBits & bit) != 0)
			{
				bleeding_source_count++;
			}
		}
		
		return bleeding_source_count;
	}
	
	void SetDiag(bool value)
	{
		m_ShowDiag = value;
		return;
		int boneIdx = m_Player.GetBoneIndexByName("RightArmExtra");

		if ( boneIdx != -1 )
		{
		  Object linkedObject = GetGame().CreateObject("Ammo_ArrowBolt", "0 0 0");
		
		//linkedObject.SetPosition("0 1 0");
		//linkedObject.SetOrientation("0 90 0");
		  /*
		
		  Set local space transform for linked object
		
		  */
			
			BleedingSourceEffect eff = new BleedingSourceEffect();
			eff.SetAutodestroy(true);
			SEffectManager.PlayInWorld( eff, "0 0 0" );
			Particle p = eff.GetParticle();
			//p.SetOrientation("0 90 0");
			m_Player.AddChild(p, boneIdx);
		
		 	 m_Player.AddChild(linkedObject, boneIdx);
		}
	
	}
	
	void OnUpdate()
	{
		#ifndef NO_GUI
		if(m_ShowDiag)
		{
			DisplayDebug();
			DisplayVisualDebug();
		}
		else if( m_ShowingDiag )
		{
			CleanDebug();
			m_ShowingDiag = false;
		}
		#endif
	}
	
	void DisplayDebug()
	{	
		m_ShowingDiag = true;
		
		DbgUI.BeginCleanupScope();
        DbgUI.Begin("Bleeding Sources", 50, 50);
		
		int pow = 0;
		
		for(int i = 0; i < BIT_INT_SIZE ; i++)
		{
			int bit = Math.Pow(2, pow);
			pow++;
			if( (m_BleedingBits & bit) != 0)
			{
				BleedingSourceZone bsz = GetBleedingSourceMeta(bit);
				
				string name = GetSelectionNameFromBit(bit);
				string slot_name =  InventorySlots.GetSlotName(bsz.GetInvLocation());
				DbgUI.Text(name + "| slot name: "+ slot_name);
			}
		}
		
        DbgUI.End();
        DbgUI.EndCleanupScope();
	}
	
	void DisplayVisualDebug()
	{
		/*
		if(m_Point) 
		{
			Debug.RemoveShape(m_Point);
		}
		
		int boneIdx = m_Player.GetBoneIndexByName("LeftKneeExtra");
		int pointIdx = m_Player.GetMemoryPointIndex("lknee");
		
		vector posLS = DayZPlayerUtils.GetMemoryPointPositionBoneRelative(m_Player, boneIdx, pointIdx);
		
		vector pTm[4];
		m_Player.GetBoneTransformMS(boneIdx, pTm);
		vector posMS = posLS.Multiply4(pTm);
		
		vector pos = m_Player.ModelToWorld(posMS);
		m_Point = Debug.DrawSphere(pos, 0.1, COLOR_RED);
		*/
		
		for(int i = 0; i < m_BleedingSources.Count(); i++)
		{
			m_BleedingSources.GetElement(i).DrawDebugShape();
		}
	}
	
	void CleanDebug()
	{
		DbgUI.BeginCleanupScope();
		DbgUI.Begin("Bleeding Sources", 50, 50);
		DbgUI.End();
		DbgUI.EndCleanupScope();
	}
}