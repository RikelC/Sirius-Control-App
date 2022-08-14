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

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#define RESET   "\033[1;0m"
#define BLACK   "\033[1;30m"
#define RED     "\033[1;31m" 
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
using namespace std;
class FIRMWIRE{
	public:
		FIRMWIRE(int b, std::string v5, std::string v6){

			Board=b; firmware_v5 =v5; firmware_v6 =v6;
		}
		int Board;
		std::string firmware_v5;
		std::string firmware_v6;
};

//--------------------
// Global variables
// ------------------
static std::string command_init_func;
static std::string command_misc_func;

static std::string command_th_polarity;
static std::string command2;
static std::string commandTempUnit;
static std::string animationInterval = "1";
static std::string boardList_str;
std::vector<int> boardList;
std::map<int, bool> boardList_check;
std::map<int, bool> boardList_off;
std::vector<FIRMWIRE> numexo;
const gchar *user_entryData;
const gchar *user_entryData2;
static pid_t child_pid =-1;
//----baseline
GtkWidget *user_entry_baseline_board;
GtkWidget *user_entry_baseline_channel;
GtkWidget *user_entry_baseline_value;
const gchar *user_entryChannel1;
const gchar *user_entryBoard1;
const gchar *user_entryValue1;
//----threshold
GtkWidget *user_entry_threshold_board;
GtkWidget *user_entry_threshold_channel;
GtkWidget *user_entry_threshold_value;
const gchar *user_entryChannel2;
const gchar *user_entryBoard2;
const gchar *user_entryValue2;

gchar* convert_to_upper(const gchar *v){
	static gchar str[1000];
	gchar ch;
	for(int i =0; i <sizeof(v);i++){
		ch = v[i];
		if(islower(ch)){
			str[i] = toupper(ch);
		}
		else
			str[i] = ch;

	}

	return str;
}

gchar* convert_to_upper(std::string v){
	static gchar str[1000];
	gchar ch;
	for(int i =0; i <v.length();i++){
		ch = v[i];
		if(islower(ch)){
			str[i] = toupper(ch);
		}
		else
			str[i] = ch;

	}

	return str;
}


void signalHandler(int signal_number)
{
	if(signal_number == SIGTERM){
		cout<<"SIGTERM signal = "<<signal_number<<"received!\n";
	}
	if(signal_number == SIGINT){
		cout<<"SIGINT signal = "<<signal_number<<"received!\n";
	}
	kill(child_pid, signal_number);


}

//---------------------------------------
// Forward declarations of the functions
//----------------------------------------
void get_nActiveBoards();
void check_Boards_ONOFF_status();
void show_firmware_version(){
	if(boardList.empty()) cout<<"First Check ON/OFF Status of the Boards"<<endl;
	else{
		numexo.clear();

		std::string value5; std::string value6; 
		std::string line_str;
		std::string str1("send command to");
		std::string str2("/V5/FIRMWARE_VERSION_V5");
		std::string str3("/V6/FIRMWARE_VERSION_V6");

		for(auto board : boardList)
		{
			char *line = NULL;
			ssize_t nread; 
			size_t len = 0;

			char name[300]; 
			sprintf(name, "csh /home/sirius/numexo2_firmware_version.csh  %d", board);

			FILE *pipe = popen(name, "r");
			if (pipe==NULL){ perror ("Error opening file"); exit(EXIT_FAILURE);}
			else
			{
				while ((nread = getline(&line, &len, pipe)) != -1) {

					line_str = line;

					size_t found_v5 = line_str.find(str2);
					size_t found_v6 = line_str.find(str3);


					if(found_v5 != string::npos){
						//read v5 version
						size_t pos = line_str.find('=');
						size_t pos2 = line_str.find(')');
						size_t n = pos2-pos;
						value5 = line_str.substr(pos+1, n);

					}
					else if(found_v6 != string::npos){
						//read v6 version
						size_t pos = line_str.find('=');
						size_t pos2 = line_str.find(')');
						size_t n = pos2-pos;
						value6 = line_str.substr(pos+1, n);

					}

				}


				FIRMWIRE a(board, value5, value6);
				numexo.push_back(a);

				free(line);
				fclose (pipe);

			}



		}


	}
	cout<<GREEN<<"Board" <<"   "<<RED<< "V5 Firmware" <<"                  "<<BLUE<<"V6 Firmware"<<RESET<<endl;
	for(auto &it : numexo){

		cout<<GREEN<<it.Board <<"     "<<RED<< it.firmware_v5 <<"     "<<BLUE<<it.firmware_v6<<RESET<<endl;
	}

}

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

void open_Board_configFile(){

	child_pid = fork();
	if (child_pid == -1) {//creation of a child process was unsuccessful.
		perror("fork");
		exit(EXIT_FAILURE);
	} 
	else if (child_pid > 0) {//Returned to parent or caller. The value contains process ID of newly created child process.
		signal(SIGTERM, signalHandler);
		signal(SIGINT, signalHandler);
		system("gedit /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/ConfigFiles/numexo2Boards.config &");
	} else {//Returned to the newly created child process.
		exit(EXIT_FAILURE);
	}



}
//-----------------
// GRU
//----------------
void open_gru_config(){


	child_pid = fork();
	if (child_pid == -1) {//creation of a child process was unsuccessful.
		perror("fork");
		exit(EXIT_FAILURE);
	} 
	else if (child_pid > 0) {//Returned to parent or caller. The value contains process ID of newly created child process.
		signal(SIGTERM, signalHandler);
		signal(SIGINT, signalHandler);
		system("gedit /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/ConfigFiles/Run.config &");
	} else {//Returned to the newly created child process.
		exit(EXIT_FAILURE);
	}

	//	system("csh /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/launch_gruConfig.csh");
}

