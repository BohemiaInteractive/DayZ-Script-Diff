enum NewsCategory
{
	DLCS,
	DLC_PROMO,
	MISC_PROMO 
}

class NewsCarousel extends ScriptedWidgetEventHandler
{
	protected const int 		MAX_ARTICLES = 3;
	protected const float 		ARTICLE_SWITCH_TIME = 3.0;
	
	protected Widget 			m_Root;
	protected ButtonWidget		m_HotlinkButton;
	protected RichTextWidget 	m_HotlinkButtonLabel;
	protected ImageWidget 		m_Image;
	protected TextWidget 		m_TitleText;
	protected ButtonWidget 		m_NextPageBtn;
	protected ButtonWidget 		m_PrevPageBtn;
	protected ImageWidget 		m_NextPageBtnIcon;
	protected ImageWidget 		m_PrevPageBtnIcon;
	
	#ifdef PLATFORM_CONSOLE	
	protected ImageWidget 		m_NextPageBtnIconConsole;
	protected ImageWidget 		m_PrevPageBtnIconConsole;
	#endif

	protected ref array<JsonDataNewsArticle> m_NewsArticles;
	protected JsonDataNewsArticle m_CurrentArticle;
	protected ModInfo m_CurrentModInfo;
	protected int m_ArticleIndex;
	
	protected ref Timer m_NewsSwitchTimer;
	protected bool m_SwitchToNext = true;
	
	#ifdef PLATFORM_CONSOLE
	protected MainMenuConsole m_MainMenu;
	#else
	protected MainMenu m_MainMenu;
	#endif
	
	protected GridSpacerWidget m_GridSpacer;
	protected ref AnimatedGridSpacer m_AnimatedSpacer;
	
	protected EInputDeviceType m_CurrentInputDevice;
	
	void NewsCarousel(Widget parent, UIScriptedMenu mainMenu)
	{
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/news_carousel.layout", parent);
		
		m_TitleText = TextWidget.Cast(m_Root.FindAnyWidget("nf_title"));
		
		m_NextPageBtn = ButtonWidget.Cast(m_Root.FindAnyWidget("next_button"));
		m_PrevPageBtn = ButtonWidget.Cast(m_Root.FindAnyWidget("prev_button"));
		m_HotlinkButton = ButtonWidget.Cast(m_Root.FindAnyWidget("nf_hotlink_button"));
		m_HotlinkButtonLabel = RichTextWidget.Cast(m_Root.FindAnyWidget("nf_hotlink_label"));
		
		m_NextPageBtnIcon = ImageWidget.Cast(m_Root.FindAnyWidget("next_icon_pc"));
		m_PrevPageBtnIcon = ImageWidget.Cast(m_Root.FindAnyWidget("prev_icon_pc"));
		
		m_PrevPageBtnIcon.Show(false);
		#ifdef PLATFORM_CONSOLE
		
		m_NextPageBtnIconConsole = ImageWidget.Cast(m_Root.FindAnyWidget("next_icon_console"));
		m_PrevPageBtnIconConsole = ImageWidget.Cast(m_Root.FindAnyWidget("prev_icon_console"));
		
		#ifdef PLATFORM_XBOX
		m_NextPageBtnIconConsole.LoadImageFile(0, "set:xbox_buttons image:DPAD_right");
		m_PrevPageBtnIconConsole.LoadImageFile(0, "set:xbox_buttons image:DPAD_left");
		#else
		m_NextPageBtnIconConsole.LoadImageFile(0, "set:playstation_buttons image:DPAD_right");
		m_PrevPageBtnIconConsole.LoadImageFile(0, "set:playstation_buttons image:DPAD_left");
		#endif
		#endif
		
		#ifdef PLATFORM_CONSOLE
		m_MainMenu = MainMenuConsole.Cast(mainMenu);
		#else
		m_MainMenu = MainMenu.Cast(mainMenu);
		#endif
		
		m_GridSpacer = GridSpacerWidget.Cast(m_Root.FindAnyWidget("nf_grid"));		
		
		m_HotlinkButtonLabel.SetText(GetButtonLabel("255, 255, 255, 255"));
		
		m_Root.SetHandler(this);
		
		UpdateCarouselInfo();
		
		#ifdef PLATFORM_CONSOLE
		if (g_Game.GetMission())
		{
			g_Game.GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		}
		#endif
		
		OnInputDeviceChanged(g_Game.GetInput().GetCurrentInputDevice());
	}

	void ~NewsCarousel()
	{		
		if (m_NewsSwitchTimer)
			m_NewsSwitchTimer.Stop();
		
		if (g_Game.GetMission())
		{
			g_Game.GetMission().GetOnInputDeviceChanged().Remove(OnInputDeviceChanged);
		}
	}
	
