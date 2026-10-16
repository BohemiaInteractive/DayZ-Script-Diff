class HudDebugWinCharStats extends HudDebugWinBase
{
	TextListboxWidget m_WgtValues;
	GridSpacerWidget m_WgtPanel;
	Widget m_WgtHeader;
	ref array<Widget> m_StatWidgets = new array<Widget>;
	ref map <SliderWidget, string> m_SliderWidgets = new map<SliderWidget, string>;
	ref array<TextWidget> m_StatValues = new array<TextWidget>;
	ref map<EditBoxWidget, string> m_StatValuesInput = new map<EditBoxWidget, string>;
	bool	m_Populated;
	bool	m_ChangingSlider;
	
	//============================================
	// Constructor
	//============================================
	void HudDebugWinCharStats(Widget widget_root)
	{
		m_WgtRoot = widget_root;
		m_WgtPanel = GridSpacerWidget.Cast(m_WgtRoot.FindAnyWidget("Stats") );
		m_WgtHeader = m_WgtRoot.FindAnyWidget("header_CharacterStats");
		
		OnInit();
	}
	
	void OnInit()
	{
		m_ConfigDebugProfile = PluginConfigDebugProfile.Cast(GetPlugin(PluginConfigDebugProfile));
		if (m_ConfigDebugProfile)
		{
			vector pos = m_ConfigDebugProfile.GetCharacterStatsPos();
			if (m_WgtRoot && pos != vector.Zero)
			{
				float wx = pos[0];
				float wy = pos[1];
				m_WgtRoot.SetPos(wx, wy);
			}
		}
	}

	//============================================
	// Destructor
	//============================================
	void ~HudDebugWinCharStats()
	{
		SetUpdate(false);
	}

	//============================================
	// GetWinType
	//============================================
	override int GetType()
	{
		return HudDebug.HUD_WIN_CHAR_STATS;
	}
	
	//============================================
	// Show 
	//============================================
	override void Show()
	{
		super.Show();
		SetUpdate( true );
	}

	//============================================
	// Hide 
	//============================================
	override void Hide()
	{
		super.Hide();
		SetUpdate(false);
	}

	//============================================
	// SetUpdate
	//============================================
	override void SetUpdate(bool state)
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		
		PluginDeveloperSync developerSync = PluginDeveloperSync.Cast(GetPlugin(PluginDeveloperSync));
		
		//if client, send RPC
		if ( g_Game.IsClient() )
		{
			Param1<bool> params = new Param1<bool>(state);
			if (player)
			{
				player.RPCSingleParam(ERPCs.DEV_STATS_UPDATE, params, true);
				SetRPCSent();
			}
		}
		//else set directly
		else
		{
			if (developerSync)
			{
				developerSync.EnableUpdate(state, ERPCs.DEV_STATS_UPDATE, player);	
			}
		}
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		if (m_IsDragging)
		{
			int x,y;
			GetMousePos(x,y);
			m_WgtRoot.SetPos(x + m_DraggingOffset[0], y + m_DraggingOffset[1]);
		}
		else
		{
			if (!m_Populated)
				SetupValues();
			
			UpdateValues();
		}
	}
	
	void SetupValues()
	{
		PluginDeveloperSync developerSync = PluginDeveloperSync.Cast(GetPlugin(PluginDeveloperSync));
		
		int valuesCount = developerSync.m_PlayerStatsSynced.Count();
		if (valuesCount > 0)
		{
			foreach (SyncedValue syncedValue : developerSync.m_PlayerStatsSynced)
			{
				AddValue(syncedValue.GetName(), syncedValue.GetValue().ToString());
			}

			m_Populated = true;
		}
	}
	
	void UpdateValues()
	{
		PluginDeveloperSync developerSync = PluginDeveloperSync.Cast(GetPlugin(PluginDeveloperSync));
		int valuesCount = developerSync.m_PlayerStatsSynced.Count();
		if (valuesCount > 0)
		{
			foreach (int i, SyncedValue syncedValue : developerSync.m_PlayerStatsSynced)
			{
				string statName = syncedValue.GetName();
				float valueNormalized = syncedValue.GetValueNorm();
				float value = syncedValue.GetValue();
				
				if ( statName == "BloodType" )
				{
					string type, name;
					bool positive;
					
					name = value.ToString();
					name += "("+BloodTypes.GetBloodTypeName(Math.Round(value), type, positive)+")";
					m_StatValues.Get(i).SetText(name);
				}
				else
				{
					if (statName == "HeatBuffer")
					{
						float heatBufferNormalized = Math.Round(Math.Lerp(-1, 1, valueNormalized) * 1000) * 0.001;
						m_StatValues.Get(i).SetText(string.Format("%1 (%2)", heatBufferNormalized.ToString(), value.ToString()));
					}
					else
						m_StatValues.Get(i).SetText(value.ToString());
					
				}

				if (!m_ChangingSlider)
					m_SliderWidgets.GetKeyByValue(statName).SetCurrent(valueNormalized * 100);
			}
		}
	}
	
	void AddValue(string title, string value)
	{
		Widget widget = g_Game.GetWorkspace().CreateWidgets( "gui/layouts/debug/day_z_hud_debug_stat.layout", m_WgtPanel );
		
		TextWidget tw = TextWidget.Cast(widget.FindAnyWidget("StatName"));
		tw.SetText(title);
		m_StatWidgets.Insert(widget);
		
		TextWidget tw_output = TextWidget.Cast(widget.FindAnyWidget("OutputValue"));
		m_StatValues.Insert(tw_output);
		
		EditBoxWidget ebw_input = EditBoxWidget.Cast(widget.FindAnyWidget("InputValue"));
		m_StatValuesInput.Insert(ebw_input, title);
		
		SliderWidget sw = SliderWidget.Cast(widget.FindAnyWidget("StatSlider"));
		m_SliderWidgets.Insert(sw,title);
	}

	void ClearValues()
	{
		m_StatWidgets.Clear();
	}

	bool OnClick(Widget w, int x, int y, int button)
	{
		SetDragging(false);
		
		if (w.GetName() == "ResetStats")
		{
			ResetStats();
			return true;
		}

		return false;
	}
	
	bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (m_StatValuesInput.Contains(EditBoxWidget.Cast(w)) && finished)
		{
			EditBoxWidget ebw = EditBoxWidget.Cast(w);
			RPCChangeStat(m_StatValuesInput.Get(EditBoxWidget.Cast(w)), ebw.GetText().ToFloat());
			return true;
		}
		if (m_SliderWidgets.Contains(SliderWidget.Cast(w)))
		{
			m_ChangingSlider = true;
			string stat_name = m_SliderWidgets.Get(SliderWidget.Cast(w));
			SliderWidget sw = SliderWidget.Cast(w);
			PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
			for (int i = 0; i < player.m_PlayerStats.GetPCO().Get().Count(); i++) 
			{
				string label = player.m_PlayerStats.GetPCO().Get().Get(i).GetLabel();
				if(label == stat_name)
				{
					float statMin = player.m_PlayerStats.GetPCO().Get().Get(i).GetMin();
					float statMax = player.m_PlayerStats.GetPCO().Get().Get(i).GetMax();
					float current_value_norm = sw.GetCurrent() / 100;
					float current_value_abs = statMin + (statMax - statMin) * current_value_norm;
					
					RPCChangeStat(label, current_value_abs);
				}
			}
			
			if(finished)
				m_ChangingSlider = false;
		}
		
		return false;
	}
	
	
	void ResetStats()
	{
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		
		//if client, send RPC
		Param1<bool> params = new Param1<bool>(false);
		if (player)
		{
			player.RPCSingleParam(ERPCs.DEV_RPC_STATS_RESET, params, true);
		}
	}
	
	void RPCChangeStat(string stat, float value)
	{
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		
		//if client, send RPC
		Param2<string, float> params = new Param2<string, float>(stat, value);
		if (player)
		{
			player.RPCSingleParam(ERPCs.DEV_RPC_STAT_SET, params, true);
		}
	}
	
	bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (button == 0 && (w == m_WgtRoot || w == m_WgtHeader))
		{
			SetDragging(true);
			return true;
		}
		
		return false;
	}
	
	bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (button == 0 && m_IsDragging)
		{
			SetDragging(false);
			return true;
		}
		
		return false;
	}
	
	override void OnDraggingEnd()
	{
		float wx, wy;
		m_WgtRoot.GetScreenPos(wx,wy);
		if (m_ConfigDebugProfile)
		{
			m_ConfigDebugProfile.SetCharacterStatsPos(Vector(wx,wy,0));
		}
	}
		
	[Obsolete("no replacement")]
	void FitWindow();	
}
