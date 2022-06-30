/***************************************************
 * A simple GUI program for controlling SIRIUS 
 * Author: Rikel CHAKMA
 * Date : 06/06/2022
 ***************************************************/
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <map>

using namespace std;
//--------------------
// Global variables
// ------------------
static std::string command_init_func;
static std::string command_misc_func;
static std::string command2;
static std::string commandTempUnit;
static std::string animationInterval = "1";
static std::string boardList_str;
std::vector<int> boardList;
std::map<int, bool> boardList_check;
std::map<int, bool> boardList_off;
const gchar *user_entryData;
const gchar *user_entryData2;

//---------------------------------------
// Forward declarations of the functions
//----------------------------------------
void get_nActiveBoards();
void check_Boards_ONOFF_status();


bool check_board_is_online(int b)
{ 
	bool check = false;
	for(size_t i = 0; i < boardList.size(); i++)
	{
		if(boardList.at(i) == b) check = true;
	}
	return check;
}

void button_pressed(GtkWidget *widget, gpointer window) 
{

	gchar *str = g_strdup_printf("%s button clicked...", gtk_button_get_label(GTK_BUTTON(widget)));
	gtk_statusbar_push(GTK_STATUSBAR(window), gtk_statusbar_get_context_id(GTK_STATUSBAR(window), str), str);
	g_free(str);
}

void temperature_animation_interval_value(GtkRange *range, gpointer window)
{

	gdouble value = gtk_range_get_value(range);
	gchar *str = g_strdup_printf("%.f", value);
	animationInterval = str;   
	gtk_label_set_text(GTK_LABEL(window), str);
	g_free(str);
}

void miscellaneous_function(){
	if(command_misc_func.compare("Check ON/OFF Status of the Boards") == 0)
		check_Boards_ONOFF_status();
	else if(command_misc_func.compare("Reload Active Boards") ==0)
		get_nActiveBoards();
	else if(command_misc_func.compare("Ping Network") == 0)
		system("./bin/lping.sh");
	else if(command_misc_func.compare("Scan Network") == 0)
		system("./bin/lscan.sh");
	else if(command_misc_func.compare("Check Readiness") == 0)
		system("./bin/numexo2_state.sh");
	else if(command_misc_func.compare("Show Driver State") == 0)
		system("./bin/numexo2_dev_sirius.sh");
	else if(command_misc_func.compare("Show Framework Version") == 0)
		system("./bin/numexo2_version.sh");
	else if(command_misc_func.compare("Show Narval State") == 0)
		system("./bin/narval_state.sh");
	else
		std::cout<<"Select from the drop-down list....\n";
}


void execute_init_function(){

	if(command_init_func.compare("Setup GTS") == 0)
		system("./SetupGTS");
	else if(command_init_func.compare("Configure NUMEXO2 boards") ==0)
		system("./config_v6_sirius.csh");
	else if(command_init_func.compare("Auto Initialization") ==0){
		system("./SetupGTS");
		system("./config_v6_sirius.csh");
	}
	else
		std::cout<<"Select from the drop-down list....\n";
}


void execute_numexo2_reset(GtkWidget *wid,gpointer data){

	user_entryData = gtk_entry_get_text(GTK_ENTRY(data));
	std::string str(user_entryData);
	if((str.compare("ALL") == 0) || (str.compare("*") == 0)){
		char cmd[300];
		if(command2.compare("ON") == 0)
		{

			sprintf(cmd, "./reset_v6_config_withAcq.csh %s", boardList_str.c_str());cout<< "command "<<cmd<<endl;
			system(cmd);
		}    

		else if(command2.compare("OFF") == 0){
			sprintf(cmd, "./reset_v6_config_noAcq.csh %s", boardList_str.c_str());cout<< "command "<<cmd<<endl;
			system(cmd);
		}

		else
			std::cout<<"Select from the drop-down list....\n";
	}
	else{
		char cmd[400];
		int bid;char *ptr;
		bid = (int) strtol(str.c_str(), &ptr, 10);
		if(check_board_is_online(bid)){
			if(command2.compare("ON") == 0)
			{

				sprintf(cmd, "./reset_v6_config_withAcq.csh %s", str.c_str());
				cout<< "command "<<cmd<<endl;
				system(cmd);
			}    

			else if(command2.compare("OFF") == 0){
				sprintf(cmd, "./reset_v6_config_noAcq.csh %s", str.c_str());cout<< "command "<<cmd<<endl;
				system(cmd);
			}

			else
				std::cout<<"Select from the drop-down list....\n";
		}else{
			cout<<"please enter a correct value from the number of active boards"<<endl;
			for(int i = 0; i < boardList.size();i++) cout<<"board  "<<i<<" : "<<boardList[i]<<endl;
		}

	}

}



