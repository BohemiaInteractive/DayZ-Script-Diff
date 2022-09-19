class PluginConfigEmotesProfile extends PluginConfigHandler
{	
	/*protected const string EMOTE_1				= "emote_slot_1";
	protected const string EMOTE_2				= "emote_slot_2";
	protected const string EMOTE_3				= "emote_slot_3";
	protected const string EMOTE_4				= "emote_slot_4";
	protected const string EMOTE_5				= "emote_slot_5";
	protected const string EMOTE_6				= "emote_slot_6";
	protected const string EMOTE_7				= "emote_slot_7";
	protected const string EMOTE_8				= "emote_slot_8";
	protected const string EMOTE_9				= "emote_slot_9";
	protected const string EMOTE_10				= "emote_slot_10";
	protected const string EMOTE_11				= "emote_slot_11";
	protected const string EMOTE_12				= "emote_slot_12";*/

	protected ref map<string, ref CfgParam>		m_DefaultValues;
	protected ref TStringArray 					m_PresetList;	
	
	//========================================
	// GetInstance
	//========================================
	static PluginConfigEmotesProfile GetInstance()
	{
		return PluginConfigEmotesProfile.Cast( GetPlugin(PluginConfigEmotesProfile) );
	}	
	

	//========================================
	// GetCfgParamInt
	//========================================
	protected CfgParamInt GetNewCfgParamInt( int value )
	{
		CfgParamInt param = new CfgParamInt( "" );
		param.SetValue( value );
		return param;
	}
	
	//========================================
	// Int
	//========================================
	protected CfgParamInt SetInt( string key, int value )
	{
		CfgParamInt param = CfgParamInt.Cast( GetParamByName( key, CFG_TYPE_INT ) );
		param.SetValue( value );
		//SaveConfigToFile();
		return param;
	}

	protected int GetInt( string key )
	{
		if ( ParamExist(key) )
		{
			CfgParamInt param = CfgParamInt.Cast( GetParamByName( key, CFG_TYPE_INT ) );
			return param.GetValue();
		}
		else
		{
			if ( m_DefaultValues.Contains(key) )
			{
				CfgParamInt default_param = CfgParamInt.Cast( m_DefaultValues.Get( key ) );
				return SetInt( key, default_param.GetValue() ).GetValue();
			}
			else
			{
				return SetInt( key, 0 ).GetValue();
			}
		}
	}
	
	//========================================
	// PluginConfigEmotesProfile
	//========================================
	void PluginConfigEmotesProfile()
	{
		m_DefaultValues = new map<string, ref CfgParam>;
		//Here you can configure default EMOTES

		m_DefaultValues.Insert( EmoteSlots.EMOTE_1.ToString(),  GetNewCfgParamInt(EmoteConstants.ID_EMOTE_GREETING) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_2.ToString(),  GetNewCfgParamInt(EmoteConstants.ID_EMOTE_SOS) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_3.ToString(),  GetNewCfgParamInt(EmoteConstants.ID_EMOTE_HEART) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_4.ToString(),  GetNewCfgParamInt(EmoteConstants.ID_EMOTE_TAUNT) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_5.ToString(),  GetNewCfgParamInt(EmoteConstants.ID_EMOTE_SURRENDER) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_6.ToString(),  GetNewCfgParamInt(EmoteConstants.ID_EMOTE_TAUNTKISS) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_7.ToString(),  GetNewCfgParamInt(EmoteConstants.ID_EMOTE_POINT) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_8.ToString(),  GetNewCfgParamInt(EmoteConstants.ID_EMOTE_TAUNTELBOW) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_9.ToString(),  GetNewCfgParamInt(EmoteConstants.ID_EMOTE_THUMB) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_10.ToString(), GetNewCfgParamInt(EmoteConstants.ID_EMOTE_THROAT) );
		m_DefaultValues.Insert( EmoteSlots.EMOTE_11.ToString(), GetNewCfgParamInt(EmoteConstants.ID_EMOTE_SUICIDE) );
		//m_DefaultValues.Insert( EMOTE_12, GetNewCfgParamInt(EmoteConstants.ID_EMOTE_DANCE) );
		//m_DefaultValues.Insert( EMOTE_12, GetNewCfgParamInt(EmoteConstants.ID_EMOTE_SOS) );
	}
	
	//========================================
	// GetFileName
	//========================================
	override string GetFileName()
	{
		return CFG_FILE_EMOTES_PROFILE;
	}

	//========================================
	// EmoteStoring
	//========================================		
	int GetEmoteSlot1()
	{
		return GetInt( EmoteSlots.EMOTE_1.ToString() );
	}

	void SetEmoteSlot1( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_1.ToString(), emoted_id );
	}
	
	int GetEmoteSlot2()
	{
		return GetInt( EmoteSlots.EMOTE_2.ToString() );
	}

	void SetEmoteSlot2( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_2.ToString(), emoted_id );
	}
	
	int GetEmoteSlot3()
	{
		return GetInt( EmoteSlots.EMOTE_3.ToString() );
	}

	void SetEmoteSlot3( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_3.ToString(), emoted_id );
	}
	
	int GetEmoteSlot4()
	{
		return GetInt( EmoteSlots.EMOTE_4.ToString() );
	}

	void SetEmoteSlot4( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_4.ToString(), emoted_id );
	}

	int GetEmoteSlot5()
	{
		return GetInt( EmoteSlots.EMOTE_5.ToString() );
	}

	void SetEmoteSlot5( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_5.ToString(), emoted_id );
	}
	
	int GetEmoteSlot6()
	{
		return GetInt( EmoteSlots.EMOTE_6.ToString() );
	}

	void SetEmoteSlot6( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_6.ToString(), emoted_id );
	}
	
	int GetEmoteSlot7()
	{
		return GetInt( EmoteSlots.EMOTE_7.ToString() );
	}

	void SetEmoteSlot7( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_7.ToString(), emoted_id );
	}
	
	int GetEmoteSlot8()
	{
		return GetInt( EmoteSlots.EMOTE_8.ToString() );
	}

	void SetEmoteSlot8( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_8.ToString(), emoted_id );
	}
	
	int GetEmoteSlot9()
	{
		return GetInt( EmoteSlots.EMOTE_9.ToString() );
	}

	void SetEmoteSlot9( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_9.ToString(), emoted_id );
	}
	
	int GetEmoteSlot10()
	{
		return GetInt( EmoteSlots.EMOTE_10.ToString() );
	}

	void SetEmoteSlot10( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_10.ToString(), emoted_id );
	}
	
	int GetEmoteSlot11()
	{
		return GetInt( EmoteSlots.EMOTE_11.ToString() );
	}

	void SetEmoteSlot11( int emoted_id )
	{
		SetInt( EmoteSlots.EMOTE_11.ToString(), emoted_id );
	}
	
	/*int GetEmoteSlot12()
	{
		return GetInt( EMOTE_12 );
	}

	void SetEmoteSlot12( int emoted_id )
	{
		SetInt( EMOTE_12, emoted_id );
	}*/
	
	int GetEmoteFromSlot(string slot)
	{
		return GetInt( slot );
	}
}

enum EmoteSlots
{
	EMOTE_1 = 1,
	EMOTE_2,
	EMOTE_3,
	EMOTE_4,
	EMOTE_5,
	EMOTE_6,
	EMOTE_7,
	EMOTE_8,
	EMOTE_9,
	EMOTE_10,
	EMOTE_11,
	EMOTE_12
}
