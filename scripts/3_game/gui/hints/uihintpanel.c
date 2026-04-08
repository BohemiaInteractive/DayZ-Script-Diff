/*
	Ui class for hints in in-game-menu
*/

class UiHintPanel extends ScriptedWidgetEventHandler
{	
	#ifdef DIAG_DEVELOPER
	static int				m_ForcedIndex = -1;//only for debug purposes
	#endif
	
	// Const
	protected int 			m_SlideShowDelay			= 25000;											// The speed of the slideshow 
	protected string 				m_RootPath			= "Gui/layouts/new_ui/hints/in_game_hints.layout";	// Layout path 
	protected const string 			m_DataPath			= "scripts/data/hints.json";						// Json path
	// Widgets
	protected Widget 				m_RootFrame;
	protected Widget 				m_SpacerFrame;
	protected ButtonWidget 			m_UiLeftButton;
	protected ButtonWidget 			m_UiRightButton;
	protected RichTextWidget		m_UiDescLabel;
	protected TextWidget			m_UiHeadlineLabel;
	protected ImageWidget 			m_UiHintImage;
	protected TextWidget 			m_UiPageingLabel;
	// Data		
	protected ref array<ref HintPage>	m_ContentList;
	protected int 					m_PageIndex = int.MIN;
	protected DayZGame				m_Game;
	protected bool					m_Initialized;
	protected Widget 				m_ParentWidget;
	protected int					m_PreviousRandomIndex = int.MIN;
	
	// ---------------------------------------------------------
	
	// Constructor
	void UiHintPanel(Widget parent_widget)
	{		
		DayZGame game = DayZGame.Cast(g_Game);
		m_ParentWidget = parent_widget;
		Init(game);
	}
	// Destructor
	void ~UiHintPanel()
	{
		StopSlideShow();
	
		if(m_RootFrame)
			m_RootFrame.Unlink();
	}

	
	void Init(DayZGame game)
	{
		//as this class is now also being instantiated from within the DayZGame CTOR, where g_Game does not work yet, we need a way to pass the game instance from DayZGame CTOR
		//however for modding legacy support purposes, this was done without modifying the CTOR signature with the addition of the Init method, 
		//in order to keep compatibility with existing MODs, there is still a way to instantiate this class properly even without calling Init from the outside
		
		if (m_Initialized)
			return;
		if (!game)//is null when instantiated from DayZGame during loading before calling Init explicitly
			return;
		m_Initialized = true;

		m_Game = game;
		// Load Json File 
		LoadContentList();
		// If load successful 
		if (m_ContentList)	
		{
			// Build the layout
			BuildLayout(m_ParentWidget);
			// Get random page index 
			RandomizePageIndex();
			// Populate the layout with data
			PopulateLayout();
			// Start the slideshow
			StartSlideshow();			
		}
		else 
			ErrorEx("Could not create the hint panel. The data are missing!");
	}
	
	// ------------------------------------------------------
	
	// Load content data from json file 
	protected void LoadContentList()
	{
		string errorMessage;
		if (!JsonFileLoader<array<ref HintPage>>.LoadFile(m_DataPath, m_ContentList, errorMessage))
			ErrorEx(errorMessage);
	}	
	
	// Create and Build the layout 
	protected void BuildLayout(Widget parent_widget)
	{
		// Create the layout
		m_RootFrame = m_Game.GetWorkspace().CreateWidgets(m_RootPath, parent_widget);
		
		if (m_RootFrame)
		{
			// Find Widgets
			m_SpacerFrame		= m_RootFrame.FindAnyWidget("GridSpacerWidget1");		
			m_UiLeftButton		= ButtonWidget.Cast(m_RootFrame.FindAnyWidget("LeftButton"));		
			m_UiRightButton		= ButtonWidget.Cast(m_RootFrame.FindAnyWidget("RightButton"));
			m_UiHeadlineLabel	= TextWidget.Cast(m_RootFrame.FindAnyWidget("HeadlineLabel"));	
			m_UiDescLabel		= RichTextWidget.Cast(m_RootFrame.FindAnyWidget("HintDescLabel"));
			m_UiHintImage		= ImageWidget.Cast(m_RootFrame.FindAnyWidget("HintImage"));
			m_UiPageingLabel	= TextWidget.Cast(m_RootFrame.FindAnyWidget("PageInfoLabel"));
			// Set handler
			m_RootFrame.SetHandler(this);
		}
	}
	