void execute_Tmux_open(){
	system("source launch_tmux_TelnetViewer.csh");
}


void execute_Tmux_close(){
	system("source quit_tmux_TelnetViewer.sh");
}


void execute_Temp_Draw(){
	if((commandTempUnit.compare("sec") == 0) || (commandTempUnit.compare("min") == 0) || (commandTempUnit.compare("hr") == 0)){
		char command[400];
		sprintf(command, "source launch_temp_Viewer.csh %d %s", std::stoi(animationInterval), commandTempUnit.c_str());
		//sprintf(command, "python numexo2_temperature.py %d %s", std::stoi(animationInterval), commandTempUnit.c_str());
		system(command);
	}
	else
		std::cout<<"Select the unit from the drop-down list....\n";
}

void execute_Temp_Kill(){
	system("source kill_numexo2_temperature.sh");
}

void execute_numexo2_TempGet(GtkWidget *wid,gpointer data){
	user_entryData2 = gtk_entry_get_text(GTK_ENTRY(data));
	std::string str(user_entryData2);

	if((str.compare("ALL") == 0) || (str.compare("*") == 0)){
		system("./bin/numexo2_temperature.sh");
	}
	else{
		char command[400];
		char *ptr;
		int bid;
		bid = (int) strtol(str.c_str(), &ptr, 10);
		if(check_board_is_online(bid)){
			sprintf(command, "./bin/numexo2_temperature.sh %s", str.c_str());
			system(command);  
		}
		else{
			cout<<"please enter a correct value from the number of active boards"<<endl;
			for(int i = 0; i < boardList.size();i++) cout<<"board  "<<i<<" : "<<boardList[i]<<endl;
		}

	}
}
void misc_func_selected(GtkWidget *widget, gpointer window) 
{
	gchar *text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
	gtk_label_set_text(GTK_LABEL(window), text);
	std::string str(text);
	command_misc_func = str;
	g_free(text);
}

void init_func_selected(GtkWidget *widget, gpointer window) 
{
	gchar *text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
	gtk_label_set_text(GTK_LABEL(window), text);
	std::string str(text);
	command_init_func = str;
	g_free(text);
}

void acq_state_selected(GtkWidget *widget, gpointer window) {

	gchar *text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
	gtk_label_set_text(GTK_LABEL(window), text);
	std::string str(text);
	command2 = str;
	g_free(text);
}

void comboTemp_selected(GtkWidget *widget, gpointer window) {

	gchar *text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
	gtk_label_set_text(GTK_LABEL(window), text);
	std::string str(text);
	commandTempUnit = str;
	g_free(text);
}


