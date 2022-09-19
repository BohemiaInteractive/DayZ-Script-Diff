class ModsMenuDetailedEntry extends ScriptedWidgetEventHandler
{
	protected Widget			m_Root;
	protected Widget			m_Detail;
	
	//Header
	protected ImageWidget		m_IconSmall;
	protected ImageWidget		m_IconCollapse;
	protected TextWidget		m_Name;
	
	//Left Side Panel
	protected ImageWidget		m_IconBig;
	protected MultilineTextWidget		m_Author;
	protected TextWidget		m_Version;
	protected RichTextWidget	m_ActionWebsite;
	protected RichTextWidget	m_ActionPurchase;
	
	//Description Panel
	protected RichTextWidget	m_Description;

	protected ModInfo			m_Data;
	protected ModsMenuDetailed	m_ParentMenu;
	protected bool				m_IsOpen;

	void ModsMenuDetailedEntry(ModInfo data, Widget parent, ModsMenuDetailed parent_menu)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/mods_menu/mods_menu_detailed_entry.layout", parent);
		m_Detail = m_Root.FindAnyWidget("DetailContainer");
		
		m_IconSmall = ImageWidget.Cast(m_Root.FindAnyWidget("IconSmall"));
		m_IconCollapse = ImageWidget.Cast(m_Root.FindAnyWidget("collapse_button"));
		m_IconCollapse.LoadImageFile( 1, "set:dayz_gui image:icon_open" );
		m_Name = TextWidget.Cast(m_Root.FindAnyWidget("Name"));
		
		m_IconBig = ImageWidget.Cast(m_Root.FindAnyWidget("IconBig"));
		m_Author = MultilineTextWidget.Cast(m_Root.FindAnyWidget("Author"));
		m_Author.SetLineBreakingOverride(LinebreakOverrideMode.LINEBREAK_WESTERN);
		
		m_Version = TextWidget.Cast(m_Root.FindAnyWidget("Version"));
		m_ActionWebsite = RichTextWidget.Cast(m_Root.FindAnyWidget("Link"));
		m_ActionPurchase = RichTextWidget.Cast(m_Root.FindAnyWidget("Purchase"));
		
		m_Description = RichTextWidget.Cast(m_Root.FindAnyWidget("Description"));
		
		m_Data = data;
		m_ParentMenu = parent_menu;
		
		m_Root.SetHandler(this);

		LoadData();
	}
	
	void ~ModsMenuDetailedEntry()
	{
		delete m_Root;
	}
	
	Widget GetWidget()
	{
		return m_Root;
	}
	
	void Select()
	{
		m_Root.SetColor( ARGBF( m_Root.GetAlpha(), 0.3, 0, 0 ) );
		
		m_Detail.Show(true);
		m_IconBig.Show( true );
		m_IconSmall.Show( false );
		m_IconCollapse.SetImage( 1 );
		m_Detail.Update();
		m_Root.Update();
		m_IsOpen = true;
	}
	
	void Deselect()
	{
		m_Root.SetColor( ARGBF( m_Root.GetAlpha(), 0.2, 0.2, 0.2 ) );
		
		m_Detail.Show(false);
		m_IconBig.Show( false );
		m_IconSmall.Show( true );
		m_IconCollapse.SetImage( 0 );
		m_Detail.Update();
		m_Root.Update();
		m_IsOpen = false;
	}

	void LoadData()
	{
		string picture = m_Data.GetPicture();
		string logo = m_Data.GetLogoSmall();
		string name = m_Data.GetName();
		string description = m_Data.GetOverview();
		string author = m_Data.GetAuthor();
		string version = m_Data.GetVersion();
		string action = m_Data.GetAction();

		//Load Large Icon
		if (picture != "")
		{
			m_IconBig.LoadImageFile(0, picture);
		}
		else if (logo != "")
		{
			m_IconBig.LoadImageFile(0, logo);
		}
		
		//Load Small Icon
		if (logo != "")
		{
			m_IconSmall.LoadImageFile(0, logo);
		}
		else if (picture != "")
		{
			m_IconSmall.LoadImageFile(0, picture);
		}

		if (name != "")
		{
			m_Name.SetText(name);
		}

		if (description != "")
		{
			m_Description.SetText(description);
			m_Description.Update();
			m_Detail.Update();
		}
		
		if (author != "")
		{
			m_Author.Show( true );
			m_Author.SetText(author);
		}
		
		if (version != "")
		{
			m_Version.Show( true );
			m_Version.SetText(version);
		}
		
		#ifdef PLATFORM_WINDOWS
		if (action != "")
		{
			m_ActionWebsite.Show( true );
		}
		#endif
		
		if ( m_Data.GetIsDLC() )
		{
			bool isOwned = m_Data.GetIsOwned();
			m_Root.FindAnyWidget("ModOwnership").Show( true );
			m_Root.FindAnyWidget("Owned").Show( isOwned );
			m_Root.FindAnyWidget("Unowned").Show( !isOwned );
			m_ActionPurchase.Show( true );
			m_Version.Show( false );
		}
	}
	
	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if( w == m_IconCollapse )
		{
			m_ParentMenu.Select( m_Data, !m_IsOpen );
			return true;
		}
		else if( w == m_ActionWebsite )
		{
			GetGame().OpenURL( m_Data.GetAction() );
		}
		else if( w == m_ActionPurchase )
		{
			m_Data.GoToStore();
		}
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if( w == m_ActionWebsite )
		{
			m_ActionWebsite.SetBold( true );
			m_ActionWebsite.SetText( "#mod_detail_info_website" );
		}
		
		if( w == m_ActionPurchase )
		{
			m_ActionPurchase.SetBold( true );
			m_ActionPurchase.SetText( "#mod_detail_info_store" );
		}
		
		if( w == m_Root )
		{
			if( m_Data.GetTooltip() != "" )
				m_ParentMenu.PrepareTooltip( m_Data );
			return true;
		}
		
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if( w == m_ActionWebsite )
		{
			m_ActionWebsite.SetBold( false );
			m_ActionWebsite.SetText( "#mod_detail_info_website" );
		}
		
		if( w == m_ActionPurchase )
		{
			m_ActionPurchase.SetBold( false );
			m_ActionPurchase.SetText( "#mod_detail_info_store" );
		}
		
		if( enterW != m_Root )
		{
			m_ParentMenu.HideTooltip();
			return true;
		}
		
		return false;
	}
}