void perform_gru_launch(){

	system("source /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/launch_gruScript.csh &");

}
void kill_gru(){
	system("bash /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/kill_gruScript.sh");
}

void perform_vigru_launch(){

	system("source /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/launch_vigru.csh");

}

//-----------------
// GUser
//----------------
void open_guser_workspace(){

	child_pid = fork();
	if (child_pid == -1) {//creation of a child process was unsuccessful.
		perror("fork");
		exit(EXIT_FAILURE);
	} 
	else if (child_pid > 0) {//Returned to parent or caller. The value contains process ID of newly created child process.
		signal(SIGTERM, signalHandler);
		signal(SIGINT, signalHandler);
		system("gedit /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-sources/GUser.C  /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-sources/GUser.h &");
	} else {//Returned to the newly created child process.
		exit(EXIT_FAILURE);
	}

	//	system("csh /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/openGuserWorkspace.csh");
}
void do_guser_cmake(){
	system("csh /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/doCmake.csh 2");
}

void do_guser_cleanmake(){
	system("csh /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/doCleanMake.csh 2");
}

void do_guser_make(){
	system("csh /home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/doMake.csh 2");
}

void check_network_readiness(){
	system("bash /home/sirius/bin/numexo2_state.sh");
}

void miscellaneous_function(){
	child_pid = fork();
	if (child_pid == -1) {//creation of a child process was unsuccessful.
		perror("fork");
		exit(EXIT_FAILURE);
	} 
	else if (child_pid > 0) {//Returned to parent or caller. The value contains process ID of newly created child process.
		signal(SIGTERM, signalHandler);
		signal(SIGINT, signalHandler);
		//if(command_misc_func.compare("Check ON/OFF Status of the Boards") == 0)
		//check_Boards_ONOFF_status();
		//else if(command_misc_func.compare("Reload Active Boards") ==0)
		//	get_nActiveBoards();
		//else
		if(command_misc_func.compare("Ping Network") == 0)
			system("bash /home/sirius/bin/lping.sh");
		else if(command_misc_func.compare("Scan Network") == 0)
			system("bash /home/sirius/bin/lscan.sh");
		//else if(command_misc_func.compare("Check Readiness") == 0)
		//system("bash /home/sirius/bin/numexo2_state.sh");
		else if(command_misc_func.compare("Show Driver State") == 0)
			system("bash /home/sirius/bin/numexo2_dev_sirius.sh");
		else if(command_misc_func.compare("Show Embedded Software Version") == 0)
			system("bash /home/sirius/bin/numexo2_version.sh");
		else if(command_misc_func.compare("Show Firmware Version") == 0)
			show_firmware_version();
		else if(command_misc_func.compare("Show Narval State") == 0)
			system("bash /home/sirius/bin/narval_state.sh");
		else
			std::cout<<"Select from the drop-down list....\n";
	} else {//Returned to the newly created child process.
		exit(EXIT_FAILURE);
	}


}


void execute_AutoInit_function(){
	child_pid = fork();
	if (child_pid == -1) {//creation of a child process was unsuccessful.
		perror("fork");
		exit(EXIT_FAILURE);
	} 
	else if (child_pid > 0) {//Returned to parent or caller. The value contains process ID of newly created child process.
		signal(SIGTERM, signalHandler);
		signal(SIGINT, signalHandler);

		system("csh /home/sirius/config_v6_sirius.csh");
		system("/home/sirius/SetupGTS");
	} else {//Returned to the newly created child process.
		exit(EXIT_FAILURE);
	}



}



void execute_config_boards(){
	child_pid = fork();
	if (child_pid == -1) {//creation of a child process was unsuccessful.
		perror("fork");
		exit(EXIT_FAILURE);
	} 
	else if (child_pid > 0) {//Returned to parent or caller. The value contains process ID of newly created child process.
		signal(SIGTERM, signalHandler);
		signal(SIGINT, signalHandler);

		system("csh /home/sirius/config_v6_sirius.csh");
	} else {//Returned to the newly created child process.
		exit(EXIT_FAILURE);
	}


}

void execute_setupGTS(){
	child_pid = fork();
	if (child_pid == -1) {//creation of a child process was unsuccessful.
		perror("fork");
		exit(EXIT_FAILURE);
	} 
	else if (child_pid > 0) {//Returned to parent or caller. The value contains process ID of newly created child process.
		signal(SIGTERM, signalHandler);
		signal(SIGINT, signalHandler);

		system("/home/sirius/SetupGTS");
	} else {//Returned to the newly created child process.
		exit(EXIT_FAILURE);
	}



}

