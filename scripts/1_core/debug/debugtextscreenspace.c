
/*!
\addtogroup Debug
\{
*/

class DebugTextScreenSpace: DebugText
{
	// Instantiate using the Create method
	private void DebugTextScreenSpace();
	void ~DebugTextScreenSpace();

	proto external void SetPosition(float x, float y);
	proto external vector GetPosition();
	/*!
	Creates a text instance.
	\warning Returned instance should be saved to `ref`, otherwise it is impossible
	to remove from screen (unless DebugTextFlags.ONCE is used).
	\param text Text to draw on screen
	\param flags DebugTextFlags
	\param x X coordinate of the top left corner of the text
	\param y Y coordinate of the top left corner of the text
	\param size Size of the font in pixels
	\param color Color of the text
	\param bgColor Color of the back background behind the text
	\param priority Z order priority. Note: texts with background are always rendered under texts without background.
	\return DebugText Instance of debug text with given properties set, if flags contains DebugTextFlags.ONCE null is returned
	*/
	static proto DebugTextScreenSpace Create(string text, DebugTextFlags flags, float x, float y, float size = 20.0, int color = 0xFFFFFFFF, int bgColor = 0x00000000, int priority = 1000);
}

/*!
\}
*/