void check_Boards_ONOFF_status(){
	//get_nActiveBoards();
	boardList_check.clear();
	std::vector<int> temp_check;
	std::vector<int> temp;
	int board;
	std::ifstream file;
	file.open("numexo2BoardList.txt", std::ifstream::in);
	if(file.is_open()){
		while(!file.eof()){
			file >> board;
			temp_check.push_back(board);
			std::pair<int, bool> a(board, 0);

			boardList_check.insert(a);
		}
		file.close();

	}else{
		cerr<<"numexo2 Board list file does not exist or cannot be opened...."<<endl;
	} 

	if(boardList.size() == 0) std::cout<<"no Numexo2 boards is online.."<<std::endl;

	if(boardList.size() >0 && boardList_check.size() > 0){
		//list of active boards
		for(size_t i =0; i < boardList.size();i++){
			temp.push_back(boardList.at(i));
		}

		//compare
		int i =0;
		for(int i = 0; i < temp.size(); i++){
			for(size_t j = 0; j < temp_check.size(); j++){
				if(temp.at(i) == temp_check.at(j)){
					boardList_check[temp.at(i)] = true;
					break;
				}

			}

		}


	}
	for(auto &it : boardList_check){
		if(it.second == true){
			cout<<"Board: "<<it.first <<"  status: "<<"\033[1;32m ON \033[0m"<<endl;
		}
		else{
			boardList_off.insert(it);
			cout<<"Board: "<<it.first <<"  status: "<<"\033[1;31m OFF \033[0m"<<endl;
		}
	}
	//warn
	cout<<"=======================================\n";
	if(boardList_off.size()> 0){
		if(boardList_off.size()> 1) cout<<boardList_off.size()<<" out of "<<boardList_check.size()<<" Boards are"<<"\033[1;31m OFF. \033[0m"<<"\nPlease restart the Crates..."<<endl;
		else cout<<boardList_off.size()<<" out of "<<boardList_check.size()<<" Boards is"<<"\033[1;31m OFF. \033[0m"<<"\nPlease restart the Crates..."<<endl;

		for(auto &it : boardList_off){

			cout<<"Board: "<<it.first <<" is  "<<"\033[1;31m OFF \033[0m"<<endl;
		}

	}else{
		cout<<"All the listed boards are"<<"\033[1;32m ON \033[0m"<<endl;
	}
	cout<<"=======================================\n";
	temp.clear();
	temp_check.clear();
}


void get_nActiveBoards(){

	std::string bList = "";
	boardList.clear();
	int i; char ch;
	FILE *pipe = popen("./bin/numexo2_list.sh", "r");
	if (pipe==NULL) perror ("Error opening file");
	else
	{
		do {
			i = fgetc (pipe);
			ch = (char)i;
			bList.append(1, ch);
		} while (i != EOF);
		fclose (pipe);
	}
	std::stringstream ss(bList); string cc; int bid;
	while(!ss.eof()){
		ss >> cc;
		char * pEnd;
		bid = strtol (cc.c_str(),&pEnd,10);
		if(bid) boardList.push_back(bid);
	}
	for(int i = 0; i < boardList.size();i++){ boardList_str +=  std::to_string(boardList[i]); boardList_str += " ";}
	//for(int i = 0; i < boardList.size();i++) cout<<"board  "<<i<<" : "<<boardList[i]<<endl;

}

void display_about( GtkWidget*widget, gpointer data ){
	GError *error = NULL;

	/*GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(
	  "expsiriuslogo.png", 250, 150, TRUE, &error);
	  if (pixbuf == NULL) {
	  g_printerr ("Error loading file: #%d %s\n", error->code, error->message);
	  g_error_free (error);
	  exit (1);
	  }
	  */	GtkWidget * dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Exp@SIRIUS");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "Author: Rikel CHAKMA");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "An application for controlling SIRIUS setup. \n\n This software has been developed using gtk2. It executes the shell scripts written mostly by Sébastien COUDERT(GANIL), some by me and Thomas CHAMINADE(IRFU/Saclay). \n\n This is a free Software, you can redistribute or modify it, under GNU General Public Liscence as published by the Free Software Foundation.  ");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "");
	//gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	//g_object_unref(pixbuf), pixbuf = NULL;
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}


