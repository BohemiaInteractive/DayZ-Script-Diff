/*
	Ui class for hints in in-game-menu
*/
class UiHintPanel extends ScriptedWidgetEventHandler
{
	// Const
	private const int 				m_SlideShowDelay	= 25000;												// The speed of the slideshow 
	private const string 			m_RootPath			= "Gui/layouts/new_ui/hints/in_game_hints.layout";		// Layout path 
	private const string 			m_DataPath			= "Scripts/data/hints.json";							// Json path
	// Widgets
	private Widget 					m_RootFrame;
	private Widget 					m_SpacerFrame;
	private ButtonWidget 			m_UiLeftButton;
	private ButtonWidget 			m_UiRightButton;
	private RichTextWidget			m_UiDescLabel;
	private TextWidget				m_UiHeadlineLabel;
	private ImageWidget 			m_UiHintImage;
	private TextWidget 				m_UiPageingLabel;
	// Data		
	private ref array<ref HintPage>	m_ContentList;
	private int 					m_PageIndex;
	
	// ---------------------------------------------------------
	
	// Constructor
	void UiHintPanel(Widget parent_widget)
	{		
		// Load Json File 
		LoadContentList();
		// If load successful 
		if (m_ContentList)	
		{
			// Build the layout
			BuildLayout(parent_widget);
			// Get random page index 
			RandomizePageIndex();
			// Populate the layout with data
			PopulateLayout();
			// Start the slideshow
			StartSlideshow();			
		}
		else 
		{
			Print("ERROR: UiHintPanel - Could not create the hint panel. The data are missing!");
		}
	}
	// Destructor
	void ~UiHintPanel()
	{
		StopSlideShow();
	}
	
	// ------------------------------------------------------
	
	// Load content data from json file 
	private void LoadContentList()
	{
		JsonFileLoader<array<ref HintPage>>.JsonLoadFile( m_DataPath, m_ContentList );
	}	
	
	// Create and Build the layout 
	private void BuildLayout(Widget parent_widget)
	{
		// Create the layout
		m_RootFrame = GetGame().GetWorkspace().CreateWidgets( m_RootPath, parent_widget );
		
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
	private void PopulateLayout()
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
	private void SetHintHeadline()
	{
		m_UiHeadlineLabel.SetText(m_ContentList.Get(m_PageIndex).GetHeadlineText());
	}
	private void SetHintDescription()
	{
		m_UiDescLabel.SetText(m_ContentList.Get(m_PageIndex).GetDescriptionText());
		m_UiDescLabel.Update();
		m_SpacerFrame.Update();
	}
	private void SetHintImage()
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
	private void SetHintPaging()
	{
		m_UiPageingLabel.SetText(string.Format("%1 / %2", m_PageIndex + 1, m_ContentList.Count()));	
	}
	// Set a random page index 
	private void RandomizePageIndex()
	{
		m_PageIndex = Math.RandomInt(0, m_ContentList.Count() - 1);	
	}
	// Show next hint page by incrementing the page index. 
	private void ShowNextPage()
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
	private void ShowPreviousPage()
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
	private void StartSlideshow()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(SlideshowThread, m_SlideShowDelay);
	}
	// Slidshow thread - run code
	private void SlideshowThread()
	{
		ShowNextPage();
	}
	// Stop the slide show
	private void StopSlideShow()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(SlideshowThread);
	}
	// Restart the slide show 
	private void RestartSlideShow()
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