#include "Preferences.hpp"
#include "OptionsGroup.hpp"
#include "GUI_App.hpp"
#include "Plater.hpp"
#include "I18N.hpp"
#include "libslic3r/AppConfig.hpp"

namespace Slic3r {
namespace GUI {

PreferencesDialog::PreferencesDialog(wxWindow* parent) : 
    DPIDialog(parent, wxID_ANY, _L("Preferences"), wxDefaultPosition, 
              wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
#ifdef __WXOSX__
    isOSX = true;
#endif
	build();
}

void PreferencesDialog::build()
{
	auto app_config = get_app_config();
	m_optgroup_general = std::make_shared<ConfigOptionsGroup>(this, _L("General"));
	m_optgroup_general->label_width = 40;
	m_optgroup_general->m_on_change = [this](t_config_option_key opt_key, boost::any value) {
		if (opt_key == "default_action_on_close_application" || opt_key == "default_action_on_select_preset")
			m_values[opt_key] = boost::any_cast<bool>(value) ? "none" : "discard";
		else
		    m_values[opt_key] = boost::any_cast<bool>(value) ? "1" : "0";
	};

	// TODO
//    $optgroup->append_single_option_line(Slic3r::GUI::OptionsGroup::Option->new(
//        opt_id = > 'version_check',
//        type = > 'bool',
//        label = > 'Check for updates',
//        tooltip = > 'If this is enabled, Slic3r will check for updates daily and display a reminder if a newer version is available.',
//        default = > $app_config->get("version_check") // 1,
//        readonly = > !wxTheApp->have_version_check,
//    ));

	bool is_editor = wxGetApp().is_editor();

	ConfigOptionDef def;
	Option option(def, "");
	if (is_editor) {
		def.label = L("Remember output directory");
		def.type = coBool;
		def.tooltip = L("If this is enabled, Slic3r will prompt the last output directory "
			"instead of the one containing the input files.");
		def.set_default_value(new ConfigOptionBool{ app_config->has("remember_output_path") ? app_config->get("remember_output_path") == "1" : true });
		option = Option(def, "remember_output_path");
		m_optgroup_general->append_single_option_line(option);

		def.label = L("Auto-center parts");
		def.type = coBool;
		def.tooltip = L("If this is enabled, Slic3r will auto-center objects "
			"around the print bed center.");
		def.set_default_value(new ConfigOptionBool{ app_config->get("autocenter") == "1" });
		option = Option(def, "autocenter");
		m_optgroup_general->append_single_option_line(option);

		def.label = L("Background processing");
		def.type = coBool;
		def.tooltip = L("If this is enabled, Slic3r will pre-process objects as soon "
			"as they\'re loaded in order to save time when exporting G-code.");
		def.set_default_value(new ConfigOptionBool{ app_config->get("background_processing") == "1" });
		option = Option(def, "background_processing");
		m_optgroup_general->append_single_option_line(option);

		// Please keep in sync with ConfigWizard
		def.label = L("Check for application updates");
		def.type = coBool;
		def.tooltip = L("If enabled, PrusaSlicer will check for the new versions of itself online. When a new version becomes available a notification is displayed at the next application startup (never during program usage). This is only a notification mechanisms, no automatic installation is done.");
		def.set_default_value(new ConfigOptionBool(app_config->get("version_check") == "1"));
		option = Option(def, "version_check");
		m_optgroup_general->append_single_option_line(option);

		// Please keep in sync with ConfigWizard
		def.label = L("Export sources full pathnames to 3mf and amf");
		def.type = coBool;
		def.tooltip = L("If enabled, allows the Reload from disk command to automatically find and load the files when invoked.");
		def.set_default_value(new ConfigOptionBool(app_config->get("export_sources_full_pathnames") == "1"));
		option = Option(def, "export_sources_full_pathnames");
		m_optgroup_general->append_single_option_line(option);

		// Please keep in sync with ConfigWizard
		def.label = L("Update built-in Presets automatically");
		def.type = coBool;
		def.tooltip = L("If enabled, Slic3r downloads updates of built-in system presets in the background. These updates are downloaded into a separate temporary location. When a new preset version becomes available it is offered at application startup.");
		def.set_default_value(new ConfigOptionBool(app_config->get("preset_update") == "1"));
		option = Option(def, "preset_update");
		m_optgroup_general->append_single_option_line(option);

		def.label = L("Suppress \" - default - \" presets");
		def.type = coBool;
		def.tooltip = L("Suppress \" - default - \" presets in the Print / Filament / Printer "
			"selections once there are any other valid presets available.");
		def.set_default_value(new ConfigOptionBool{ app_config->get("no_defaults") == "1" });
		option = Option(def, "no_defaults");
		m_optgroup_general->append_single_option_line(option);

		def.label = L("Show incompatible print and filament presets");
		def.type = coBool;
		def.tooltip = L("When checked, the print and filament presets are shown in the preset editor "
			"even if they are marked as incompatible with the active printer");
		def.set_default_value(new ConfigOptionBool{ app_config->get("show_incompatible_presets") == "1" });
		option = Option(def, "show_incompatible_presets");
		m_optgroup_general->append_single_option_line(option);

		def.label = L("Show drop project dialog");
		def.type = coBool;
		def.tooltip = L("When checked, whenever dragging and dropping a project file on the application, shows a dialog asking to select the action to take on the file to load.");
		def.set_default_value(new ConfigOptionBool{ app_config->get("show_drop_project_dialog") == "1" });
		option = Option(def, "show_drop_project_dialog");
		m_optgroup_general->append_single_option_line(option);

		def.label = L("Single instance mode");
		def.type = coBool;
#if __APPLE__
		def.tooltip = L("On OSX there is always only one instance of app running by default. However it is allowed to run multiple instances of same app from the command line. In such case this settings will allow only one instance.");
#else
		def.tooltip = L("If this is enabled, when starting PrusaSlicer and another instance of the same PrusaSlicer is already running, that instance will be reactivated instead.");
#endif
		def.set_default_value(new ConfigOptionBool{ app_config->has("single_instance") ? app_config->get("single_instance") == "1" : false });
		option = Option(def, "single_instance");
		m_optgroup_general->append_single_option_line(option);
	}

#if __APPLE__
	def.label = L("Use Retina resolution for the 3D scene");
	def.type = coBool;
	def.tooltip = L("If enabled, the 3D scene will be rendered in Retina resolution. "
	                "If you are experiencing 3D performance problems, disabling this option may help.");
	def.set_default_value(new ConfigOptionBool{ app_config->get("use_retina_opengl") == "1" });
	option = Option (def, "use_retina_opengl");
	m_optgroup_general->append_single_option_line(option);
#endif
/*  // ysFIXME THis part is temporary commented
    // The using of inches is implemented just for object's size and position
    
	def.label = L("Use inches instead of millimeters");
	def.type = coBool;
	def.tooltip = L("Use inches instead of millimeters for the object's size");
	def.set_default_value(new ConfigOptionBool{ app_config->get("use_inches") == "1" });
	option = Option(def, "use_inches");
	m_optgroup_general->append_single_option_line(option);
*/

	def.label = L("Ask for unsaved changes when closing application");
	def.type = coBool;
	def.tooltip = L("When closing the application, always ask for unsaved changes");
	def.set_default_value(new ConfigOptionBool{ app_config->get("default_action_on_close_application") == "none" });
	option = Option(def, "default_action_on_close_application");
	m_optgroup_general->append_single_option_line(option);

	def.label = L("Ask for unsaved changes when selecting new preset");
	def.type = coBool;
	def.tooltip = L("Always ask for unsaved changes when selecting new preset");
	def.set_default_value(new ConfigOptionBool{ app_config->get("default_action_on_select_preset") == "none" });
	option = Option(def, "default_action_on_select_preset");
	m_optgroup_general->append_single_option_line(option);

    // Show/Hide splash screen
	def.label = L("Show splash screen");
	def.type = coBool;
	def.tooltip = L("Show splash screen");
	def.set_default_value(new ConfigOptionBool{ app_config->get("show_splash_screen") == "1" });
	option = Option(def, "show_splash_screen");
	m_optgroup_general->append_single_option_line(option);

	m_optgroup_general->activate();

	m_optgroup_camera = std::make_shared<ConfigOptionsGroup>(this, _L("Camera"));
	m_optgroup_camera->label_width = 40;
	m_optgroup_camera->m_on_change = [this](t_config_option_key opt_key, boost::any value) {
		m_values[opt_key] = boost::any_cast<bool>(value) ? "1" : "0";
	};

	def.label = L("Use perspective camera");
	def.type = coBool;
	def.tooltip = L("If enabled, use perspective camera. If not enabled, use orthographic camera.");
	def.set_default_value(new ConfigOptionBool{ app_config->get("use_perspective_camera") == "1" });
	option = Option(def, "use_perspective_camera");
	m_optgroup_camera->append_single_option_line(option);

	def.label = L("Use free camera");
	def.type = coBool;
	def.tooltip = L("If enabled, use free camera. If not enabled, use constrained camera.");
	def.set_default_value(new ConfigOptionBool(app_config->get("use_free_camera") == "1"));
	option = Option(def, "use_free_camera");
	m_optgroup_camera->append_single_option_line(option);

	def.label = L("Reverse direction of zoom with mouse wheel");
	def.type = coBool;
	def.tooltip = L("If enabled, reverses the direction of zoom with mouse wheel");
	def.set_default_value(new ConfigOptionBool(app_config->get("reverse_mouse_wheel_zoom") == "1"));
	option = Option(def, "reverse_mouse_wheel_zoom");
	m_optgroup_camera->append_single_option_line(option);

	m_optgroup_camera->activate();

	m_optgroup_gui = std::make_shared<ConfigOptionsGroup>(this, _L("GUI"));
	m_optgroup_gui->label_width = 40;
	m_optgroup_gui->m_on_change = [this](t_config_option_key opt_key, boost::any value) {
        if (opt_key == "suppress_hyperlinks")
            m_values[opt_key] = boost::any_cast<bool>(value) ? "1" : "";
        else
            m_values[opt_key] = boost::any_cast<bool>(value) ? "1" : "0";

		if (opt_key == "use_custom_toolbar_size") {
			m_icon_size_sizer->ShowItems(boost::any_cast<bool>(value));
			this->layout();
		}
	};

	if (is_editor) {
		def.label = L("Show sidebar collapse/expand button");
		def.type = coBool;
		def.tooltip = L("If enabled, the button for the collapse sidebar will be appeared in top right corner of the 3D Scene");
		def.set_default_value(new ConfigOptionBool{ app_config->get("show_collapse_button") == "1" });
		option = Option(def, "show_collapse_button");
		m_optgroup_gui->append_single_option_line(option);

		def.label = L("Use custom size for toolbar icons");
		def.type = coBool;
		def.tooltip = L("If enabled, you can change size of toolbar icons manually.");
		def.set_default_value(new ConfigOptionBool{ app_config->get("use_custom_toolbar_size") == "1" });
		option = Option(def, "use_custom_toolbar_size");
		m_optgroup_gui->append_single_option_line(option);
	}

	def.label = L("Sequential slider applied only to top layer");
	def.type = coBool;
	def.tooltip = L("If enabled, changes made using the sequential slider, in preview, apply only to gcode top layer. "
					"If disabled, changes made using the sequential slider, in preview, apply to the whole gcode.");
	def.set_default_value(new ConfigOptionBool{ app_config->get("seq_top_layer_only") == "1" });
	option = Option(def, "seq_top_layer_only");
	m_optgroup_gui->append_single_option_line(option);

	def.label = L("Suppress to open hyperlink in browser");
	def.type = coBool;
	def.tooltip = L("If enabled, the descriptions of configuration parameters in settings tabs woldn't work as hyperlinks. "
					"If disabled, the descriptions of configuration parameters in settings tabs will work as hyperlinks.");
	def.set_default_value(new ConfigOptionBool{ app_config->get("suppress_hyperlinks") == "1" });
	option = Option(def, "suppress_hyperlinks");
	m_optgroup_gui->append_single_option_line(option);

	m_optgroup_gui->activate();

	if (is_editor) {
		create_icon_size_slider();
		m_icon_size_sizer->ShowItems(app_config->get("use_custom_toolbar_size") == "1");

		create_settings_mode_widget();
	}

	if (is_editor) {
#if ENABLE_ENVIRONMENT_MAP
		m_optgroup_render = std::make_shared<ConfigOptionsGroup>(this, _L("Render"));
		m_optgroup_render->label_width = 40;
		m_optgroup_render->m_on_change = [this](t_config_option_key opt_key, boost::any value) {
			m_values[opt_key] = boost::any_cast<bool>(value) ? "1" : "0";
		};

		def.label = L("Use environment map");
		def.type = coBool;
		def.tooltip = L("If enabled, renders object using the environment map.");
		def.set_default_value(new ConfigOptionBool{ app_config->get("use_environment_map") == "1" });
		option = Option(def, "use_environment_map");
		m_optgroup_render->append_single_option_line(option);

		m_optgroup_render->activate();
#endif // ENABLE_ENVIRONMENT_MAP
	}

	auto sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_optgroup_general->sizer, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);
	sizer->Add(m_optgroup_camera->sizer, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);
	sizer->Add(m_optgroup_gui->sizer, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);
#if ENABLE_ENVIRONMENT_MAP
	if (m_optgroup_render != nullptr)
		sizer->Add(m_optgroup_render->sizer, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);
#endif // ENABLE_ENVIRONMENT_MAP

