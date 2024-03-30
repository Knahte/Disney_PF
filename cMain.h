#pragma once
#include "wx/wx.h"

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

public:

	wxListBox* list1 = nullptr;
	wxButton* btn1 = nullptr;



	wxButton* btn2 = nullptr;

	wxComboBox* hour_comboBox = nullptr;
	wxComboBox* minute_comboBox = nullptr;

	wxComboBox* hotel_comboBox = nullptr;

	wxCheckBox* checkbox1 = nullptr;
	wxCheckBox* checkbox2 = nullptr;

	wxComboBox* speed_comboBox = nullptr;

	wxButton* btn3 = nullptr;

	void StartGeneration(wxCommandEvent& evt);
	void SaveAttractionSelected(wxCommandEvent& evt);
	void SaveSetting(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();

};

