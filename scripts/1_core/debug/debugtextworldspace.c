
/*!
\addtogroup Debug
\{
*/

class DebugTextWorldSpace: DebugText
{
	// Instantiate using the Create method
	private void DebugTextWorldSpace();
	void ~DebugTextWorldSpace();

	proto external vector GetPosition();
	proto external void SetPosition(float x, float y, float z);
	//! Sets the transformation matrix of the text. Useful only for texts created with CreateInWorld().
	proto external void SetTransform(vector mat[4]);
	//! Returns the transformation matrix of the text. Useful only for texts created with CreateInWorld().
	proto void GetTransform(out vector mat[4]);
	/*!
	Creates a text instance.
	\warning Returned instance should be saved to `ref`, otherwise it is impossible
	to remove from screen (unless DebugTextFlags.ONCE is used).
	\param text Text to draw on screen
	\param flags DebugTextFlags
	\param x X coordinate of the top left corner of the text in world space
	\param y Y coordinate of the top left corner of the text in world space
	\param z Z coordinate of the top left corner of the text in world space
	\param size Size of the font in pixels
	\param color Color of the text
	\param bgColor Color of the back background behind the text
	\param priority Z order priority. Note: texts with background are always rendered under texts without background.
	\return DebugText Instance of debug text with given properties set, if flags contains DebugTextFlags.ONCE null is returned
	*/
	static proto DebugTextWorldSpace Create(string text, DebugTextFlags flags, float x, float y, float z, float size = 20.0, int color = 0xFFFFFFFF, int bgColor = 0x00000000, int priority = 1000);
	/*!
	The text is placed in world and is occluded by other objects. The size is in meters (approximately).
	\param size Size of the font in meters (approximately)
	\see DebugTextWorldSpace.Create
	*/
	static proto DebugTextWorldSpace CreateInWorld(string text, DebugTextFlags flags, vector transform[4], float size = 20.0, int color = 0xFFFFFFFF, int bgColor = 0x00000000, int priority = 1000);
}

/*!
\}
*/