int main(int argc, char *argv[]) {
	//-------------	
	//declration
	//-----------
	GtkWidget *window;//main window
	GtkWidget *vbox;// vertical box to hold all the horizontal boxes
	GtkWidget *halign;//alignment
	GtkWidget *balign;
	//------------------
	//Introduction section
	//--------------------
	GtkWidget *hboxIntro;// horizontal box for the Intro section
	GtkWidget *button_about;
	GtkWidget *labelIntro;
	GtkWidget* image_ganillogo; 
	//-----------------------------
	// Setup initialization section
	// ----------------------------
	GtkWidget *hboxInit;//horizontal box for init section
	GtkWidget *labelInit;//label for this section
	GtkWidget *labelInit_combo_selection;//label to show which function has been selected
	GtkWidget *button_okInit;// ok button to execute the selected function
	GtkWidget *combo_init_func;// combo for a list of functions
	//-------------------------
	// Misc. function section
	// ------------------------
	GtkWidget *hboxMiscFunc;//horizontal box for this section
	GtkWidget *labelMiscFunc;//label for this section
	GtkWidget *labelMiscFunc_combo_selected;//label for the selected function
	GtkWidget *combo_misc_func;//list of functions
	GtkWidget *button_ok_misc_func;//button to execute selected function
	//---------------------------------
	// Reset Numexo2 boards section
	//----------------------------------
	GtkWidget *hboxReset;//horizontal box for this section
	GtkWidget *user_entry_reset_board;// user entry for the board number to be reset
	GtkWidget *combo_acq;//Acq ON/OFF state
	GtkWidget *labelReset;//Numexo board number
	GtkWidget *label_acq;//label for Acquisition state
	GtkWidget *label_acq_combo_selected;// label for acq state selected from the list
	GtkWidget *button_reset;// ok button to execute
	//----------------------
	// Telnet viewer section
	//-----------------------
	GtkWidget *hboxTmux;//horizontal box for thi section
	GtkWidget *labelTmux;//label for this section
	GtkWidget *button_Tmux_open;//button to open the telnet viewer using Tmux
	GtkWidget *button_Tmux_close;// button for closing the tumux session
	//----------------------------------
	// Numexo2 board temperature section
	//----------------------------------
	//command line section
	GtkWidget *hboxTemp_cl;
	GtkWidget *labelTemp_board;
	GtkWidget *user_entry_temp_board;
	GtkWidget *button_TempGet;
	//Graphical section
	GtkWidget *hboxTemp_gr;
	GtkWidget *labelTemp_gr;
	GtkWidget *animationIntervalScale;
	GtkWidget *label_interval_scale;
	GtkWidget *comboAnimationIntervalTimeUnit;
	GtkWidget *label_time_unit;
	GtkWidget *button_Temp_Draw;
	GtkWidget *button_Temp_Kill;
	//-----------------
	// Quit
	//----------------
	GtkWidget *hbox_quit;
	GtkWidget *button_quit;
	//---------------
	// Status bar
	// --------------
	GtkWidget *statusbar;

	//-------------------------
	// gtk init
	// --------------------
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), argv[1]);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);
	gtk_window_set_default_size(GTK_WINDOW(window), 100, 200); 
	// to be able to destroy the window by clicking x
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(gtk_main_quit), NULL);


	//------------------
	//init verical boxes
	//------------
	vbox = gtk_vbox_new(FALSE, 20);
	//get the list of active boards
	get_nActiveBoards();
	//check_Boards_ONOFF_status();
	//---------------------------
	// Introduction section
	//--------------------------
	hboxIntro = gtk_hbox_new(FALSE, 30);
	//ganil logo	
	GError *error = NULL;
	GdkPixbuf *pix_buf = gdk_pixbuf_new_from_file_at_scale("ganillogo.png", 150,50, TRUE, &error);
	if (pix_buf == NULL) {
		g_printerr ("Error loading file: #%d %s\n", error->code, error->message);
		g_error_free (error);
		exit (1);
	}
	image_ganillogo = gtk_image_new_from_pixbuf (pix_buf);
	gtk_box_pack_start(GTK_BOX(hboxIntro), image_ganillogo, TRUE, TRUE, 0);
	//intro label
	labelIntro = gtk_label_new(NULL);
	const char * str = "Sectroscopy and Identification of Rare Isotopes using S3 (SIRIUS)";
	const char *format = "<span foreground= \"black\" size = \"x-large\" background=\"light green\" > \%s </span>";
	//span style \"italic\"
	//
	char* markup = g_markup_printf_escaped(format,str); 
	gtk_label_set_markup(GTK_LABEL(labelIntro),markup);	
	gtk_label_set_justify(GTK_LABEL(labelIntro), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start(GTK_BOX(hboxIntro), labelIntro, TRUE, TRUE, 0);
	//about button
	button_about = gtk_button_new_with_label ("About");
	gtk_widget_set_size_request(button_about, 70, 30 );
	gtk_box_pack_start(GTK_BOX(hboxIntro), button_about, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT(button_about), "button-press-event",
			G_CALLBACK(display_about), (gpointer)window);
	//gtk_box_pack_start(GTK_BOX(vbox), hboxIntro,TRUE, TRUE, 0);
	halign = gtk_alignment_new(1, 0, 0, 0);
	gtk_container_add(GTK_CONTAINER(halign), hboxIntro);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//-----------------------------------
	// Numexo2 Initialization section
	// ---------------------------------
	hboxInit = gtk_hbox_new(TRUE, 5);
	//functionality label	
	labelInit = gtk_label_new("Setup Initialization");
	gtk_box_pack_start(GTK_BOX(hboxInit), labelInit, TRUE, TRUE, 0);
	// List of functions
	labelInit_combo_selection = gtk_label_new("");
	combo_init_func = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_init_func), "Setup GTS");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_init_func), "Configure NUMEXO2 boards");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_init_func), "Auto Initialization");
	gtk_box_pack_start(GTK_BOX(hboxInit), combo_init_func,TRUE, TRUE, 0);
	//to select from the list
	g_signal_connect(G_OBJECT(combo_init_func), "changed", G_CALLBACK(init_func_selected), (gpointer) labelInit_combo_selection);
	// OK button for single function execution
	button_okInit = gtk_button_new_with_label ("Apply");
	gtk_widget_set_size_request(button_okInit, 70, 30 );
	g_signal_connect (G_OBJECT(button_okInit), "clicked",
			G_CALLBACK(execute_init_function), NULL);
	gtk_box_pack_start(GTK_BOX(hboxInit), button_okInit, TRUE, TRUE, 0);
	//position
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxInit);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//-----------------------------------
	//Misc. function section
	//--------------------------------------	
	hboxMiscFunc = gtk_hbox_new(TRUE, 5);
	labelMiscFunc = gtk_label_new("Miscellaneous function");
	gtk_box_pack_start(GTK_BOX(hboxMiscFunc), labelMiscFunc, TRUE, TRUE, 0);
	// List of functions
	labelMiscFunc_combo_selected = gtk_label_new("");
	combo_misc_func = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Check ON/OFF Status of the Boards");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Reload Active Boards");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Ping Network");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Scan Network");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Check Readiness");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Show Driver State");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Reboot Numexos");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Show Framework Version");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Show Narval State");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Show Crate Status");
	//	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Show Crate Temperature");
	//	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Turn Crate ON");
	//	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Turn Crate OFF");
	gtk_box_pack_start(GTK_BOX(hboxMiscFunc), combo_misc_func,TRUE, TRUE, 0);
	//to select from the list
	g_signal_connect(G_OBJECT(combo_misc_func), "changed", G_CALLBACK(misc_func_selected), (gpointer) labelMiscFunc_combo_selected);
	// OK button for single function execution
	button_ok_misc_func = gtk_button_new_with_label ("OK");
	gtk_widget_set_size_request(button_ok_misc_func, 70, 30 );
	g_signal_connect (G_OBJECT(button_ok_misc_func), "clicked",
			G_CALLBACK(miscellaneous_function), NULL);
	gtk_box_pack_start(GTK_BOX(hboxMiscFunc), button_ok_misc_func, TRUE, TRUE, 0);
	//position
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxMiscFunc);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//-------------------------------------
	// Reset the numexos section
	//---------------------------------
	hboxReset = gtk_hbox_new(TRUE, 5);
	// label Numexo
	labelReset = gtk_label_new("Reset Numexo2-");
	gtk_box_pack_start(GTK_BOX(hboxReset), labelReset, TRUE, TRUE, 0);
	// user entry
	user_entry_reset_board = gtk_entry_new();
	//user_entryData = gtk_entry_get_text(GTK_ENTRY(user_entry_reset_board));
	//gtk_entry_set_placeholder_text(GTK_ENTRY(user_entry_reset_board),"ALL"); //only available in gtk3+
	gtk_box_pack_start(GTK_BOX(hboxReset), user_entry_reset_board, TRUE, TRUE, 0);

	label_acq = gtk_label_new("Acquision state");
	gtk_box_pack_start(GTK_BOX(hboxReset), label_acq, TRUE, TRUE, 0);
	// List of acquision mode
	label_acq_combo_selected = gtk_label_new("");
	combo_acq = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_acq), "ON");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_acq), "OFF");
	gtk_box_pack_start(GTK_BOX(hboxReset), combo_acq, TRUE, TRUE, 0);
	// select from the list
	g_signal_connect(G_OBJECT(combo_acq), "changed", G_CALLBACK(acq_state_selected), (gpointer) label_acq_combo_selected);
	// button reset numexo
	button_reset = gtk_button_new_with_label("Reset");
	gtk_widget_set_size_request(button_reset, 70, 30 );
	g_signal_connect (G_OBJECT(button_reset), "clicked",
			G_CALLBACK(execute_numexo2_reset), (gpointer) user_entry_reset_board);
	gtk_box_pack_start(GTK_BOX(hboxReset), button_reset, TRUE, TRUE, 0);
	//position the button
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxReset);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//------------------------
	// Telnet viewer
	//-----------------------
	hboxTmux = gtk_hbox_new(TRUE, 5);
	labelTmux = gtk_label_new("Telnet Viewer");
	gtk_box_pack_start(GTK_BOX(hboxTmux), labelTmux, TRUE, TRUE, 0);
	// open Tmux viewer
	button_Tmux_open = gtk_button_new_with_label("Open");
	gtk_widget_set_size_request(button_Tmux_open, 70, 30 );
	g_signal_connect (G_OBJECT(button_Tmux_open), "clicked",
			G_CALLBACK(execute_Tmux_open), NULL);
	gtk_box_pack_start(GTK_BOX(hboxTmux), button_Tmux_open, TRUE, TRUE, 0);
	// Close Tmux viewer
	button_Tmux_close = gtk_button_new_with_label("Close");
	gtk_widget_set_size_request(button_Tmux_close, 70, 30 );
	g_signal_connect (G_OBJECT(button_Tmux_close), "clicked",
			G_CALLBACK(execute_Tmux_close), NULL);
	gtk_box_pack_start(GTK_BOX(hboxTmux), button_Tmux_close, TRUE, TRUE, 0);
	//position the button
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxTmux);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//------------------------
	// Get Temp numexos
	//-----------------------
	hboxTemp_cl = gtk_hbox_new(TRUE, 5);
	// label Numexo
	labelTemp_board = gtk_label_new("Temperature of NUEMXO2-");
	gtk_box_pack_start(GTK_BOX(hboxTemp_cl), labelTemp_board, TRUE, TRUE, 0);
	// user entry
	user_entry_temp_board = gtk_entry_new();
	// user_entryData2 = gtk_entry_get_text(GTK_ENTRY(user_entry_temp_board));
	//gtk_entry_set_placeholder_text(GTK_ENTRY(user_entry_temp_board),"ALL"); //only available in gtk3+
	gtk_box_pack_start(GTK_BOX(hboxTemp_cl), user_entry_temp_board, TRUE, TRUE, 0);
	// numexo
	button_TempGet = gtk_button_new_with_label("Measure");
	gtk_widget_set_size_request(button_TempGet, 70, 30 );
	g_signal_connect (G_OBJECT(button_TempGet), "clicked",
			G_CALLBACK(execute_numexo2_TempGet), (gpointer) user_entry_temp_board);
	gtk_box_pack_start(GTK_BOX(hboxTemp_cl), button_TempGet, TRUE, TRUE, 0);
	//position the button
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxTemp_cl);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//------------------------
	// Graphical Plot Temp
	//-----------------------
	hboxTemp_gr = gtk_hbox_new(TRUE, 5);
	labelTemp_gr = gtk_label_new("Temperature = f(time)");
	gtk_box_pack_start(GTK_BOX(hboxTemp_gr), labelTemp_gr, TRUE, TRUE, 0);
	//scale to control the interval of animation
	animationIntervalScale = gtk_hscale_new_with_range(1, 60, 1);
	gtk_scale_set_draw_value(GTK_SCALE(animationIntervalScale), FALSE);
	gtk_widget_set_size_request(animationIntervalScale, 150, -1);
	label_interval_scale = gtk_label_new("   ");
	gtk_misc_set_alignment(GTK_MISC(label_interval_scale), 0.0, 1);
	g_signal_connect(animationIntervalScale, "value-changed",
			G_CALLBACK(temperature_animation_interval_value), label_interval_scale);      
	gtk_box_pack_start(GTK_BOX(hboxTemp_gr), animationIntervalScale, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hboxTemp_gr), label_interval_scale, TRUE, TRUE, 0);
	// List of units
	label_time_unit = gtk_label_new("");
	comboAnimationIntervalTimeUnit = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboAnimationIntervalTimeUnit), "sec");
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboAnimationIntervalTimeUnit), "min");
	gtk_combo_box_append_text(GTK_COMBO_BOX(comboAnimationIntervalTimeUnit), "hr");
	gtk_box_pack_start(GTK_BOX(hboxTemp_gr),comboAnimationIntervalTimeUnit ,TRUE, TRUE, 0);
	//to select from the list
	g_signal_connect(G_OBJECT(comboAnimationIntervalTimeUnit), "changed", G_CALLBACK(comboTemp_selected), (gpointer) label_time_unit);
	// draw button
	button_Temp_Draw = gtk_button_new_with_label("Draw");
	gtk_widget_set_size_request(button_Temp_Draw, 70, 30 );
	g_signal_connect (G_OBJECT(button_Temp_Draw), "clicked",
			G_CALLBACK(execute_Temp_Draw), NULL);
	gtk_box_pack_start(GTK_BOX(hboxTemp_gr), button_Temp_Draw, TRUE, TRUE, 0);
	// Close the temp app button
	button_Temp_Kill = gtk_button_new_with_label("Close");
	gtk_widget_set_size_request(button_Temp_Kill, 70, 30 );
	g_signal_connect (G_OBJECT(button_Temp_Kill), "clicked",
			G_CALLBACK(execute_Temp_Kill), NULL);
	gtk_box_pack_start(GTK_BOX(hboxTemp_gr), button_Temp_Kill, TRUE, TRUE, 0);
	//position the box
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxTemp_gr);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//------------
	//Quit button
	//----------------	
	hbox_quit = gtk_hbox_new(TRUE, 5);
	button_quit = gtk_button_new_with_label("Quit");
	g_signal_connect(G_OBJECT(button_quit), "clicked", 
			G_CALLBACK(gtk_main_quit), G_OBJECT(window));
	gtk_box_pack_start (GTK_BOX (hbox_quit), button_quit, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox_quit,TRUE, TRUE, 0);
	//----------------------------------
	//status bar
	//----------------------
	halign = gtk_alignment_new(1, 1, 1, 1);
	statusbar = gtk_statusbar_new();
	gtk_container_add(GTK_CONTAINER(halign), statusbar);
	gtk_box_pack_start(GTK_BOX(vbox), halign, FALSE, FALSE, 0);

	g_signal_connect(G_OBJECT(button_okInit), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_ok_misc_func), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_reset), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_quit), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_TempGet), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_Tmux_open), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_Tmux_close), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_Temp_Draw), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_Temp_Kill), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));
	// add vbox to the main window
	gtk_container_add(GTK_CONTAINER(window), vbox);
	//show all window
	gtk_widget_show_all(window);
	gtk_main();

	//clear memory

	g_object_unref(pix_buf), pix_buf = NULL;
	boardList.clear();
	boardList_check.clear();
	boardList_off.clear();
	return 0;
}