    SetFont(wxGetApp().normal_font());

	auto buttons = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	wxButton* btn = static_cast<wxButton*>(FindWindowById(wxID_OK, this));
	btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { accept(); });
	sizer->Add(buttons, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);

	SetSizer(sizer);
	sizer->SetSizeHints(this);
}

void PreferencesDialog::accept()
{
    if (m_values.find("no_defaults") != m_values.end()) {
        warning_catcher(this, wxString::Format(_L("You need to restart %s to make the changes effective."), SLIC3R_APP_NAME));
	}

    auto app_config = get_app_config();

	m_seq_top_layer_only_changed = false;
	if (auto it = m_values.find("seq_top_layer_only"); it != m_values.end())
		m_seq_top_layer_only_changed = app_config->get("seq_top_layer_only") != it->second;

	m_settings_layout_changed = false;
	for (const std::string& key : {"old_settings_layout_mode",
								   "new_settings_layout_mode",
								   "dlg_settings_layout_mode" })
	{
	    auto it = m_values.find(key);
	    if (it != m_values.end() && app_config->get(key) != it->second) {
			m_settings_layout_changed = true;
			break;
	    }
	}

	for (const std::string& key : {"default_action_on_close_application", "default_action_on_select_preset"})
	{
	    auto it = m_values.find(key);
		if (it != m_values.end() && it->second != "none" && app_config->get(key) != "none")
			m_values.erase(it); // we shouldn't change value, if some of those parameters was selected, and then deselected
	}

	for (std::map<std::string, std::string>::iterator it = m_values.begin(); it != m_values.end(); ++it)
		app_config->set(it->first, it->second);

	app_config->save();
	EndModal(wxID_OK);

	if (m_settings_layout_changed)
		;// application will be recreated after Preference dialog will be destroyed
	else
	    // Nothify the UI to update itself from the ini file.
        wxGetApp().update_ui_from_settings();
}

