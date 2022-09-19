class IntroSceneCharacter extends Managed
{
	protected int			m_CharacterId;
	protected string		m_CharacterType;
	protected MenuData		m_CharacterDta;
	protected PlayerBase	m_CharacterObj;
	protected vector		m_CharacterPos;
	protected vector		m_CharacterRot;
		
	protected ref TStringArray 	m_CharGenderList				= new TStringArray;
	protected ref TStringArray	m_CharShirtList					= new TStringArray; //legacy
	protected ref TStringArray 	m_CharPantsList					= new TStringArray; //legacy
	protected ref TStringArray	m_CharShoesList					= new TStringArray; //legacy
	
	protected ref map<ECharGender, ref array<string>> m_Characters = new map<ECharGender, ref array<string>>;
	
	protected ECharGender		m_CharGender;
	
	void IntroSceneCharacter()
	{
		m_CharacterId = GameConstants.DEFAULT_CHARACTER_MENU_ID;
	}
	
	void ~IntroSceneCharacter()
	{
		CharacterUnload();
	}
	
	bool IsDefaultCharacter()
	{
		return m_CharacterId == GameConstants.DEFAULT_CHARACTER_MENU_ID;
	}
	
	void SetToDefaultCharacter()
	{
		m_CharacterId = GameConstants.DEFAULT_CHARACTER_MENU_ID;
	}
	//==============================================
	// SetcharacterID
	//==============================================
	void SetCharacterID(int char_id)
	{
		m_CharacterId = char_id;
	}
	
	//==============================================
	// GetCharacterID
	//==============================================
	int GetCharacterID()
	{
		return m_CharacterId;
	}
	
	//==============================================
	// GetCharacterObj
	//==============================================
	PlayerBase GetCharacterObj()
	{
		return m_CharacterObj;
	}
	
	//==============================================
	// GetCharGenderList	
	//==============================================
	TStringArray GetCharGenderList()
	{
		return m_CharGenderList;
	}
	
	//==============================================
	// GetCharGenderList	
	//==============================================
	TStringArray GetCharList(ECharGender gender)
	{
		return m_Characters[gender];
	}
	
	//==============================================
	// GetCharShirtsList
	//==============================================
	TStringArray GetCharShirtsList()
	{
		return m_CharShirtList;
	}
	
	//==============================================
	// GetCharPantsList
	//==============================================
	TStringArray GetCharPantsList()
	{
		return m_CharPantsList;
	}
	
	//==============================================
	// GetCharShoesList
	//==============================================
	TStringArray GetCharShoesList()
	{
		return m_CharShoesList;
	}
	
	//==============================================
	// SetCharacterGender
	//==============================================
	void SetCharacterGender(ECharGender gender)
	{
		m_CharGender = gender;
	}
	
	//==============================================
	// IsCharacterFemale
	//==============================================
	bool IsCharacterFemale()
	{
		return ( m_CharGender == ECharGender.Female );
	}
	
	//==============================================
	// SetCharacterGender
	//==============================================
	ECharGender GetCharacterGender()
	{
		return m_CharGender;
	}
	
	//==============================================
	// GetPosition
	//==============================================
	vector GetPosition()
	{
		return m_CharacterObj.GetPosition();
	}
	
	//==============================================
	// GetNextCharacterID
	//==============================================
	int GetNextCharacterID()
	{
		int count = m_CharacterDta.GetCharactersCount();
		
		if ( count == 0 )
		{	
			return -1;
		}
		
		if ( m_CharacterId + 1 < count )
		{
			return m_CharacterId + 1;
		}
		else
		{
			return -1;
		}
	}
	
	//==============================================
	// GetPrevCharacterID
	//==============================================
	int GetPrevCharacterID()
	{
		int count = m_CharacterDta.GetCharactersCount();
		
		if ( count == 0 )
		{
			return -1;
		}
		
		if ( m_CharacterId > -1 )
		{
			return m_CharacterId - 1;
		}
		
		return count - 1;
	}
	
	//==============================================
	// CreateNewCharacterRandom
	//==============================================
	void CreateNewCharacterRandom()
	{
		// Select random gender
		SetCharacterGender( Math.RandomInt(0, 2) );
		
		// Select random character skin (class name)
		string char_name_random = m_Characters[GetCharacterGender()].GetRandomElement();
		
		// Create new character
		CreateNewCharacterByName( char_name_random );
	}
	
	//==============================================
	// CreateNewCharacterById
	//==============================================
	void CreateNewCharacterById( int character_id )
	{
		if ( character_id == GameConstants.DEFAULT_CHARACTER_MENU_ID )
		{
			CreateDefaultCharacter();
		}
		else
		{
			CharacterLoad( character_id, m_CharacterPos, m_CharacterRot );
		}
	}
	
	//==============================================
	// CreateNewCharacterByName
	//==============================================
	void CreateNewCharacterByName( string character_name, bool randomize_equip = true )
	{
		m_CharacterType = character_name;
		GetGame().GetMenuDefaultCharacterData().SetCharacterType(m_CharacterType);
		if (randomize_equip)
			GetGame().GetMenuDefaultCharacterData().GenerateRandomEquip();
		
		CreateNewCharacter();
	}
	
	void CreateDefaultCharacter()
	{
		CharacterUnload();
		//m_CharacterDta.RequestGetDefaultCharacterData();
		m_CharacterType = GetGame().GetMenuDefaultCharacterData().GetCharacterType();
		if (m_CharacterType != "")
		{
			CreateNewCharacter();
		}
		
		if(m_CharacterObj)
		{
			m_CharacterObj.PlaceOnSurface();
			m_CharacterObj.SetPosition(m_CharacterPos);
			m_CharacterObj.SetOrientation(m_CharacterRot);
		}
		else
		{
			string default_name = Widget.TranslateString( GameConstants.DEFAULT_CHARACTER_NAME );
			CreateNewCharacterRandom();
			//m_CharacterDta.SetCharacterName(GameConstants.DEFAULT_CHARACTER_MENU_ID, default_name);
			GetGame().GetMenuDefaultCharacterData().SetCharacterName(GameConstants.DEFAULT_CHARACTER_NAME);
		}
	}
	
	void GetLastPlayedServer(int characterID, out string address, out string name, out int port)
	{
		m_CharacterDta.GetLastServerAddress(characterID,address);
		port = m_CharacterDta.GetLastServerPort(characterID);
		m_CharacterDta.GetLastServerName(characterID,name);
	}
	//==============================================
	// CreateNewCharacter
	//==============================================
	void CreateNewCharacter()
	{
		// Unload old character (if exist)
		CharacterUnload();
		
		// Create Character
		m_CharacterObj = PlayerBase.Cast(g_Game.CreateObjectEx(m_CharacterType, m_CharacterPos, ECE_PLACE_ON_SURFACE));
		
		if (m_CharacterObj)
		{
			m_CharacterObj.PlaceOnSurface();
			m_CharacterObj.SetOrientation(m_CharacterRot);
			GetGame().GetMenuDefaultCharacterData().EquipDefaultCharacter(m_CharacterObj);
		}
		
		//Create New Random Character
		SetupPlayerName( true );
	}
	
	//! Generates random equip for the new IntroSceneCharacter, whatever is defined in 'cfgCharacterCreation'
	/*void EquipNewCharacter()
	{
		int slot_ID;
		string attachment_type;
		for (int i = 0; i < DefaultCharacterCreationMethods.GetAttachmentSlotsArray().Count(); i++)
		{
			slot_ID = DefaultCharacterCreationMethods.GetAttachmentSlotsArray().Get(i);
			if (DefaultCharacterCreationMethods.GetConfigArrayCountFromSlotID(slot_ID) > 0)
			{
				attachment_type = DefaultCharacterCreationMethods.GetConfigAttachmentTypes(slot_ID).GetRandomElement();
				if (attachment_type != "")
					SetAttachment(attachment_type,slot_ID);
			}
		}
	}*/
	
	//==============================================
	// LoadCharacterData
	//==============================================
	void LoadCharacterData(vector char_pos, vector char_rot, bool default_char = false)
	{
		m_CharacterDta = g_Game.GetMenuData();
		m_CharacterPos = char_pos;
		m_CharacterRot = char_rot;
		
		if (!default_char && m_CharacterDta.GetLastPlayedCharacter() > -1 )
		{
			m_CharacterId = m_CharacterDta.GetLastPlayedCharacter();
			m_CharacterDta.GetCharacterName(m_CharacterId, g_Game.GetPlayerGameName());
		}
		
		// Load all avalible options for character creation; mostly legacy stuff
		g_Game.ConfigGetTextArray("cfgCharacterCreation" + " gender",	m_CharGenderList);
		g_Game.ConfigGetTextArray("cfgCharacterCreation" + " top",		m_CharShirtList);
		g_Game.ConfigGetTextArray("cfgCharacterCreation" + " bottom",	m_CharPantsList);
		g_Game.ConfigGetTextArray("cfgCharacterCreation" + " shoe",		m_CharShoesList);

		// Init character table
		m_Characters.Clear();
		m_Characters.Insert( ECharGender.Male,		new array<string> );
		m_Characters.Insert( ECharGender.Female,	new array<string> );
		
		// Sort character by Gender
		TStringArray characters = GetGame().ListAvailableCharacters();
		for (int i = 0; i < characters.Count(); i++)
		{
			string char_cfg_name = characters.Get(i);
			if ( GetGame().IsKindOf(char_cfg_name, "SurvivorMale_Base") )
			{
				m_Characters[ECharGender.Male].Insert( char_cfg_name );
			}
			else
			{
				m_Characters[ECharGender.Female].Insert( char_cfg_name );
			}
		}
		
		CreateNewCharacterById(m_CharacterId/*, m_CharacterPos, m_CharacterRot*/);
		
		if (GetCharacterObj() )
		{
			if ( GetCharacterObj().IsMale() )
				SetCharacterGender(ECharGender.Male);
			else
				SetCharacterGender(ECharGender.Female);
		}
	}
	
	//-------------------------
	// CharacterUnload
	//-------------------------
	protected void CharacterUnload()
	{
		if ( m_CharacterObj )
		{
			g_Game.ObjectDelete(m_CharacterObj);
			m_CharacterObj = NULL;
		}
	}

	//-------------------------
	// CharacterLoad
	//-------------------------
	protected void CharacterLoad( int character_id, vector char_pos, vector char_rot )
	{
		if ( character_id == -1 )
		{
			Error("IntroSceneCharacter->CharacterLoad: character_id = "+ character_id +" Cant Load Character!!!");
			return;
		}
		
		CharacterUnload();
		
		SetCharacterID( character_id );
				
		m_CharacterObj = PlayerBase.Cast( m_CharacterDta.CreateCharacterPerson( character_id ) );
		
		if ( m_CharacterObj )
		{
			
			m_CharacterObj.PlaceOnSurface();
			m_CharacterObj.SetPosition(char_pos);
			m_CharacterObj.SetOrientation(char_rot);	
		}	
	}
	
	//-------------------------
	// SetupPlayerName
	//-------------------------
	protected void SetupPlayerName( bool new_name )
	{
		string name = GameConstants.DEFAULT_CHARACTER_NAME;		
		
#ifdef PLATFORM_CONSOLE
		BiosUserManager user_manager = GetGame().GetUserManager();
		if( user_manager )
		{
			BiosUser user = user_manager.GetSelectedUser();
			if( user )
			{
				name = user.GetName();
			}
		}
#else
		if ( !new_name )
		{
			m_CharacterDta.GetCharacterName(m_CharacterId, name);
		}
#endif
		
		g_Game.SetPlayerGameName(name);
	}
	
	//==============================================
	// SetAttachment
	//==============================================
	void SetAttachment(string type, int slot)
	{
		if ( !m_CharacterObj )
		{
			return;
		}
		
		g_Game.ObjectDelete(m_CharacterObj.GetInventory().FindAttachment(slot));
		
		EntityAI entity = EntityAI.Cast( g_Game.CreateObjectEx(type, m_CharacterObj.GetPosition(), ECE_PLACE_ON_SURFACE) );
		
		if (entity)
		{
			m_CharacterObj.LocalTakeEntityAsAttachmentEx(entity, slot);
		}
	}
	
	string GetCharacterNameById(int char_id )
	{
		string character_name;
		if (char_id == GameConstants.DEFAULT_CHARACTER_MENU_ID)
		{
			character_name = GetGame().GetMenuDefaultCharacterData().GetCharacterName();
		}
		else
		{
			m_CharacterDta.GetCharacterName(char_id, character_name);
		}
		return character_name;
	}
	
	string GetCharacterName()
	{
		string character_name;
		if (IsDefaultCharacter())
		{
			character_name = GetGame().GetMenuDefaultCharacterData().GetCharacterName();
		}
		else
		{
			m_CharacterDta.GetCharacterName(m_CharacterId, character_name);
		}
		return character_name;
	}
	
	//==============================================
	// SaveCharName
	//==============================================
	void SaveCharName( string name )
	{
		if (IsDefaultCharacter())
		{
			GetGame().GetMenuDefaultCharacterData().SetCharacterName(name);
		}
		else
		{
			m_CharacterDta.SetCharacterName(m_CharacterId, name);
		}
		m_CharacterDta.SaveCharactersLocal();
	}
	
	//==============================================
	// SaveDefaultCharacter
	//==============================================
	void SaveDefaultCharacter()
	{
		//if (m_CharacterObj)
		{
			m_CharacterDta.RequestSetDefaultCharacterData();
		}
	}
}
