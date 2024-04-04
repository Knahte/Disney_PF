#include "cApp.h"

// Implement the wxWidgets application
wxIMPLEMENT_APP(cApp);

// Constructor implementation
cApp::cApp() {

}
// Destructor implementation
cApp::~cApp() {

}

// Function called when the application initializes
bool cApp::OnInit()
{
	// Create an instance of the main frame
	m_frame1 = new cMain();

	// Show the main frame (it's usefull)
	m_frame1->Show();

	// Return true to indicate successful initialization
	return true;
}