	void Destroy()
	{
		m_Root.Unlink();
	}
	
	protected void SwitchNews()
	{
		if (m_SwitchToNext && m_ArticleIndex < m_NewsArticles.Count() - 1)
		{
			NextArticle();
			if (m_ArticleIndex == m_NewsArticles.Count() - 1)
			{
				m_SwitchToNext = false;
			}
		}
				
		if (!m_SwitchToNext && m_ArticleIndex > 0)
		{
			PreviousArticle();
			if (m_ArticleIndex == 0)
			{
				m_SwitchToNext = true;
			}
		}
	}
	
	void ShowNewsCarousel(bool show)
	{
		if (show)
		{
			UpdateCarouselInfo();
		}
		
		m_Root.Show(show);
	}
	
	protected void UpdateCarouselInfo()
	{
		if (!m_NewsArticles)
			m_NewsArticles = new array<JsonDataNewsArticle>;
		else
			m_NewsArticles.Clear();
		
		JsonDataNewsList newsData = MainMenuData.GetNewsData();
		if (!newsData)
			return;

		for (int i = 0; i < newsData.News.Count(); i++)
		{
			JsonDataNewsArticle article = newsData.News[i];
			if (article)
			{
				if (article.categoryID == NewsCategory.DLCS)
				{
					ModInfo modInfo = MainMenuData.GetDLCModInfoByName(article.dlcName);				
					if (modInfo && !modInfo.GetIsOwned())
					{
						m_NewsArticles.Insert(article);
					}
				}
				
				if (article.categoryID == NewsCategory.DLC_PROMO)
				{
					m_NewsArticles.Insert(article);
				}
				
				if (m_NewsArticles.Count() >= MAX_ARTICLES)
					break;
			}
		}
		
		int articlesCount = m_NewsArticles.Count();		
		if (articlesCount < MAX_ARTICLES)
		{
			if (articlesCount == 0)
			{
				m_Root.Show(false);
			}
			else
			{
				m_Root.Show(true);
				if (articlesCount == 1)
				{
					Widget pageIndicators = m_Root.FindAnyWidget("nf_pages");
					if (pageIndicators)
					{
						pageIndicators.Show(false);
					}
					
					#ifndef PLATFORM_CONSOLE
					m_NextPageBtnIcon.Show(false);
					#else
					m_NextPageBtnIconConsole.Show(false);
					#endif
				}
				
				// Color carousel page indicators or hide them if not needed
				for (int j = 1; j <= MAX_ARTICLES; j++)
				{
					Widget w = m_Root.FindAnyWidget("page" + j);
					if (!w)
						break;
									
					if (j > articlesCount)
					{
						w.Show(false);
					}
				}
			}
		}
		
		if (!m_NewsSwitchTimer && articlesCount > 1)
		{
			m_NewsSwitchTimer = new Timer(CALL_CATEGORY_GUI);
			m_NewsSwitchTimer.Run(ARTICLE_SWITCH_TIME, this, "SwitchNews", null, true);
		}
		
		if (!m_AnimatedSpacer)
		{
			int maxColums = (articlesCount - 1);
			m_AnimatedSpacer = new AnimatedGridSpacer(m_GridSpacer, maxColums, 0.4, AnimatedGridSpacerEasingType.EASE_IN_OUT_QUART);
			m_AnimatedSpacer.m_OnAnimationComplete.Insert(OnAnimationCompleted);
		}
		
		ShowPost(0);
	}
	
	protected void ShowPost(int index)
	{
		m_CurrentArticle = null;
		m_CurrentModInfo = null;
		
		if (index == 0 && m_NewsArticles.Count() == 0 || index > m_NewsArticles.Count() - 1)
			return;
		
		JsonDataNewsArticle article = m_NewsArticles[index];
		if (!article)
		{
			ErrorEx(string.Format("Failed to get article for index %1", index), ErrorExSeverity.WARNING);
			return;
		}
		
		m_CurrentArticle = article;
		
		ModInfo modInfo = MainMenuData.GetDLCModInfoByName(article.dlcName);
		if (modInfo)
		{
			m_CurrentModInfo = modInfo;
		}
		
		m_ArticleIndex = index;
		m_TitleText.SetText(article.title);
		
		ImageWidget imageW = ImageWidget.Cast(m_Root.FindAnyWidget("nf_image" + index));
		if (imageW)
		{
			if (article.coverImage != "")
			{
				imageW.LoadImageFile(0, article.coverImage);
				imageW.SetImage(0);
			}
		}
		
		for (int i = 1; i <= MAX_ARTICLES; i++)
		{
			Widget w = m_Root.FindAnyWidget("page" + i);
			if (!w)
				break;
			
			if ((i - 1) == index)
			{
				ColorHighlight(w);
			}
			else
			{
				ColorNormal(w);
			}
		}
	}
	