void execute_baseline_read(){
	user_entryBoard1= gtk_entry_get_text(GTK_ENTRY(user_entry_baseline_board));
	user_entryChannel1= gtk_entry_get_text(GTK_ENTRY(user_entry_baseline_channel));
	std::string board = convert_to_upper(user_entryBoard1);
	std::string channel = convert_to_upper(user_entryChannel1);
	cout<<"Board: "<<board<<"  Channel: "<<channel<<endl;
	char cmd[500];
	sprintf(cmd, "bash /home/sirius/numexo_baseline.sh READ %s %s", board.data(), channel.data());
	system(cmd);

}


void execute_baseline_write(){

	user_entryBoard1= gtk_entry_get_text(GTK_ENTRY(user_entry_baseline_board));
	user_entryChannel1= gtk_entry_get_text(GTK_ENTRY(user_entry_baseline_channel));
	user_entryValue1= gtk_entry_get_text(GTK_ENTRY(user_entry_baseline_value));

	std::string board = convert_to_upper(user_entryBoard1);
	std::string channel = convert_to_upper(user_entryChannel1);
	std::string value = convert_to_upper(user_entryValue1);
	cout<<"Board: "<<board<<"  Channel: "<<channel<<" Value: "<<value <<endl;
	char cmd[500];
	sprintf(cmd, "bash /home/sirius/numexo_baseline.sh WRITE %s %s %s", board.data(), channel.data(), value.data());
	system(cmd);

}

void execute_threshold_read(){
	user_entryBoard2 = gtk_entry_get_text(GTK_ENTRY(user_entry_threshold_board));
	user_entryChannel2 = gtk_entry_get_text(GTK_ENTRY(user_entry_threshold_channel));
	std::string board = convert_to_upper(user_entryBoard2);
	std::string channel = convert_to_upper(user_entryChannel2);
	std::string polarity = convert_to_upper(command_th_polarity);
	cout<<"Board: "<<board<<"  Channel: "<<channel<<"  Polarity: "<<polarity<<endl;
	char cmd[500];
	sprintf(cmd, "bash /home/sirius/numexo_threshold.sh READ %s %s %s", board.data(), channel.data(), polarity.data());
	system(cmd);
}


void execute_threshold_write(){
	user_entryBoard2 = gtk_entry_get_text(GTK_ENTRY(user_entry_threshold_board));
	user_entryChannel2 = gtk_entry_get_text(GTK_ENTRY(user_entry_threshold_channel));
	user_entryValue2= gtk_entry_get_text(GTK_ENTRY(user_entry_threshold_value));
	std::string board = convert_to_upper(user_entryBoard2);
	std::string channel = convert_to_upper(user_entryChannel2);
	std::string value = convert_to_upper(user_entryValue2);
	std::string polarity = convert_to_upper(command_th_polarity);
	cout<<"Board: "<<board<<"  Channel: "<<channel<<"  Polarity: "<<polarity<<" Value: "<<value <<endl;
	char cmd[500];
	sprintf(cmd, "bash /home/sirius/numexo_threshold.sh WRITE %s %s %s %s", board.data(), channel.data(), polarity.data(), value.data());
	system(cmd);

}


