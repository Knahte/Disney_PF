#include "cMain.h"
#include "Generating_path.h"
#include <streambuf>

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
EVT_BUTTON(10001, StartGeneration)
EVT_BUTTON(10002, SaveAttractionSelected)
EVT_BUTTON(10003, SaveSetting)
wxEND_EVENT_TABLE();



setting current_setting;



cMain::cMain() : wxFrame(nullptr, wxID_ANY, "DISNEY path finder", wxPoint(30, 30), wxSize(1020, 620), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
	wxIcon appIcon;
	appIcon.LoadFile("disney_logo_icon.ico", wxBITMAP_TYPE_ICO);

	// Définir l'icône pour la fenêtre principale
	SetIcon(appIcon);

	redirectOutputToFile((std::string)"DEBUG_OUTPUT_c_main.txt");

	btn1 = new wxButton(this, 10001, "Start Generation", wxPoint(10, 10), wxSize(320, 50));
	list1 = new wxListBox(this, 20001, wxPoint(10, 70), wxSize(320, 500));

	setting tmp_setting;
	std::vector<int> path = generatePath(tmp_setting, 10, 1);




	btn2 = new wxButton(this, 10002, "Save Selection", wxPoint(340, 520), wxSize(320, 50));

	wxScrolledWindow* panel = new wxScrolledWindow(this, wxID_ANY);
	panel->SetScrollbars(1, 1, 0, 0); // Activer les barres de défilement
	panel->SetSize(wxRect(340, 10, 320, 500));

	// Création d'une grille pour contenir les cases à cocher
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	int numCheckboxes = current_setting.full_ID_list.size(); // Nombre de cases à cocher
	int checkboxHeight = 30; // Hauteur de chaque case à cocher
	int initialYPos = 20; // Position initiale en y



	int i = 0;
	for (int& id : current_setting.full_ID_list){
		
		wxString attractionName = wxString::FromUTF8(attraction_data[id].name);
		wxCheckBox* checkbox = new wxCheckBox(panel, 30000 + id, attractionName);
		checkbox->SetValue(true);
		sizer->Add(checkbox, 0, wxALL, 2); // Ajouter la case à cocher à la grille avec une marge de 5 pixels
		int yPos = initialYPos + i * checkboxHeight;
		++i;
		checkbox->SetPosition(wxPoint(10, yPos));
	}

	panel->SetSizer(sizer);
	panel->FitInside();
	panel->SetScrollRate(5, 5);




	wxPanel* rightPanel = new wxPanel(this, wxID_ANY, wxPoint(670, 10), wxSize(320, 560));

	wxArrayString hour_choices;
	for (int hour = 0; hour < 24; ++hour) {
		hour_choices.Add(wxString::Format("%02d", hour));
	}
	hour_comboBox = new wxComboBox(rightPanel, 40001, "", wxPoint(10, 10), wxDefaultSize, hour_choices);
	hour_comboBox->SetSelection(6);

	wxArrayString minute_choices;
	for (int minute = 0; minute < 60; minute += 5) {
		minute_choices.Add(wxString::Format("%02d", minute));
	}
	minute_comboBox = new wxComboBox(rightPanel, 40002, "", wxPoint(70, 10), wxDefaultSize, minute_choices);
	minute_comboBox->SetSelection(0);

	wxArrayString hotel_choices;
	for (auto& hotel_to_add : hotel_data) {
		hotel_choices.Add(wxString(hotel_to_add.second.name));
	}
	hotel_comboBox = new wxComboBox(rightPanel, 40003, "", wxPoint(10, 40), wxDefaultSize, hotel_choices);
	hotel_comboBox->SetSelection(0);

	checkbox1 = new wxCheckBox(rightPanel, 40004, "Fastest Path mode", wxPoint(10, 70), wxSize(300, 30));
	checkbox1->SetValue(false);
	checkbox2 = new wxCheckBox(rightPanel, 40005, "Single Rider (WIP)", wxPoint(10, 100), wxSize(300, 30));
	checkbox2->SetValue(false);
	//checkbox2->Enable(false);

	wxArrayString speed_choices;
	for (double speed = 1.0; speed < 6.0; speed += 0.5) {
		speed_choices.Add(wxString::Format("%.1f", speed));
	}
	speed_comboBox = new wxComboBox(rightPanel, 40006, "", wxPoint(10, 140), wxDefaultSize, speed_choices);
	speed_comboBox->SetSelection(3);


	btn3 = new wxButton(rightPanel, 10003, "Save Setting", wxPoint(10, 180), wxSize(300, 50));


	wxStaticText* messageText = new wxStaticText(rightPanel, wxID_ANY,
		"Hi, it's the dev!\nThis is my application!\nIt generates the most optimal route for a typical day at DisneyLand Paris.\nTo run a path simulation, press the 'Start Generation' button and it'll work (probably)....\n\nYou can customize your simulation: \n- you can deselect a checkbox and *poof* the path won't go there anymore!\n- you can also switch to Single Rider mode, change your starting point (hotels/station) or, if you really want to walk as little as possible, you can change the type of path from fastest to shortest.\n\nNow you know everything! \nThe Dev.\n\nThe github is here: https://github.com/Knahte/Disney_PF"
		, wxPoint(10, 250), wxSize(300, 320));


	restoreOutput();
}

