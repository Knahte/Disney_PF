#pragma once
#include "wx/wx.h"
#include "cMain.h"

// Define the application class derived from wxApp
class cApp : public wxApp
{
public:
	// Constructor
	cApp();
	// Destructor
	~cApp();



private:
	cMain* m_frame1 = nullptr; // Pointer to the main frame of the application

public:
	// Function called when the application initializes
	virtual bool OnInit();

};

