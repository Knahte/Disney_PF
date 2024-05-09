#include "cMain.h"
#include "Generating_path.h"
#include <iostream>
#include <streambuf>


wxBEGIN_EVENT_TABLE(cMain, wxFrame)
EVT_BUTTON(10001, StartGeneration)
EVT_BUTTON(10002, ResetApplication)
wxEND_EVENT_TABLE();


setting current_setting;


// Constructor implementation
cMain::cMain() : wxFrame(nullptr, wxID_ANY, "DISNEY path finder", wxPoint(30, 30), wxSize(1020, 620), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
	// Load application icon
	wxIcon appIcon;
	appIcon.LoadFile("disney_logo_icon.ico", wxBITMAP_TYPE_ICO);
	// Set the icon for the main window
	SetIcon(appIcon);

	// Redirect debug output to a file if debug mode is enabled
	if (current_setting.debug_mode) redirectOutputToFile((std::string)"DEBUG_OUTPUT_c_main.txt");

	// generate a path just to get the attraction data working
	std::vector<int> path = generatePath(current_setting, 10, 1, current_setting.path_to_data);



	// Button to start generation
	btn1 = new wxButton(this, 10001, "Start Generation", wxPoint(10, 10), wxSize(320, 50));

	// List box to display attractions (on the path)
	list1 = new wxListBox(this, 20001, wxPoint(10, 70), wxSize(320, 500));




	// Panel to contain checkboxes
	wxScrolledWindow* panel = new wxScrolledWindow(this, wxID_ANY);
	panel->SetScrollbars(1, 1, 0, 0); // Enable scroll bars
	panel->SetSize(wxRect(340, 10, 320, 560));

	// Create a sizer to arrange checkboxes vertically
	sizer = new wxBoxSizer(wxVERTICAL);

	// Number of checkboxes
	int numCheckboxes = current_setting.full_ID_list.size();

	// Height of each checkbox
	int checkboxHeight = 30;

	// Initial position in y-axis
	int initialYPos = 20;


	// Iterate through attraction IDs
	int i = 0;
	for (int& id : current_setting.full_ID_list) {
		bool is_single_rider = false;



		if (!is_single_rider) {
			// Get attraction name
			wxString attractionName = wxString::FromUTF8(attraction_data[id].name);

			// Create and initialize checkbox
			wxCheckBox* checkbox = new wxCheckBox(panel, 30000 + id, attractionName);
			checkbox->SetValue(true);
			sizer->Add(checkbox, 0, wxALL, 2); // Add checkbox to sizer with a margin of 2 pixels
			int yPos = initialYPos + i * checkboxHeight;
			++i;
			checkbox->SetPosition(wxPoint(10, yPos));
		}
	}

	panel->SetSizer(sizer);
	panel->FitInside();
	panel->SetScrollRate(5, 5);







	// Panel to contain settings on the right side
	wxPanel* rightPanel = new wxPanel(this, wxID_ANY, wxPoint(670, 10), wxSize(320, 560));

	// Combo box to select hour
	wxArrayString hour_choices;
	for (int hour = 0; hour < 24; ++hour) {
		hour_choices.Add(wxString::Format("%02d", hour));
	}
	hour_comboBox = new wxComboBox(rightPanel, 40001, "", wxPoint(10, 10), wxDefaultSize, hour_choices);
	hour_comboBox->SetSelection(6);

	// Combo box to select minute
	wxArrayString minute_choices;
	for (int minute = 0; minute < 60; minute += 5) {
		minute_choices.Add(wxString::Format("%02d", minute));
	}
	minute_comboBox = new wxComboBox(rightPanel, 40002, "", wxPoint(70, 10), wxDefaultSize, minute_choices);
	minute_comboBox->SetSelection(0);

	// Combo box to select hotel
	wxArrayString hotel_choices;
	for (auto& hotel_to_add : hotel_data) {
		hotel_choices.Add(wxString(hotel_to_add.second.name));
	}
	hotel_comboBox = new wxComboBox(rightPanel, 40003, "", wxPoint(10, 40), wxDefaultSize, hotel_choices);
	hotel_comboBox->SetSelection(0);

	// Checkbox for fastest path mode
	checkbox1 = new wxCheckBox(rightPanel, 40004, "Fastest Path mode", wxPoint(10, 70), wxSize(300, 30));
	checkbox1->SetValue(false);

	// Checkbox for single rider mode
	checkbox2 = new wxCheckBox(rightPanel, 40005, "Single Rider", wxPoint(10, 100), wxSize(300, 30));
	checkbox2->SetValue(false);

	// Combo box to select walking speed
	wxArrayString speed_choices;
	for (double speed = 1.0; speed < 6.0; speed += 0.5) {
		speed_choices.Add(wxString::Format("%.1f", speed));
	}
	speed_comboBox = new wxComboBox(rightPanel, 40006, "", wxPoint(10, 140), wxDefaultSize, speed_choices);
	speed_comboBox->SetSelection(3);
	wxStaticText* messageTextSpeed = new wxStaticText(rightPanel, wxID_ANY, "Walking Speed (in km/h)", wxPoint(60, 143), wxSize(320 - 60, 30));

	wxStaticText* messageTextSilder = new wxStaticText(rightPanel, wxID_ANY, "appoximative/fast generation       slow/precise generation", wxPoint(10, 170), wxSize(320, 30));
	complexity_slider = new wxSlider(rightPanel, wxID_ANY, 0, 10, 400, wxPoint(10, 190), wxSize(300, -1)); // Les valeurs 0, 100 sont les valeurs min et max respectivement
	complexity_slider->SetValue(current_setting.number_of_path);








	std::vector<wxString> park_list = { wxString("Disney Land Paris"), wxString("Europapark")};

	wxArrayString park_choices;
	for (auto& park : park_list) {
		park_choices.Add(park);
	}

	folderComboBox = new wxComboBox(rightPanel, 40007, "", wxPoint(10, 240), wxDefaultSize, park_choices);
	folderComboBox->SetSelection(0);

	// Créer un bouton pour réinitialiser l'application
	resetButton = new wxButton(rightPanel, 10002, "Reset Application", wxPoint(10, 270), wxSize(300, -1));




	//message by me (ﾟvﾟ)ノ
	wxStaticText* messageTextInfo = new wxStaticText(rightPanel, wxID_ANY,
		"Hi, it's the dev! And this is my app!\nIt generates the optimal route for a typical day at DisneyLand Paris.\nTo run a path simulation, press 'Start Generation' and it'll work (probably)....\n\nThe Dev.\n\n\nDisclaimer: This project is not endorsed or supported by any other companies or entities, including Disney. It is solely developed and maintained by an independent developer (me :D).",
		wxPoint(10, 330), wxSize(300, 320));

	// Restore debug output if debug mode is enabled
	if (current_setting.debug_mode)
	restoreOutput();
}