void PreferencesDialog::on_dpi_changed(const wxRect &suggested_rect)
{
	m_optgroup_general->msw_rescale();
	m_optgroup_camera->msw_rescale();
	m_optgroup_gui->msw_rescale();

    msw_buttons_rescale(this, em_unit(), { wxID_OK, wxID_CANCEL });

    layout();
}

void PreferencesDialog::layout()
{
    const int em = em_unit();

    SetMinSize(wxSize(47 * em, 28 * em));
    Fit();

    Refresh();
}

void PreferencesDialog::create_icon_size_slider()
{
    const auto app_config = get_app_config();

    const int em = em_unit();

    m_icon_size_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxWindow* parent = m_optgroup_gui->ctrl_parent();

    if (isOSX)
        // For correct rendering of the slider and value label under OSX
        // we should use system default background
        parent->SetBackgroundStyle(wxBG_STYLE_ERASE);

    auto label = new wxStaticText(parent, wxID_ANY, _L("Icon size in a respect to the default size") + " (%) :");

    m_icon_size_sizer->Add(label, 0, wxALIGN_CENTER_VERTICAL| wxRIGHT | (isOSX ? 0 : wxLEFT), em);

    const int def_val = atoi(app_config->get("custom_toolbar_size").c_str());

    long style = wxSL_HORIZONTAL;
    if (!isOSX)
        style |= wxSL_LABELS | wxSL_AUTOTICKS;

    auto slider = new wxSlider(parent, wxID_ANY, def_val, 30, 100, 
                               wxDefaultPosition, wxDefaultSize, style);

    slider->SetTickFreq(10);
    slider->SetPageSize(10);
    slider->SetToolTip(_L("Select toolbar icon size in respect to the default one."));

    m_icon_size_sizer->Add(slider, 1, wxEXPAND);

    wxStaticText* val_label{ nullptr };
    if (isOSX) {
        val_label = new wxStaticText(parent, wxID_ANY, wxString::Format("%d", def_val));
        m_icon_size_sizer->Add(val_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, em);
    }

    slider->Bind(wxEVT_SLIDER, ([this, slider, val_label](wxCommandEvent e) {
        auto val = slider->GetValue();
        m_values["custom_toolbar_size"] = (boost::format("%d") % val).str();

        if (val_label)
            val_label->SetLabelText(wxString::Format("%d", val));
    }), slider->GetId());

    for (wxWindow* win : std::vector<wxWindow*>{ slider, label, val_label }) {
        if (!win) continue;         
        win->SetFont(wxGetApp().normal_font());

        if (isOSX) continue; // under OSX we use wxBG_STYLE_ERASE
        win->SetBackgroundStyle(wxBG_STYLE_PAINT);
    }

	m_optgroup_gui->sizer->Add(m_icon_size_sizer, 0, wxEXPAND | wxALL, em);
}