	void OnAnimationCompleted()
	{
		Widget focusW = GetWidgetUnderCursor();
		if (!focusW || focusW && focusW != m_HotlinkButton)
		{
			m_HotlinkButtonLabel.SetText(GetButtonLabel("255, 255, 255, 255"));
		}
		else
		{
			m_HotlinkButtonLabel.SetText(GetButtonLabel("255, 0, 0, 255"));
		}
	}
	
	protected void ColorHighlight(Widget w)
	{
		w.SetColor(ARGB(255, 255, 0, 0));
	}
	
	protected void ColorNormal(Widget w)
	{
		w.SetColor(ARGB(140, 0, 0, 0));
	}

	void OnClickNextArticle()
	{
		if (m_NewsSwitchTimer)
			m_NewsSwitchTimer.Stop();
		
		NextArticle();
		
		if (m_NewsSwitchTimer)
			m_NewsSwitchTimer.Run(3.0, this, "SwitchNews", null, true);
	}
	
	protected void NextArticle()
	{
		if (!m_AnimatedSpacer || m_AnimatedSpacer.IsAnimating())
			return;
		
		if (m_ArticleIndex < m_NewsArticles.Count() - 1)
		{
			m_ArticleIndex++;
			ShowPost(m_ArticleIndex);
			m_AnimatedSpacer.MoveRight();			
		}
		
		bool showRight = m_ArticleIndex < m_NewsArticles.Count() - 1;
		ShowArrowRight(showRight);
		bool showLeft = m_ArticleIndex > 0;
		ShowArrowLeft(showLeft);
	}
	
	void OnClickPreviousArticle()
	{
		if (m_NewsSwitchTimer)
			m_NewsSwitchTimer.Stop();
		
		PreviousArticle();
		
		if (m_NewsSwitchTimer)
			m_NewsSwitchTimer.Run(3.0, this, "SwitchNews", null, true);
	}
	
	protected void PreviousArticle()
	{
		if (!m_AnimatedSpacer || m_AnimatedSpacer.IsAnimating())
			return;
		
		if (m_ArticleIndex > 0)
		{
			m_ArticleIndex--;
			ShowPost(m_ArticleIndex);
			m_AnimatedSpacer.MoveLeft();
		}
		
		bool showRight = m_ArticleIndex < m_NewsArticles.Count() - 1;
		ShowArrowRight(showRight);
		bool showLeft = m_ArticleIndex > 0;
		ShowArrowLeft(showLeft);
	}
	
	protected void ShowArrowRight(bool state)
	{
		switch (m_CurrentInputDevice)
		{
			#ifdef PLATFORM_CONSOLE
			case EInputDeviceType.CONTROLLER:
			{
				m_NextPageBtnIconConsole.Show(state);
				break;
			}
			#endif
			default:
			{
				if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
				{
					m_NextPageBtnIcon.Show(state);
				}
				break;
			}
		}
	}
	
	protected void ShowArrowLeft(bool state)
	{
		switch (m_CurrentInputDevice)
		{
			#ifdef PLATFORM_CONSOLE
			case EInputDeviceType.CONTROLLER:
			{
				m_PrevPageBtnIconConsole.Show(state);
				break;
			}
			#endif
			default:
			{
				if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
				{
					m_PrevPageBtnIcon.Show(state);
				}
				break;
			}
		}
	}

