class LoadingMenu extends UIScriptedMenu
{
	void LoadingMenu()
	{
	}

	void ~LoadingMenu()
	{
	}
	
	override Widget Init()
	{
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/loading.layout");
		
		m_label = TextWidget.Cast( layoutRoot.FindAnyWidget("TextWidget") );
		m_progressBar = ProgressBarWidget.Cast( layoutRoot.FindAnyWidget("ProgressBarWidget") );
		m_image = ImageWidget.Cast( layoutRoot.FindAnyWidget("ImageBackground") );
		
		m_image.LoadImageFile( 0, GetRandomLoadingBackground() );
		layoutRoot.FindAnyWidget("notification_root").Show(false);
		
		Widget expNotification = layoutRoot.FindAnyWidget("notification_root");
		bool isXBoxExperimental = false;
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
			isXBoxExperimental = true;
		#endif
		#endif
		#ifdef PLATFORM_MSSTORE
		#ifdef BUILD_EXPERIMENTAL
			isXBoxExperimental = true;
		#endif
		#endif
		if (expNotification)
			expNotification.Show(isXBoxExperimental);
		
		return layoutRoot;
	}

	TextWidget m_label;
	ProgressBarWidget m_progressBar;
	ImageWidget m_image;
}