void execute_numexo2_reset(GtkWidget *wid,gpointer data){

	user_entryData = gtk_entry_get_text(GTK_ENTRY(data));
	std::string str(user_entryData);
	if((str.compare("ALL") == 0) || (str.compare("*") == 0)){
		char cmd[300];
		if(command2.compare("ON") == 0)
		{

			sprintf(cmd, "csh /home/sirius/reset_v6_config_withAcq.csh %s", boardList_str.c_str());cout<< "command "<<cmd<<endl;
			system(cmd);
		}    

		else if(command2.compare("OFF") == 0){
			sprintf(cmd, "csh /home/sirius/reset_v6_config_noAcq.csh %s", boardList_str.c_str());cout<< "command "<<cmd<<endl;
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

				sprintf(cmd, "csh /home/sirius/reset_v6_config_withAcq.csh %s", str.c_str());
				cout<< "command "<<cmd<<endl;
				system(cmd);
			}    

			else if(command2.compare("OFF") == 0){
				sprintf(cmd, "csh /home/sirius/reset_v6_config_noAcq.csh %s", str.c_str());cout<< "command "<<cmd<<endl;
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
	system("csh /home/sirius/launch_tmux_TelnetViewer.csh");
}


void execute_Tmux_close(){
	system("bash /home/sirius/quit_tmux_TelnetViewer.sh");
}


void execute_Temp_Draw(){
	if((commandTempUnit.compare("sec") == 0) || (commandTempUnit.compare("min") == 0) || (commandTempUnit.compare("hr") == 0)){
		char command[400];
		sprintf(command, "csh /home/sirius/launch_temp_Viewer.csh %d %s", std::stoi(animationInterval), commandTempUnit.c_str());
		//sprintf(command, "python /home/sirius/numexo2_temperature.py %d %s &", std::stoi(animationInterval), commandTempUnit.c_str());
		system(command);
	}
	else
		std::cout<<"Select the unit from the drop-down list....\n";
}

void execute_Temp_Kill(){
	system("bash /home/sirius/kill_numexo2_temperature.sh");
}

void execute_numexo2_TempGet(GtkWidget *wid,gpointer data){
	user_entryData2 = gtk_entry_get_text(GTK_ENTRY(data));
	std::string str(user_entryData2);

	if((str.compare("ALL") == 0) || (str.compare("*") == 0)){
		system("/home/sirius/bin/numexo2_temperature.sh");
	}
	else{
		char command[400];
		char *ptr;
		int bid;
		bid = (int) strtol(str.c_str(), &ptr, 10);
		if(check_board_is_online(bid)){
			sprintf(command, "bash /home/sirius/bin/numexo2_temperature.sh %s", str.c_str());
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

void th_polarity_selected(GtkWidget *widget, gpointer window){
	gchar *text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
	gtk_label_set_text(GTK_LABEL(window), text);
	std::string str(text);
	command_th_polarity = str;
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
	get_nActiveBoards();
	boardList_off.clear();
	boardList_check.clear();
	std::vector<int> temp_check;
	std::vector<int> temp;
	int board;
	std::ifstream file; std::string side; char separator;
	file.open("/home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/ConfigFiles/numexo2Boards.config", std::ifstream::in);
	if(file.is_open()){
		while(!file.eof()){
			file >> side >> separator >> board;
			//cout<< side<< " " << separator <<" " <<board<<endl;
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
	FILE *pipe = popen("bash /home/sirius/bin/numexo2_list.sh", "r");
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
	  "/home/sirius/SiriusControlApp_Dir/expsiriuslogo.png", 250, 150, TRUE, &error);
	  if (pixbuf == NULL) {
	  g_printerr ("Error loading file: #%d %s\n", error->code, error->message);
	  g_error_free (error);
	  exit (1);
	  }
	  */	GtkWidget * dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Exp@SIRIUS");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "Author: Rikel CHAKMA");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "An application for controlling SIRIUS setup. \n\n This software has been developed using gtk2. It executes the shell scripts written by Sébastien COUDERT(GANIL) and me. \n\n This is a free Software, you can redistribute or modify it, under GNU General Public Liscence as published by the Free Software Foundation.  ");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "");
	//gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	//g_object_unref(pixbuf), pixbuf = NULL;
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}


void show_help(){


cout<<"=================================Necassary Chack===================================\n";
cout<<"--Boards ON/OFF Status: This check must be performed first. It shows whether all the boards listed in <<Board Config file>> are ON or OFF.\n";
cout<<"--Open Board config file: To edit the <<Board Config file>>. The format must be preserved. This file is read in several places in the program.\n";
cout<<"--Network Readiness: To check whether the boards are ready for initialization.\n";

cout<<"=================================Setup Initialization===================================\n";
cout<<"--Configure Numexo2 boards: To initialize basaline, threshold and other registers.\n";
cout<<"--Setup GTS: To align the GTS tree.\n";
cout<<"--Auto initialization: It performs the above two operations sequentially.\n";


cout<<"=================================Miscellaneous functions===================================\n";
cout<<"--Ping Network: To test the various connected IP addresses in the Network.\n";
cout<<"--Scan Network: To see who is who and their status.\n";
cout<<"--Show Driver State: To see the Narval actor state.\n";
cout<<"--Show Firmware versions: To check the versions of the firmwares installed in the boards.\n";
cout<<"--Show Embaded Software version: To see the embadded software installed in V5.\n";
cout<<"--Show Narval State: To check the status of Narval.\n";

cout<<"=================================Reset Numexo2===================================\n";
cout<<"--Reset Numexo2: To reset the Numexo2 boards. This is required to bring the baseline to 0 value. Very essential for proper functiong of the thresholds set for each boards.\n";
cout<<"--You can Enter Board numbers in formats like: 65,66,67 or ALL or Front or Back (Note the entry is case insensitive).\n";
cout<<"--Chose the state of the Acquisition as the commands are different for ON and OFF state.\n";


cout<<"=================================Baseline===================================\n";
cout<<"--Baseline: To read and write the baseline of the numexo2 boards.\n";
cout<<"--You can Enter Board numbers in formats like: 65,66,67 or 64-70 or ALL or DSSD/Front/Back/Tunnel (Note the entry is case insensitive).\n";
cout<<"--You can Enter Channel numbers in formats like: 1,10,16 or 1-4 or ALL (Note the entry is case insensitive).\n";
cout<<"--Chose whether to read or write. For writing you must give in Hexadecimal format 0x.....\n";


cout<<"=================================Threshold===================================\n";
cout<<"--Threshold: To read and write the threshold levels of the numexo2 boards.\n";
cout<<"--You can Enter Board numbers in formats like: 65,66,67 or 64-70 or ALL or DSSD/Front/Back/Tunnel (Note the entry is case insensitive).\n";
cout<<"--You can Enter Channel numbers in formats like: 1,10,16 or 1-4 or ALL (Note the entry is case insensitive).\n";
cout<<"--Chose the Polarity of the threshold.\n";
cout<<"--Read: To read the values for the chosen boards and channels\n";
cout<<"--Write: To write the value provided in Hexadecimal format 0x..... to the chosen boards and channels.\n";

cout<<"=================================Telnet Viewer===================================\n";
cout<<"--Open: To view the real time flow of data for different boards.\n";
cout<<"--Close: To close the telnet viewer session.\n";


cout<<"=================================Temperature of Numexo2===================================\n";
cout<<"--Measure: To see the temeratue of vertex5 and vertex6 of the numexo2 boards.\n";
cout<<"--You can enter ALL or by board numbers 65,70,....\n";

cout<<"=================================Temperature as a function of time===================================\n";
cout<<"--Draw: To view graphically the temeratue of vertex5 and vertex6 of the numexo2 boards.\n";
cout<<"--Close: To kill the graphical viewer.\n";

cout<<"=================================GRU===================================\n";
cout<<"--Open Configuration file: To edit/view the configurations set for Online/Offline analysis.\n";
cout<<"--Launch: To launch the GruScript.\n";
cout<<"--Kill: If you want to kill.\n";

cout<<"=================================VIGRU===================================\n";
cout<<"--Launch: To launch Vigru.\n";

cout<<"=================================GUser===================================\n";
cout<<"--Open Workspace: To edit/view the GUser header and source files.\n";
cout<<"--Cmake: To do Cmake of the source codes.\n";
cout<<"--Clean Make: To do clean Make.\n";
cout<<"--Make: To Make.\n";

}


int main(int argc, char *argv[]) {
	//-------------	
	//declration
	//-----------
	GtkWidget *window;//main window
	GtkWidget *vbox;// vertical box to hold all the horizontal boxes
	GtkWidget *halign;//alignment
	GtkWidget *balign;
	//-----------------------------
	// Necessary Check 
	// ----------------------------

	GtkWidget *hboxCheck;//horizontal box for Check section
	GtkWidget *labelCheck;//label for this section

	GtkWidget *button_OnOff;//  button to check On/Off status of the boards
	GtkWidget *button_OpenBConfig;//  button to open boards config file
	GtkWidget *button_checkR;// button to check Readiness of the network
	//------------------
	//Introduction section
	//--------------------
	GtkWidget *hboxIntro;// horizontal box for the Intro section
	GtkWidget *hboxIntro2;// horizontal box for the Intro section
	GtkWidget *button_about;
	GtkWidget *button_help;
	GtkWidget *labelIntro;
	GtkWidget* image_ganillogo; 
	//-----------------------------
	// Setup initialization section
	// ----------------------------
	GtkWidget *hboxInit;//horizontal box for init section
	GtkWidget *labelInit;//label for this section
	GtkWidget *button_configBoard;
	GtkWidget *button_gets;
	GtkWidget *button_configAuto;
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
	GtkWidget *labelReset2;//Numexo board number
	GtkWidget *label_acq;//label for Acquisition state
	GtkWidget *label_acq_combo_selected;// label for acq state selected from the list
	GtkWidget *button_reset;// ok button to execute
	//---------------------------------
	// Baseline
	//----------------------------------
	GtkWidget *hboxBaseline;//horizontal box for this section
	GtkWidget *labelBaseline;//
	GtkWidget *labelBoard;//
	GtkWidget *labelChannel;//

	GtkWidget *button_baseline_read;
	GtkWidget *button_baseline_write;
	//---------------------------------
	// Threshold
	//----------------------------------
	GtkWidget *hboxThreshold;//horizontal box for this section
	GtkWidget *labelThresshold;//
	GtkWidget *labelBoard_th;//
	GtkWidget *labelChannel_th;//
	GtkWidget *labelPolarity_th;//

	GtkWidget *combo_th;//th polarity
	GtkWidget *button_threshold_read;
	GtkWidget *button_threshold_write;

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
	GtkWidget *labelTemp_board2;
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
	// GRU
	//----------------
	GtkWidget *hbox_gru;
	GtkWidget *label_gru;
	GtkWidget *button_gru_config;
	GtkWidget *button_gru_launch;
	GtkWidget *button_gru_kill;
	//-----------------
	// VIGRU
	//----------------
	GtkWidget *hbox_vigru;
	GtkWidget *label_vigru;
	GtkWidget *button_vigru_launch;

	//-----------------
	// GUser
	//----------------
	GtkWidget *hbox_guser;
	GtkWidget *label_guser;
	GtkWidget *button_guser_workspace;
	GtkWidget *button_guser_cmake;
	GtkWidget *button_guser_cleanmake;
	GtkWidget *button_guser_make;

	//---------------
	// Status bar
	// --------------
	GtkWidget *statusbar;
	//-------------------------
	// gtk init
	// --------------------
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Exp@Sirius");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);
	gtk_window_set_default_size(GTK_WINDOW(window), 100, 200); 
	// to be able to destroy the window by clicking x
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(gtk_main_quit), NULL);

	//------------------
	//init verical boxes
	//------------
	vbox = gtk_vbox_new(FALSE, 30);
	//get the list of active boards
	//get_nActiveBoards();
	//check_Boards_ONOFF_status();
	//---------------------------
	// Introduction section
	//--------------------------
	hboxIntro = gtk_hbox_new(FALSE, 50);
	hboxIntro2 = gtk_hbox_new(FALSE, 10);
	//ganil logo	
	GError *error = NULL;
	GdkPixbuf *pix_buf = gdk_pixbuf_new_from_file_at_scale("/home/sirius/ganacq_manip/test/GRU_SIRIUS/GUI_SiriusControlApp/ganillogo.png", 150,50, TRUE, &error);
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
	gtk_box_pack_start(GTK_BOX(hboxIntro2), button_about, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT(button_about), "button-press-event",
			G_CALLBACK(display_about), (gpointer)window);
	//help button
	button_help = gtk_button_new_with_label ("Help");
	gtk_widget_set_size_request(button_help, 70, 30 );
	gtk_box_pack_start(GTK_BOX(hboxIntro2), button_help, TRUE, TRUE, 0);
	g_signal_connect (G_OBJECT(button_help), "button-press-event",
			G_CALLBACK(show_help), NULL);
	

	gtk_box_pack_start(GTK_BOX(hboxIntro), hboxIntro2, TRUE, TRUE, 0);

	//gtk_box_pack_start(GTK_BOX(vbox), hboxIntro,TRUE, TRUE, 0);
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxIntro);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//-----------------------------
	// Necessary Check 
	// ----------------------------

	hboxCheck = gtk_hbox_new(FALSE, 10);
	labelCheck = gtk_label_new("Necessary Check");
	gtk_box_pack_start(GTK_BOX(hboxCheck), labelCheck, TRUE, TRUE, 0);
	button_OnOff = gtk_button_new_with_label ("Boards ON/OFF Status");
	gtk_widget_set_size_request(button_OnOff, 70, 30 );
	g_signal_connect (G_OBJECT(button_OnOff), "clicked",
			G_CALLBACK(check_Boards_ONOFF_status), NULL);
	gtk_box_pack_start(GTK_BOX(hboxCheck), button_OnOff, TRUE, TRUE, 0);

	button_OpenBConfig = gtk_button_new_with_label ("Open Board config file");
	gtk_widget_set_size_request(button_OpenBConfig, 70, 30 );
	g_signal_connect (G_OBJECT(button_OpenBConfig), "clicked",
			G_CALLBACK(open_Board_configFile), NULL);
	gtk_box_pack_start(GTK_BOX(hboxCheck), button_OpenBConfig, TRUE, TRUE, 0);

	button_checkR = gtk_button_new_with_label ("Network Readiness");
	gtk_widget_set_size_request(button_checkR, 70, 30 );
	g_signal_connect (G_OBJECT(button_checkR), "clicked",
			G_CALLBACK(check_network_readiness), NULL);
	gtk_box_pack_start(GTK_BOX(hboxCheck), button_checkR, TRUE, TRUE, 0);
	//position
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxCheck);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//-----------------------------------
	// Numexo2 Initialization section
	// ---------------------------------
	hboxInit = gtk_hbox_new(FALSE, 10);
	//functionality label	
	labelInit = gtk_label_new("Setup Initialization");
	gtk_box_pack_start(GTK_BOX(hboxInit), labelInit, TRUE, TRUE, 0);
	button_configBoard = gtk_button_new_with_label ("Configure Numexo2 boards");
	gtk_widget_set_size_request(button_configBoard, 70, 30 );
	g_signal_connect (G_OBJECT(button_configBoard), "clicked",
			G_CALLBACK(execute_config_boards), NULL);
	gtk_box_pack_start(GTK_BOX(hboxInit), button_configBoard, TRUE, TRUE, 0);


	button_gets = gtk_button_new_with_label ("Setup GTS");
	gtk_widget_set_size_request(button_gets, 70, 30 );
	g_signal_connect (G_OBJECT(button_gets), "clicked",
			G_CALLBACK(execute_setupGTS), NULL);
	gtk_box_pack_start(GTK_BOX(hboxInit), button_gets, TRUE, TRUE, 0);

	button_configAuto = gtk_button_new_with_label ("Auto Initialization");
	gtk_widget_set_size_request(button_configAuto, 70, 30 );
	g_signal_connect (G_OBJECT(button_configAuto), "clicked",
			G_CALLBACK(execute_AutoInit_function), NULL);
	gtk_box_pack_start(GTK_BOX(hboxInit), button_configAuto, TRUE, TRUE, 0);
	//position
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxInit);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//-----------------------------------
	//Misc. function section
	//--------------------------------------	
	hboxMiscFunc = gtk_hbox_new(FALSE, 10);
	labelMiscFunc = gtk_label_new("Miscellaneous function");
	gtk_box_pack_start(GTK_BOX(hboxMiscFunc), labelMiscFunc, TRUE, TRUE, 0);
	// List of functions
	labelMiscFunc_combo_selected = gtk_label_new("");
	combo_misc_func = gtk_combo_box_new_text();
	//gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Check ON/OFF Status of the Boards");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Reload Active Boards");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Ping Network");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Scan Network");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Check Readiness");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Show Driver State");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Reboot Numexos");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Show Firmware Version");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_misc_func), "Show Embedded Software Version");
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
	hboxReset = gtk_hbox_new(FALSE, 10);
	// label Numexo
	labelReset = gtk_label_new("Reset");
	gtk_box_pack_start(GTK_BOX(hboxReset), labelReset, TRUE, TRUE, 0);
	labelReset2 = gtk_label_new("Board");
	gtk_box_pack_start(GTK_BOX(hboxReset), labelReset2, TRUE, TRUE, 0);
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

	//---------------------------------
	// Baseline
	//----------------------------------
	hboxBaseline= gtk_hbox_new(FALSE, 10);
	labelBaseline= gtk_label_new("Baseline");
	//gtk_widget_set_size_request(labelBaseline, 50, 30 );
	gtk_box_pack_start(GTK_BOX(hboxBaseline), labelBaseline, TRUE, TRUE, 0);

	labelBoard= gtk_label_new("Board");
	//gtk_widget_set_size_request(labelBoard, 30, 30 );
	gtk_box_pack_start(GTK_BOX(hboxBaseline), labelBoard, TRUE, TRUE, 0);

	user_entry_baseline_board= gtk_entry_new();
	//user_entryBoard1 = gtk_entry_get_text(GTK_ENTRY(user_entry_baseline_board));
	gtk_box_pack_start(GTK_BOX(hboxBaseline), user_entry_baseline_board, TRUE, TRUE, 0);

	//------------------
	labelChannel= gtk_label_new("Channel");
	//gtk_widget_set_size_request(labelChannel, 30, 30 );
	gtk_box_pack_start(GTK_BOX(hboxBaseline), labelChannel, TRUE, TRUE, 0);
	user_entry_baseline_channel= gtk_entry_new();
	//user_entryChannel1 = gtk_entry_get_text(GTK_ENTRY(user_entry_baseline_channel));
	gtk_box_pack_start(GTK_BOX(hboxBaseline), user_entry_baseline_channel, TRUE, TRUE, 0);

	button_baseline_read= gtk_button_new_with_label("Read");
	gtk_widget_set_size_request(button_baseline_read, 70, 30 );
	g_signal_connect (G_OBJECT(button_baseline_read), "clicked",
			G_CALLBACK(execute_baseline_read), (gpointer) NULL);
	gtk_box_pack_start(GTK_BOX(hboxBaseline), button_baseline_read, TRUE, TRUE, 0);


	button_baseline_write= gtk_button_new_with_label("Write");
	gtk_widget_set_size_request(button_baseline_write, 70, 30 );
	g_signal_connect (G_OBJECT(button_baseline_write), "clicked",
			G_CALLBACK(execute_baseline_write), NULL);
	gtk_box_pack_start(GTK_BOX(hboxBaseline), button_baseline_write, TRUE, TRUE, 0);
	//
	user_entry_baseline_value= gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hboxBaseline), user_entry_baseline_value, TRUE, TRUE, 0);

	//position 
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxBaseline);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//---------------------------------
	// Threshold
	//----------------------------------
	hboxThreshold = gtk_hbox_new(FALSE, 10);
	labelThresshold= gtk_label_new("Threshold");
	gtk_box_pack_start(GTK_BOX(hboxThreshold), labelThresshold, TRUE, TRUE, 0);
	labelBoard_th= gtk_label_new("Board");

	gtk_box_pack_start(GTK_BOX(hboxThreshold), labelBoard_th, TRUE, TRUE, 0);
	//
	user_entry_threshold_board= gtk_entry_new();

	gtk_box_pack_start(GTK_BOX(hboxThreshold), user_entry_threshold_board, TRUE, TRUE, 0);
	//
	labelChannel_th= gtk_label_new("Channel");
	gtk_box_pack_start(GTK_BOX(hboxThreshold), labelChannel_th, TRUE, TRUE, 0);
	//
	user_entry_threshold_channel= gtk_entry_new();

	gtk_box_pack_start(GTK_BOX(hboxThreshold), user_entry_threshold_channel, TRUE, TRUE, 0);


	labelPolarity_th = gtk_label_new("Polarity");
	gtk_box_pack_start(GTK_BOX(hboxThreshold), labelPolarity_th, TRUE, TRUE, 0);
	// Choose
	combo_th = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_th), "Positive");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_th), "Negative");
	gtk_box_pack_start(GTK_BOX(hboxThreshold), combo_th, TRUE, TRUE, 0);
	// select from the list
	g_signal_connect(G_OBJECT(combo_th), "changed", G_CALLBACK(th_polarity_selected), (gpointer) labelPolarity_th);




	button_threshold_read= gtk_button_new_with_label("Read");
	gtk_widget_set_size_request(button_threshold_read, 70, 30 );
	g_signal_connect (G_OBJECT(button_threshold_read), "clicked",
			G_CALLBACK(execute_threshold_read), NULL);
	gtk_box_pack_start(GTK_BOX(hboxThreshold), button_threshold_read, TRUE, TRUE, 0);

	button_threshold_write= gtk_button_new_with_label("Write");
	gtk_widget_set_size_request(button_threshold_write, 70, 30 );
	g_signal_connect (G_OBJECT(button_threshold_write), "clicked",
			G_CALLBACK(execute_threshold_write), NULL);
	gtk_box_pack_start(GTK_BOX(hboxThreshold), button_threshold_write, TRUE, TRUE, 0);
	//
	user_entry_threshold_value= gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hboxThreshold), user_entry_threshold_value, TRUE, TRUE, 0);
	//
	//position the button
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hboxThreshold);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);

	//------------------------
	// Telnet viewer
	//-----------------------
	hboxTmux = gtk_hbox_new(FALSE, 10);
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
	hboxTemp_cl = gtk_hbox_new(FALSE, 10);
	// label Numexo
	labelTemp_board = gtk_label_new("Temperature");
	gtk_box_pack_start(GTK_BOX(hboxTemp_cl), labelTemp_board, TRUE, TRUE, 0);
	labelTemp_board2 = gtk_label_new("Board");
	gtk_box_pack_start(GTK_BOX(hboxTemp_cl), labelTemp_board2, TRUE, TRUE, 0);
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
	hboxTemp_gr = gtk_hbox_new(FALSE, 10);
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
	//-----------------
	// GRU
	//----------------
	hbox_gru= gtk_hbox_new(FALSE, 10);
	label_gru = gtk_label_new("GRU"); 
	gtk_box_pack_start(GTK_BOX(hbox_gru), label_gru, TRUE, TRUE, 0);


	button_gru_config =gtk_button_new_with_label("Open Configuration file");
	g_signal_connect(G_OBJECT(button_gru_config), "clicked", 
			G_CALLBACK(open_gru_config), NULL);
	gtk_box_pack_start (GTK_BOX (hbox_gru), button_gru_config, TRUE, TRUE, 0);

	button_gru_launch = gtk_button_new_with_label("Launch");
	g_signal_connect(G_OBJECT(button_gru_launch), "clicked", 
			G_CALLBACK(perform_gru_launch), NULL);
	gtk_box_pack_start (GTK_BOX (hbox_gru), button_gru_launch, TRUE, TRUE, 0);


	button_gru_kill= gtk_button_new_with_label("Kill");
	g_signal_connect(G_OBJECT(button_gru_kill), "clicked", 
			G_CALLBACK(kill_gru), NULL);
	gtk_box_pack_start (GTK_BOX (hbox_gru), button_gru_kill, TRUE, TRUE, 0);

	//position the box
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hbox_gru);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);

	//-----------------
	// VIGRU
	//----------------
	hbox_vigru= gtk_hbox_new(FALSE, 10);
	label_vigru = gtk_label_new("VIGRU"); 
	gtk_box_pack_start(GTK_BOX(hbox_vigru), label_vigru, TRUE, TRUE, 0);

	button_vigru_launch = gtk_button_new_with_label("Launch");
	g_signal_connect(G_OBJECT(button_vigru_launch), "clicked", 
			G_CALLBACK(perform_vigru_launch), NULL);
	gtk_box_pack_start (GTK_BOX (hbox_vigru), button_vigru_launch, TRUE, TRUE, 0);



	//position the box
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hbox_vigru);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);

	//-----------------
	// GUser
	//----------------
	hbox_guser= gtk_hbox_new(FALSE, 10);
	label_guser = gtk_label_new("GUser");
	gtk_box_pack_start(GTK_BOX(hbox_guser), label_guser, TRUE, TRUE, 0);

	button_guser_workspace = gtk_button_new_with_label("Open Workspace");
	g_signal_connect(G_OBJECT(button_guser_workspace), "clicked", 
			G_CALLBACK(open_guser_workspace), NULL);
	gtk_box_pack_start (GTK_BOX (hbox_guser), button_guser_workspace, TRUE, TRUE, 0);

	button_guser_cmake= gtk_button_new_with_label("Cmake");
	g_signal_connect(G_OBJECT(button_guser_cmake), "clicked", 
			G_CALLBACK(do_guser_cmake), NULL);
	gtk_box_pack_start (GTK_BOX (hbox_guser), button_guser_cmake, TRUE, TRUE, 0);

	button_guser_cleanmake = gtk_button_new_with_label("Clean Make");
	g_signal_connect(G_OBJECT(button_guser_cleanmake), "clicked", 
			G_CALLBACK(do_guser_cleanmake), NULL);
	gtk_box_pack_start (GTK_BOX (hbox_guser), button_guser_cleanmake, TRUE, TRUE, 0);

	button_guser_make = gtk_button_new_with_label("Make");
	g_signal_connect(G_OBJECT(button_guser_make), "clicked", 
			G_CALLBACK(do_guser_make), NULL);
	gtk_box_pack_start (GTK_BOX (hbox_guser), button_guser_make, TRUE, TRUE, 0);


	//position the box
	halign = gtk_alignment_new(1, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(halign), hbox_guser);
	gtk_box_pack_start(GTK_BOX(vbox), halign, TRUE, TRUE, 0);
	//----------------------------------
	//status bar
	//----------------------
	halign = gtk_alignment_new(1, 1, 1, 1);
	statusbar = gtk_statusbar_new();
	gtk_container_add(GTK_CONTAINER(halign), statusbar);
	gtk_box_pack_start(GTK_BOX(vbox), halign, FALSE, FALSE, 0);

	//g_signal_connect(G_OBJECT(button_okInit), "clicked", 
	//G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_ok_misc_func), "clicked", 
			G_CALLBACK(button_pressed), G_OBJECT(statusbar));

	g_signal_connect(G_OBJECT(button_reset), "clicked", 
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