// Destructor implementation
cMain::~cMain()
{

}



void cMain::StartGeneration(wxCommandEvent& evt) {



	if (current_setting.debug_mode)
		redirectOutputToFile((std::string)"DEBUG_OUTPUT_generation.txt");

	// Update the setting based on user input
	current_setting.consider_waiting_times = !(checkbox1->GetValue());
	current_setting.single_rider = checkbox2->GetValue();

	// Extract selected hour and minute from combo boxes and update entry time
	wxString selectedHour = hour_comboBox->GetStringSelection();
	wxString selectedMinute = minute_comboBox->GetStringSelection();
	current_setting.entry_time = std::stoi(selectedHour.ToStdString()) + std::stoi(selectedMinute.ToStdString()) / 60.0;

	// Extract selected hotel from combo box and update hotel ID in setting
	wxString selectedHotel = hotel_comboBox->GetStringSelection();
	for (const auto& hotel : hotel_data) {
		if (hotel.second.name == selectedHotel) {
			current_setting.hotel_ID = hotel.second.ID;
			break;
		}
	}

	// Extract selected walking speed from combo box and update in setting
	wxString selectedSpeed = speed_comboBox->GetStringSelection();
	current_setting.walking_speed = std::stod(selectedSpeed.ToStdString());

	current_setting.number_of_path = complexity_slider->GetValue();


	// Iterate through all attractions
	current_setting.ID_list = {};

	// Iterate through all attractions
	for (int id : current_setting.full_ID_list) {
		// Get the ID of the checkbox associated with this attraction
		int checkboxID = 30000 + attraction_data[id].ID;

		// Find the checkbox and check if it's checked
		wxCheckBox* checkbox = wxDynamicCast(FindWindow(checkboxID), wxCheckBox);
		if (checkbox && checkbox->IsChecked()) {
			// If checked, add the attraction ID to the list of selected IDs
			current_setting.ID_list.push_back(id);
		}
	}

	// Clear the list box and disable the button temporarily
	list1->Clear();
	btn1->Enable(false);

	// Generate the path using the current settings you are free too use what ever you want
	std::vector<int> path = generatePath(current_setting, current_setting.ID_list.size() * 3, current_setting.number_of_path, current_setting.path_to_data);

	double current_time = current_setting.entry_time;
	int distance_to_next = 0;

	// Display the starting location in the list box
	list1->AppendString(std::to_string((int)current_time % 24) + "h" + std::to_string((int)((current_time - (int)current_time) * 60)) + " - " + std::to_string((int)distance_to_next) + "m - " + attraction_data[0].name);

	// Calculate the distance to the next location
	distance_to_next = findShortestPath(intersection_data[0], intersection_data[path[0] * 1000]);

	current_time += distance_to_next / current_setting.walking_speed / 1000;

	// Iterate over the path and display each attraction in the list box
	for (int i = 0; i < path.size() - 1; i++) {

		// Calculate the distance to the next location if not the first attraction
		if (i != 0)
			distance_to_next = findShortestPath(intersection_data[path[i - 1] * 1000], intersection_data[path[i] * 1000]);

		// Update the current time based on walking time to the next location
		

		// Display the attraction in the list box
		list1->AppendString(std::to_string((int)current_time % 24) + "h" + std::to_string((int)((current_time - (int)current_time) * 60)) + " - " + std::to_string((int)distance_to_next) + "m - " + attraction_data[path[i]].name);

		// update current time
		current_time += getTimeTaken(distance_to_next, current_time, path[i], current_setting);
	}

	// Display the last attraction in the list box
	list1->AppendString(std::to_string((int)current_time % 24) + "h" + std::to_string((int)((current_time - (int)current_time) * 60)) + " - " + std::to_string((int)distance_to_next) + "m - " + attraction_data[path.back()].name);

	// Export the generated path to a GPX file
	pathToGPX(path, "path_generated" + std::to_string(time(nullptr)), current_setting);

	// Re-enable the button and 
	btn1->Enable(true);

	// Restore output if debug mode is enabled
	if (current_setting.debug_mode)
		restoreOutput();
}