	// Populate the hint with content
	protected void PopulateLayout()
	{
		if (m_RootFrame)
		{
			SetHintHeadline();
			SetHintDescription();
			SetHintImage();
			SetHintPaging();
		}
	}	
	
	// -------------------------------------------
	// Setters
	protected void SetHintHeadline()
	{
		m_UiHeadlineLabel.SetText(m_ContentList.Get(m_PageIndex).GetHeadlineText());
	}
	protected void SetHintDescription()
	{
		#ifdef DEVELOPER
		//Print("showing contents for page "+m_PageIndex);
		#endif
		m_UiDescLabel.SetText(m_ContentList.Get(m_PageIndex).GetDescriptionText());
		m_UiDescLabel.Update();
		m_SpacerFrame.Update();
	}
	protected void SetHintImage()
	{
		string image_path = m_ContentList.Get(m_PageIndex).GetImagePath();
		
		// If there is an image 
		if (image_path)
		{	
			// Show the widget
			m_UiHintImage.Show(true);
			// Set the image path
			m_UiHintImage.LoadImageFile(0, image_path);
		}
		else 
		{
			// Hide the widget
			m_UiHintImage.Show(false);
		}
	}
	protected void SetHintPaging()
	{
		if (m_UiPageingLabel)
			m_UiPageingLabel.SetText(string.Format("%1 / %2", m_PageIndex + 1, m_ContentList.Count()));	
	}
	
	void ShowRandomPage()
	{
		RandomizePageIndex();
		PopulateLayout();
	}
	
	// Set a random page index 
	protected void RandomizePageIndex()
	{
		#ifdef DIAG_DEVELOPER
		if (DiagMenu.IsInitialized())
		{
			if (m_ForcedIndex != -1)
			{
				m_PageIndex = Math.Clamp(m_ForcedIndex,0,m_ContentList.Count() - 1);
				return;	
			}
		}	
		#endif
		
		Math.Randomize(m_Game.GetTime());
		Math.RandomFloat01();//throw-away value, without calling this, the next random number is always the same, calling Math.Randomize(-1) makes no difference
		while (m_PageIndex == m_PreviousRandomIndex)
			m_PageIndex = Math.RandomIntInclusive(0, m_ContentList.Count() - 1);
		m_PreviousRandomIndex = m_PageIndex;
		
	}
	// Show next hint page by incrementing the page index. 
	protected void ShowNextPage()
	{
		// Update the page index
		if ( m_PageIndex < m_ContentList.Count() - 1 )
		{
			m_PageIndex++;
		}
		else 											
		{
			m_PageIndex = 0;
		}
		
		//Update the hint page
		PopulateLayout();
	}	
	// Show previous hint page by decreasing the page index. 
	protected void ShowPreviousPage()
	{
		// Update the page index
		if ( m_PageIndex == 0 )
		{
			m_PageIndex = m_ContentList.Count() - 1;
		}
		else 
		{
			m_PageIndex--;
		
		}
		//Update the hint page
		PopulateLayout();
	}

	// -------------------------------------------
	// Slideshow
	
	// Creates new slidshow thread
	protected void StartSlideshow()
	{
		m_Game.GetCallQueue(CALL_CATEGORY_GUI).CallLater(SlideshowThread, m_SlideShowDelay);
	}
	// Slidshow thread - run code
	protected void SlideshowThread()
	{
		ShowNextPage();
	}
	// Stop the slide show
	protected void StopSlideShow()
	{
		m_Game.GetCallQueue(CALL_CATEGORY_GUI).Remove(SlideshowThread);
	}
	// Restart the slide show 
	protected void RestartSlideShow()
	{
		StopSlideShow();
		StartSlideshow();
	}
	
	// ----------------------------------------
	// Layout manipulation
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button == MouseState.LEFT)
		{
			switch (w)
			{
				case m_UiLeftButton:
				{
					ShowPreviousPage();
					return true;
				}
				case m_UiRightButton:
				{	
					ShowNextPage();
					return true;
				}	
			}
		}
		return false;
	}
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w == m_RootPath || w == m_UiLeftButton || w == m_UiRightButton)
		{
			StopSlideShow();
			return true;
		}
		return false;
	}
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w == m_RootPath || w == m_UiLeftButton || w == m_UiRightButton)
		{
			RestartSlideShow();
			return true;
		}
		return false;
	}
}

// ---------------------------------------------------------------------------------------------------------
class UiHintPanelLoading extends UiHintPanel
{
	override void Init(DayZGame game)
	{
		m_RootPath = "Gui/layouts/new_ui/hints/in_game_hints_load.layout";
		super.Init(game);
	}
}
