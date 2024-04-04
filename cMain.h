#pragma once
#include "wx/wx.h"

// Declare the main frame class derived from wxFrame
class cMain : public wxFrame
{
public:
	// Constructor
	cMain();

	// Destructor
	~cMain();


	// List box to display attractions
	wxListBox* list1 = nullptr;

	// Button to start generation
	wxButton* btn1 = nullptr;


	// Button to save selected attractions
	wxButton* btn2 = nullptr;


	// Combo box to select the **hour** of the start time
	wxComboBox* hour_comboBox = nullptr;
	// Combo box to select the **hour** of the start time
	wxComboBox* minute_comboBox = nullptr;

	// Combo box to select hotel
	wxComboBox* hotel_comboBox = nullptr;

	// Checkbox for shortest past (with distance) Mode
	wxCheckBox* checkbox1 = nullptr;

	// Checkbox for Single Rider Mode
	wxCheckBox* checkbox2 = nullptr;

	// Combo box to select walking speed
	wxComboBox* speed_comboBox = nullptr;

	// Curser to select the complexity of the generation (influence on the time taken)
	wxSlider* complexity_slider = nullptr;

	// Button to save settings
	wxButton* btn3 = nullptr;

	// Event handler for starting generation
	void StartGeneration(wxCommandEvent& evt);
	
	// Event handler for saving selected attractions
	void SaveAttractionSelected(wxCommandEvent& evt);

	// Event handler for saving settings
	void SaveSetting(wxCommandEvent& evt);

	// Event table declaration for handling events
	wxDECLARE_EVENT_TABLE();

};

