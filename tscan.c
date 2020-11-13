#include "gtktemplate.h"
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#define MAX_INT 32767
void portscan();
void startscan();
void cancelscan();
void savetofile();

// compile with gcc -Wall -g tscan.c -o tscan `pkg-config --cflags --libs gtk+-3.0`
struct Global_Widgets
{
GtkWidget *entries[3];
}gwidget;

struct input_variables
{
const char *startport;
const char *endport;
const char *ipaddress;
const char *entrytext[3];
size_t label_len;
int savecount;
}input;

int main(int argc, char *argv[])
{
GtkWidget *window; //widget for the window
GtkWidget *vbox; //widget for the vertical box
GtkWidget *hbox;
GtkWidget *buttonbox;
GtkWidget *label_grid;
GtkWidget *entrygrid;
gchar *buttonlabels[] ={"Start Scan", "Cancel Scan", "Save To File"};
void *buttoncallbacks [] = {startscan, cancelscan, savetofile};
gchar *labels[] = {"Starting Port\n", "Ending Port\n", "IP Address"};
input.label_len = arraysize(labels);

gtk_init(&argc, &argv); //starting gtk 

window = createwindow("Tscan", GTK_WIN_POS_CENTER);
label_grid = createlabels(labels, input.label_len);
entrygrid = gtk_grid_new();
create_entries(input.label_len, gwidget.entries, entrygrid);
buttonbox = createsinglesizegrid(buttonlabels, buttoncallbacks, NULL, 1, 3);

vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1); //creates a vbox without autosizing 
hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1); //creates a vbox without autosizing 

gtk_container_add(GTK_CONTAINER(window), vbox); //adds the vbox to the window 
gtk_box_pack_start(GTK_BOX(vbox),  hbox, FALSE, FALSE, 0); //packs the display into the vbox
gtk_box_pack_start(GTK_BOX(hbox),  label_grid, FALSE, FALSE, 0); 
gtk_box_pack_start(GTK_BOX(hbox),  entrygrid, FALSE, FALSE, 0); 
gtk_box_pack_start(GTK_BOX(vbox),  buttonbox, FALSE, FALSE, 0); //packs the display into the vbox2

show_and_destroy(window);
}

void startscan()
{
	struct sockaddr_in server_address;
	get_entry_text(gwidget.entries, input.entrytext, input.label_len);
	int start = atoi(input.entrytext[0]);
	int end = atoi(input.entrytext[1]);
	FILE *filepointer;

	if (input.savecount > 0)
	{
		filepointer = fopen("Results.txt", "w");
	}

for(int i = start; i<=end; i++)
{
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(i);
	int network_socket = socket(AF_INET, SOCK_STREAM, 0); 
	  if (network_socket < 0 ) 
	  { 
        printf("socket creation failed...\n"); 
    } 
    else
    {
        printf("Socket successfully created..\n"); 
    }
    char hostname[MAX_INT];
    strncpy(hostname, input.entrytext[2], 15);
	server_address.sin_addr.s_addr = inet_addr(input.entrytext[2]);
	int err = 0;
	err = connect(network_socket,(struct sockaddr *) &server_address,sizeof server_address);
	if (err <0)
	{
		g_print("Port: %d is closed\n", i);

		if (input.savecount > 0)
		{
			fprintf(filepointer, "Port: %d is closed\n", i);
		}
	}
	else
	{
		g_print("Port: %d is open\n", i);

		if (input.savecount > 0)
		{
			fprintf(filepointer, "Port: %d is open\n", i);
		}
	}
	close(network_socket);
}
g_print("program finished");

if (input.savecount > 0)
		{
			fclose(filepointer);
		}
}

void cancelscan()
{
	exit(1);
}

void savetofile()
{
	input.savecount = 1;
}