void PreferencesDialog::create_settings_mode_widget()
{
	wxString choices[] = {	_L("Old regular layout with the tab bar"),
							_L("New layout, access via settings button in the top menu"),
							_L("Settings in non-modal window")		};

	auto app_config = get_app_config();
	int selection = app_config->get("old_settings_layout_mode") == "1" ? 0 :
	                app_config->get("new_settings_layout_mode") == "1" ? 1 :
	                app_config->get("dlg_settings_layout_mode") == "1" ? 2 : 0;

	wxWindow* parent = m_optgroup_gui->ctrl_parent();

	m_layout_mode_box = new wxRadioBox(parent, wxID_ANY, _L("Layout Options"), wxDefaultPosition, wxDefaultSize, WXSIZEOF(choices), choices,
		3, wxRA_SPECIFY_ROWS);
	m_layout_mode_box->SetFont(wxGetApp().normal_font());
	m_layout_mode_box->SetSelection(selection);

	m_layout_mode_box->Bind(wxEVT_RADIOBOX, [this](wxCommandEvent& e) {
		int selection = e.GetSelection();

		m_values["old_settings_layout_mode"] = boost::any_cast<bool>(selection == 0) ? "1" : "0";
		m_values["new_settings_layout_mode"] = boost::any_cast<bool>(selection == 1) ? "1" : "0";
		m_values["dlg_settings_layout_mode"] = boost::any_cast<bool>(selection == 2) ? "1" : "0";
	});

	auto sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_layout_mode_box, 1, wxALIGN_CENTER_VERTICAL);

	m_optgroup_gui->sizer->Add(sizer, 0, wxEXPAND);
}


} // GUI
} // Slic3r
