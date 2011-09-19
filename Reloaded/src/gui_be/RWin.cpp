/* Reloaded - Amstrad CPC emulator
 * Haiku/BeOS GUI by PulkoMandy - 2011
 */

#include <Application.h>
#include <FilePanel.h>
#include <MenuBar.h>
#include <MenuItem.h>

#include "video.h"

#include "KeymapWindow.h"
#include "REmulator.h"
#include "RBitmapView.h"
#include "RWin.h"

ReloadedWin::ReloadedWin()
	: BWindow(BRect(80,80,847,619),"Reloaded", B_TITLED_WINDOW,
			B_NOT_ZOOMABLE|B_NOT_RESIZABLE|B_QUIT_ON_WINDOW_CLOSE)
{
	// TODO - compute the size instead of hardcoding it
	SetSizeLimits(847-80,847-80,619-61,619-61);

	BMenuBar* menu = new BMenuBar(BRect(0,0,847,16),"mainmenu");
		BMenu* file = new BMenu("File");
		menu->AddItem(file);
			BMenuItem* insertdsk = new BMenuItem("Insert Disc",
				new BMessage('dins'));
		file->AddItem(insertdsk);
		
		BMenu* settings = new BMenu("Settings");
		menu->AddItem(settings);
			BMenuItem* keymap = new BMenuItem("Keymap",
				new BMessage('kymp'));
		settings->AddItem(keymap);
	AddChild(menu);
	
	fBitmapView = new RBitmapView(this, false);
	fBitmapView->MoveTo(0, menu->Bounds().bottom + 1);
	
	Show();
}


void ReloadedWin::MessageReceived(BMessage* mess)
{
	int k;
	int m;

	switch(mess->what)
	{
		case B_KEY_DOWN:
		case B_UNMAPPED_KEY_DOWN:
			m = mess->FindInt32("modifiers");
			k = mess->FindInt32("key");
			REmulator::getInstance()->PressKey(k,m);
			break;
		case B_KEY_UP:
		case B_UNMAPPED_KEY_UP:
			m = mess->FindInt32("modifiers");
			k = mess->FindInt32("key");
			REmulator::getInstance()->ReleaseKey(k,m);
			break;

		case 'rfsh':
			fBitmapView->Invalidate();
			break;
			
		case 'dins':
		{
			BFilePanel* f = new BFilePanel(B_OPEN_PANEL, &be_app_messenger, NULL, B_FILE_NODE, false);
			f->Show();
			break;
		}
		
		case 'kymp':
		{
			KeymapWindow* kwin = new KeymapWindow();
			kwin->Show();
			break;	
		}
		
		default:
			BWindow::MessageReceived(mess);
			break;
	}
}