cMain::~cMain()
{

}



void cMain::StartGeneration(wxCommandEvent& evt) {
	redirectOutputToFile((std::string)"DEBUG_OUTPUT_generation.txt");
	list1->Clear();
	btn1->Enable(false);



	std::vector<int> path = generatePath(current_setting, 100, 200);// That is the setting : you can change it if you want


	double current_time = current_setting.entry_time;
	int distance_to_next = 0;

	list1->AppendString(std::to_string((int)current_time % 24) + "h" + std::to_string((int)((current_time - (int)current_time) * 60)) + " - " + std::to_string((int)distance_to_next) + "m - " + attraction_data[0].name);

	distance_to_next = findShortestPath(intersection_data[0], intersection_data[path[0] * 1000]);

	for (int i = 0; i < path.size() - 1; i++) {

		if (i != 0) distance_to_next = findShortestPath(intersection_data[path[i - 1] * 1000], intersection_data[path[i] * 1000]);
		current_time += distance_to_next / current_setting.walking_speed / 1000;
		std::cout << "walking_time = " << distance_to_next / current_setting.walking_speed / 1000 << std::endl;
		list1->AppendString(std::to_string((int)current_time % 24) + "h" + std::to_string((int)((current_time - (int)current_time) * 60)) + " - " + std::to_string((int)distance_to_next) + "m - " + attraction_data[path[i]].name);


		current_time += attraction_data[path[i]].wait_time[(int)(current_time) % 24] / 60.0;

	}
	list1->AppendString(std::to_string((int)current_time % 24) + "h" + std::to_string((int)((current_time - (int)current_time) * 60)) + " - " + std::to_string((int)distance_to_next) + "m - " + attraction_data[path.back()].name);

	pathToGPX(path, "path_generated" + std::to_string(time(nullptr)), current_setting);

	btn1->Enable(true);
	restoreOutput();
	evt.Skip();
}

void cMain::SaveAttractionSelected(wxCommandEvent& evt) {
	btn2->Enable(false);
	int numCheckboxes = attraction_data.size();

	current_setting.ID_list = {};

	// Parcourir toutes les cases à cocher
	for (auto it = attraction_data.begin(); it != attraction_data.end(); ++it) {
		int id = it->first;
		// Récupérer l'ID de la case à cocher
		int checkboxID = 30000 + attraction_data[id].ID;

		// Vérifier si la case à cocher est cochée
		wxCheckBox* checkbox = wxDynamicCast(FindWindow(checkboxID), wxCheckBox);
		if (checkbox && checkbox->IsChecked()) {
			current_setting.ID_list.push_back(id);
		}
	}
	btn2->Enable(true);
}

void cMain::SaveSetting(wxCommandEvent& evt) {
	redirectOutputToFile((std::string)"DEBUG_OUTPUT_SaveSetting.txt");

	current_setting.consider_waiting_times = !(checkbox1->GetValue());

	current_setting.single_rider = checkbox2->GetValue();

	wxString selectedHour = hour_comboBox->GetStringSelection();
	wxString selectedMinute = minute_comboBox->GetStringSelection();

	current_setting.entry_time = std::stoi(selectedHour.ToStdString()) + std::stoi(selectedMinute.ToStdString()) / 60.0;

	wxString selectedHotel = hotel_comboBox->GetStringSelection();
	for (const auto& hotel : hotel_data) {
		if (hotel.second.name == selectedHotel) {
			current_setting.hotel_ID = hotel.second.ID;
			break;
		}
	}

	wxString selectedSpeed = speed_comboBox->GetStringSelection();
	current_setting.walking_speed = std::stod(selectedSpeed.ToStdString());
		
	restoreOutput();
}