void cMain::ResetApplication(wxCommandEvent& evt) {
	redirectOutputToFile((std::string)"DEBUG_OUTPUT_closing.txt");

	// Retrieves the selected folder name from the dropdown list
	wxString folder_name = folderComboBox->GetStringSelection();

	// Displays a message indicating that the application needs to be restarted
	wxMessageBox(wxT("You'll need to restart the app (I'm working on that)"), wxT("Error"), wxICON_ERROR | wxOK);

	if (!folder_name.IsEmpty()) {
		// Paths to the files to be copied
		wxString json_file_path = wxT("data/data.json");
		wxString csv_file_path = wxT("data/waiting_time.csv");

		// Path of the original folder containing the files to be copied
		wxString origin_folder = wxT("data/") + folder_name;

		// Copies the data.json file from the original folder to the application folder
		if (!wxCopyFile(origin_folder + wxT("/data.json"), json_file_path, true)) {
			wxMessageBox(wxT("Error copying data.json file"), wxT("Error"), wxICON_ERROR | wxOK);
		}

		// Copies the waiting_time.csv file from the original folder to the application folder
		if (!wxCopyFile(origin_folder + wxT("/waiting_time.csv"), csv_file_path, true)) {
			wxMessageBox(wxT("Error copying waiting_time.csv file"), wxT("Error"), wxICON_ERROR | wxOK);
		}
	}

	// Terminates the execution of the application
	exit(0);

	restoreOutput();
}