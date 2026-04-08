
/*!
\addtogroup Debug
\{
*/

//! Used for creating on screen texts in world.
class DebugText: Managed
{
	//! Instantiate using the Create method in derived classes
	private void DebugText();
	void ~DebugText();

	proto external string GetText();
	proto external float GetFontSize();
	proto external int GetTextColor();
	proto external int GetBackgroundColor();
	proto external int GetPriority();
	proto external void SetText(string text);
	proto external void SetTextColor(int color);
	proto external void SetFontSize(float size);
	proto external void SetBackgroundColor(int color);
	/*!
	Sets Z order priority.
	\note Texts with background are always rendered under texts without background.
	*/
	proto external void SetPriority(int priority);
}

/*!
\}
*/