	void ShowPromotion()
	{
		JsonDataNewsArticle article = m_NewsArticles[m_ArticleIndex];
		if (!article)
		{
			ErrorEx(string.Format("Failed to get article for index %1", m_ArticleIndex), ErrorExSeverity.WARNING);
			return;
		}
		
		if (m_CurrentModInfo)
		{
			m_CurrentModInfo.GoToStore();
		}
		else
		{
			if (article.fullUrl != "")
			{
				g_Game.OpenURL(article.fullUrl);
			}
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_HotlinkButton)
		{
			ShowPromotion();
		}
		else if (w == m_NextPageBtn)
		{
			OnClickNextArticle();
		}
		else if (w == m_PrevPageBtn)
		{
			OnClickPreviousArticle();
		}
		
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w == m_HotlinkButton)
		{
			if (m_CurrentArticle && m_CurrentArticle.fullUrl == string.Empty)
				return false;
			
			m_HotlinkButtonLabel.SetText(GetButtonLabel("255, 0, 0, 255"));
			m_HotlinkButtonLabel.SetColor(ARGB(255, 255, 0, 0));
			return true;
		}
		else if (w == m_NextPageBtn)
		{
			m_NextPageBtnIcon.SetColor(ARGB(255, 255, 0, 0));
			return true;
		}
		else if (w == m_PrevPageBtn)
		{
			m_PrevPageBtnIcon.SetColor(ARGB(255, 255, 0, 0));
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w == m_HotlinkButton)
		{
			if (m_CurrentArticle && m_CurrentArticle.fullUrl == string.Empty)
				return false;
			
			m_HotlinkButtonLabel.SetText(GetButtonLabel("255, 255, 255, 255"));
			m_HotlinkButtonLabel.SetColor(ARGB(255, 255, 255, 255));
			return true;
		}
		else if (w == m_NextPageBtn)
		{
			m_NextPageBtnIcon.SetColor(ARGB(255, 255, 255, 255));
			return true;
		}
		else if (w == m_PrevPageBtn)
		{
			m_PrevPageBtnIcon.SetColor(ARGB(255, 255, 255, 255));
			return true;
		}
		return false;
	}
		
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		bool showNext = m_ArticleIndex < m_NewsArticles.Count() - 1;
		bool showPrevious = m_ArticleIndex > 0;
		
		if (pInputDeviceType == EInputDeviceType.UNKNOWN && g_Game.GetInput().IsActiveGamepadSelected())
			pInputDeviceType = EInputDeviceType.CONTROLLER;
		
		switch (pInputDeviceType)
		{
			case EInputDeviceType.CONTROLLER:
			{
				m_CurrentInputDevice = pInputDeviceType;
				m_HotlinkButtonLabel.SetText(GetButtonLabel("255, 255, 255, 255"));
				m_NextPageBtnIcon.Show(false);
				m_PrevPageBtnIcon.Show(false);
				#ifdef PLATFORM_CONSOLE
				m_NextPageBtnIconConsole.Show(showNext);
				m_PrevPageBtnIconConsole.Show(showPrevious);
				#endif
				break;
			}
			default:
			{
				if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
				{
					m_CurrentInputDevice = pInputDeviceType;
					m_HotlinkButtonLabel.SetText(GetButtonLabel("255, 255, 255, 255"));
					#ifdef PLATFORM_CONSOLE
					m_NextPageBtnIconConsole.Show(false);
					m_PrevPageBtnIconConsole.Show(false);
					#endif
					m_NextPageBtnIcon.Show(showNext);
					m_PrevPageBtnIcon.Show(showPrevious);
					break;
				}
			}
		}
	}
	
	protected string GetButtonLabel(string color)
	{
		string shopLabel;
		#ifdef PLATFORM_WINDOWS
		shopLabel = "#dlc_open_store";
		#endif
		
		#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_MSSTORE
		shopLabel = "#dlc_open_store_Xbox";
		#else
		#ifdef PLATFORM_XBOX
		shopLabel = "#dlc_open_store_Xbox";
		#else
		shopLabel = "#dlc_open_store_PS";
		#endif
		#endif
		#endif
		
		if (m_CurrentArticle)
		{
			switch (m_CurrentArticle.categoryID)
			{
				case NewsCategory.DLC_PROMO:
				{
					if (m_CurrentArticle.fullUrl != string.Empty)
					{
						shopLabel = "#dlc_news_wishlist";
					}
					else
					{
						shopLabel = "#dlc_news_comingsoon";
					}
					break;
				}
				case NewsCategory.MISC_PROMO:
				{
					shopLabel = "#dlc_news_learn_more";
					break;
				}
			}
		}
		
		string label = shopLabel;
		string linkIcon = string.Format("<image set=\"%1\" name=\"%2\" scale=\"%3\"/>", "dayz_gui", "externallink", 0.9);
		
		if (m_CurrentInputDevice == EInputDeviceType.CONTROLLER && m_CurrentArticle && m_CurrentArticle.fullUrl != string.Empty)
		{
			label = string.Format("#dlc_news_hold %1%2", InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlX", "", EUAINPUT_DEVICE_CONTROLLER, 1.2), shopLabel);
		}
		
		string fullLabel = string.Format("<color rgba=\"%1\">%2 %3</color>", color, label, linkIcon);
		if (m_CurrentArticle && m_CurrentArticle.fullUrl == string.Empty)
			fullLabel = string.Format("<color rgba=\"%1\">%2</color>", color, label);
				
		return fullLabel;
	}
	
	void Update(float timeslice)
	{
		if (m_AnimatedSpacer)
			m_AnimatedSpacer.Update(timeslice);
	}
}
