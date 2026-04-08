//this is instantiated on the client for both the controlled character, as well as the remote characters
class BleedingSourcesManagerRemote extends BleedingSourcesManagerBase
{
	int m_BleedingBits;
	bool m_ShowDiag;
	bool m_ShowingDiag;
	bool m_ShowingDiagDraw;
	Shape m_Point;
	bool m_EnableHitIndication = false;
	
	override protected void Init()
	{
		super.Init();
		
		if (g_Game.GetMission().GetEffectWidgets()/* && m_Player.IsControlledPlayer()*/)
		{
			Param3<bool,int,float> par = new Param3<bool,int,float>(true,0,0);
			g_Game.GetMission().GetEffectWidgets().RegisterGameplayEffectData(EffectWidgetsTypes.BLEEDING_LAYER,par);
		}
	}
	
	override protected void RegisterBleedingZoneEx(string name, int max_time, string bone = "", vector orientation = "0 0 0", vector offset = "0 0 0", float flow_modifier = 1, string particle_name = "BleedingSourceEffect", int inv_location = 0)
	{
		super.RegisterBleedingZoneEx(name,max_time,bone,orientation,offset,flow_modifier,particle_name,inv_location);
		
		GameplayEffectWidgets_base widgets = g_Game.GetMission().GetEffectWidgets();
		if (widgets/* && m_Player.IsControlledPlayer()*/)
		{
			Param3<bool,int,float> par = new Param3<bool,int,float>(false,m_Bit,flow_modifier);
			widgets.RegisterGameplayEffectData(EffectWidgetsTypes.BLEEDING_LAYER,par);
		}
	}
	
	void OnVariablesSynchronized(int current_bits)
	{
		if (current_bits != m_BleedingBits)
		{
			if (m_BleedingBits == 0)
			{
				m_Player.OnBleedingBegin();
			}
			OnBleedingBitsUpdate(m_BleedingBits, current_bits);
			m_BleedingBits = current_bits;
			if (m_BleedingBits == 0)
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
		
		for (int i = 0; i < BIT_INT_SIZE; ++i)
		{
			int bit = 1 << bit_offset;
			bit_offset++;
			if ((bit & m_BleedingBits) != 0)
			{
				RemoveBleedingSource(bit);
				AddBleedingSource(bit);
			}
		}
	}
	
	
	override protected void AddBleedingSource(int bit)
	{
		super.AddBleedingSource(bit);
		#ifdef DIAG_DEVELOPER
		m_BleedingSources.Get(bit).m_DiagTimeStart = g_Game.GetTickTime();
		#endif
		if (g_Game.IsMultiplayer())
			m_Player.OnBleedingSourceAdded();
	}
	
	override protected bool RemoveBleedingSource(int bit)
	{
		if (super.RemoveBleedingSource(bit))
		{
			if (g_Game.IsMultiplayer())
				m_Player.OnBleedingSourceRemovedEx(m_Item);
			return true;
		}
		return false;
	}

	void OnBleedingBitsUpdate(int old_mask, int new_mask)
	{
		for (int i = 0; i < 32; ++i)
		{
			int compare_bit = 1 << i;
			int new_compare_result_bit = compare_bit & new_mask;
			int old_compare_result_bit = compare_bit & old_mask;
			
			if (new_compare_result_bit)
			{
				if (!(new_compare_result_bit & old_mask))
				{
					//a different active bit in the new mask
					AddBleedingSource(new_compare_result_bit);
				}
			}
			else
			{
				if (new_compare_result_bit != old_compare_result_bit)
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
		
		for (int i = 0; i < BIT_INT_SIZE ; ++i)
		{
			int bit = Math.Pow(2, pow);
			pow++;
			if ((m_BleedingBits & bit) != 0)
			{
				bleeding_source_count++;
			}
		}
		
		return bleeding_source_count;
	}
	
	void SetDiag(bool value)
	{
		m_ShowDiag = value;
	}
	
	void OnUpdate()
	{
		#ifndef NO_GUI
		if (m_ShowDiag)
		{
			DisplayDebug();
			DisplayVisualDebug();
		}
		else if (m_ShowingDiag || m_ShowingDiagDraw)
		{
			if (m_ShowingDiag)
				CleanDebug();
			if (m_ShowingDiagDraw)
				CleanVisualDebug();
		}
		#endif
	}
	
	void DisplayDebug()
	{	
		m_ShowingDiag = true;
		DbgUI.BeginCleanupScope();
        DbgUI.Begin("Bleeding Sources", 50, 50);
		
		int pow = 0;
		bool anyBleedingSourceActive = false;
		
		for (int i = 0; i < BIT_INT_SIZE ; ++i)
		{
			int bit = Math.Pow(2, pow);
			pow++;
			if ((m_BleedingBits & bit) != 0)
			{
				BleedingSourceZone bsz = GetBleedingSourceMeta(bit);
				string name = GetSelectionNameFromBit(bit);
				string slot_name =  InventorySlots.GetSlotName(bsz.GetInvLocation());
				float timeRemaining = -1;
				
				#ifdef DIAG_DEVELOPER
				BleedingSource bsi = m_BleedingSources.Get(bit);
				timeRemaining = bsz.GetMaxTime() + bsi.m_DiagTimeStart - g_Game.GetTickTime();
				timeRemaining = Math.Round(timeRemaining);
				#endif
				
				DbgUI.Text(string.Format("zone: %1 | closest inv. slot: %2 | time remaining: %3", name, slot_name, timeRemaining.ToString()));
				anyBleedingSourceActive = true;
			}
		}
		
		if (!anyBleedingSourceActive)
		{
			DbgUI.Text("No bleeding sources are active.");
		}
		else
		{
			DbgUI.Text("");
			DbgUI.Text("Note: BleedingSourcesManagerServer only updates active sources every 3s, displayed times are client estimates.");
		}
		
        DbgUI.End();
        DbgUI.EndCleanupScope();
	}
	
	void CleanDebug()
	{
		m_ShowingDiag = false;
		
		DbgUI.BeginCleanupScope();
		DbgUI.Begin("Bleeding Sources", 50, 50);
		DbgUI.End();
		DbgUI.EndCleanupScope();
	}
	
	void DisplayVisualDebug()
	{
		/*
		if (m_Point) 
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
		
		m_ShowingDiagDraw = true;
		
		int bsCount = m_BleedingSources.Count();
		for (int i = 0; i < bsCount; ++i)
		{
			m_BleedingSources.GetElement(i).DrawDebugShape();
		}
	}
	
	void CleanVisualDebug()
	{
		m_ShowingDiagDraw = false;
		
		int bsCount = m_BleedingSources.Count();
		for (int i = 0; i < bsCount; ++i)
		{
			m_BleedingSources.GetElement(i).RemoveDebugShape();
		}
	}
}