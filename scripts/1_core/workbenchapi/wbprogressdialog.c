/*
===========================================
Do not modify, this script is generated
===========================================
*/

#ifdef WORKBENCH

/*!
\addtogroup WorkbenchAPI
\{
*/

/*!
\brief Modal progress dialog used to display task progress and status.

Provides an interface for updating progress, the main title, and the action
description shown to the user. The dialog is associated with a parent module
window.

\code
WBProgressDialog progress = new WBProgressDialog("Main Title", null);

for (int i = 0; i < 100; i++)
{
progress.SetProgress(i / 100);
progress.SetMainTitle("Working on something " + i / 20);
progress.SetActionTitle("Processing " + i / 10);
Print(i);
}
\endcode
*/
sealed class WBProgressDialog: Managed
{
	void WBProgressDialog(string title, WBModuleDef parentWindow);

	/*!
	Updates the progress value displayed in the dialog.
	\param progress Progress value in the range [0.0, 1.0].
	*/
	proto external void SetProgress(float progress);
	/*!
	Sets the main title text displayed at the top of the dialog.
	\param text New main title.
	*/
	proto external void SetMainTitle(string text);
	/*!
	Sets the action title text describing the current operation.
	\param text New action description.
	*/
	proto external void SetActionTitle(string text);
}

/*!
\}
*/

#endif // WORKBENCH
