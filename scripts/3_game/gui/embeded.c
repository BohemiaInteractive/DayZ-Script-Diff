
class EmbededButtonScript
{
	reference string Caption;
	void OnWidgetScriptInit(Widget w)
	{
		ButtonWidget button = ButtonWidget.Cast(w.GetChildren());
		button.SetText(Caption);
	}
};

