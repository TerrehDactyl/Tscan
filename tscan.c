#include <netinet/tcp.h>
#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#define MAX_INT 32767
void testconnection(char *input1, int port);
void portscan(void);
void createsinglesizegrid(GtkWidget *grid, gchar *labels[], void *callback[], int rows, int columns, const gchar *style[], int stylenumber);
void add_context(const gchar *style, GtkWidget *widget);
void set_spacing(GtkWidget *widget, int colspace, int rowspace);
void button_connect_callback(GtkWidget *button, void *button_callback);
void createdisplays(GtkWidget *widget, GtkWidget *buffer, const gchar *style[], gchar *startingtext[]);
void testcallback(void);
static void startportcallback( GtkWidget *widget, GtkWidget *entry);
static void endportcallback( GtkWidget *widget, GtkWidget *entry);
static void ipaddresscallback( GtkWidget *widget, GtkWidget *entry);
void startscan();
void cancelscan();
void savetofile();

// compile with gcc -Wall -g portscan.c -o portscan `pkg-config --cflags --libs gtk+-3.0`
struct Global_Widgets
{
GtkWidget *outputdisplay; 
GtkTextBuffer *outputbuffer; //widget for the text
GtkWidget *startportentry;
GtkWidget *endportentry;
GtkWidget *ipaddressentry;
int savecount;
}gwidget;

struct input_variables
{
const char *startport;
const char *endport;
const char *ipaddress;
}input;

int main(int argc, char *argv[])
{
GtkWidget *window; //widget for the window
GtkWidget *vbox; //widget for the vertical box
GtkWidget *hbox;
GtkWidget *buttonbox;
GtkWidget *buttonhbox;
GtkWidget *label;
GtkWidget *grid;
GtkWidget *entrygrid;
gchar *buttonlabels[] ={"Start Scan", "Cancel Scan", "Save To File"};
void *buttoncallbacks [] = {startscan, cancelscan, savetofile};
const gchar *style [] = {"window_style", "display_style", "button_style"};
gchar *startingtext[] = {"Starting Port\n", "Ending Port\n", "IP Address"};

gtk_init(&argc, &argv); //starting gtk 

window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //creates toplevel window
gtk_window_set_title(GTK_WINDOW(window), "TScan"); //sets a window title 
gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER); //opens the window in the center of the screen
gtk_container_set_border_width(GTK_CONTAINER(window), 5); //sets the border size of the window
add_context(style[0], window);

grid = gtk_grid_new();

for (int i=0; i<=2; i++)
{
	for(int j = 0; j<1; j++)
	{	
		label = gtk_label_new(startingtext[i]);
		gtk_grid_attach(GTK_GRID(grid), label, j, i, 1, 1); //sets the defaults for creating each table button
	}
}

entrygrid = gtk_grid_new();
gwidget.startportentry = gtk_entry_new ();
gwidget.endportentry = gtk_entry_new();
gwidget.ipaddressentry = gtk_entry_new();

gtk_grid_attach(GTK_GRID(entrygrid), gwidget.startportentry, 0, 0, 1, 1); //sets the defaults for creating each table button
gtk_grid_attach(GTK_GRID(entrygrid), gwidget.endportentry, 0,1,1,1);
gtk_grid_attach(GTK_GRID(entrygrid), gwidget.ipaddressentry, 0,2,1,1);

 g_signal_connect (gwidget.startportentry, "activate", G_CALLBACK (startportcallback), gwidget.startportentry);
 g_signal_connect (gwidget.endportentry, "activate", G_CALLBACK (endportcallback), gwidget.endportentry);
 g_signal_connect (gwidget.ipaddressentry, "activate", G_CALLBACK (ipaddresscallback), gwidget.ipaddressentry);

gwidget.outputdisplay = gtk_text_view_new (); //sets the display widget as a text display 
gwidget.outputbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gwidget.outputdisplay)); //sets the gwidget.buffer widget to the text gwidget.buffer of display
gtk_text_buffer_set_text (gwidget.outputbuffer, "output", -1); //displays input.num1
add_context(style[1], gwidget.outputdisplay);

//g_signal_connect(window, "key-release-event", G_CALLBACK(keyboard_callback), NULL);

buttonbox = gtk_grid_new(); 
createsinglesizegrid(buttonbox, buttonlabels, buttoncallbacks, 1, 3, style, 2);
set_spacing(buttonbox, 3, 3);

vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1); //creates a vbox without autosizing 
hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1); //creates a vbox without autosizing 
buttonhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1); //creates a vbox without autosizing 

gtk_container_add(GTK_CONTAINER(window), vbox); //adds the vbox to the window 
gtk_box_pack_start(GTK_BOX(vbox),  hbox, FALSE, FALSE, 0); //packs the display into the vbox
gtk_box_pack_start(GTK_BOX(hbox),  grid, FALSE, FALSE, 0); 
gtk_box_pack_start(GTK_BOX(hbox),  entrygrid, FALSE, FALSE, 0); 
gtk_box_pack_start(GTK_BOX(vbox),  buttonhbox, FALSE, FALSE, 0); //packs the display into the vbox
gtk_box_pack_start(GTK_BOX(buttonhbox),  buttonbox, FALSE, FALSE, 0); //packs the display into the vbox
gtk_box_pack_start(GTK_BOX(vbox),  gwidget.outputdisplay, FALSE, FALSE, 0); //packs the display into the vbox

g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

gtk_widget_show_all(window); //shows all widgets 

gtk_main();//gtk main, this is the main loop of GTK
}

static void startportcallback( GtkWidget *widget, GtkWidget *entry)
{
  input.startport = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Starting Port: %s\n", input.startport);
}

static void endportcallback( GtkWidget *widget, GtkWidget *entry)
{
  input.endport = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Ending Port: %s\n", input.endport);
}

static void ipaddresscallback( GtkWidget *widget, GtkWidget *entry)
{
  input.ipaddress = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("IP Address: %s\n", input.ipaddress);
}

void startscan()
{
	struct sockaddr_in server_address;
	startportcallback(gwidget.startportentry, gwidget.startportentry);
	endportcallback(gwidget.endportentry, gwidget.endportentry);
	ipaddresscallback(gwidget.ipaddressentry, gwidget.ipaddressentry);
	int start = atoi(input.startport);
	int end = atoi(input.endport);
	FILE *filepointer;

	if (gwidget.savecount > 0)
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
    strncpy(hostname, input.ipaddress, 15);
	server_address.sin_addr.s_addr = inet_addr(input.ipaddress);
	int err = 0;
	err = connect(network_socket,(struct sockaddr *) &server_address,sizeof server_address);
	if (err <0)
	{
		gtk_text_buffer_set_text (gwidget.outputbuffer, "closed", -1); //displays input.num1
		printf("Port: %d is closed\n", i);

		if (gwidget.savecount > 0)
		{
			fprintf(filepointer, "Port: %d is closed\n", i);
		}
	}
	else
	{
		gtk_text_buffer_set_text (gwidget.outputbuffer, "open", -1); //displays input.num1
		printf("Port: %d is open\n", i);

		if (gwidget.savecount > 0)
		{
			fprintf(filepointer, "Port: %d is open\n", i);
		}
	}
	close(network_socket);
}
fclose(filepointer);
}

void cancelscan()
{
	exit(1);
}

void savetofile()
{
	gwidget.savecount = 1;
}

void set_spacing(GtkWidget *widget, int colspace, int rowspace)
{
  gtk_grid_set_column_spacing(GTK_GRID(widget), colspace);
  gtk_grid_set_row_spacing(GTK_GRID(widget), rowspace);
}

void createsinglesizegrid(GtkWidget *grid, gchar *labels[], void *callback[], int rows, int columns, const gchar *style[], int stylenumber)
{
 int pos = 0;
GtkWidget *button; //widget for the buttons 
for (int i=0; i < rows; i++) //for loop for the rows
{
for (int j=0; j < columns; j++) //for loop for the columns
{
button = gtk_button_new_with_label(labels[pos]); //sets each button label to the respective button 
button_connect_callback(button, callback[pos]); //attaches the button to the respective callback
gtk_grid_attach(GTK_GRID(grid), button, j, i, 1, 1); //sets the defaults for creating each table button
gtk_widget_set_size_request(button, 40, 40); //sets the size of the buttons
add_context(style[stylenumber], button);
pos++; //changes the position 
}
}
}

void add_context(const gchar *style, GtkWidget *widget)
{
  GtkCssProvider* Provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(Provider), "Styles.css", NULL);
  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  gtk_style_context_add_class(context, style);
  gtk_style_context_add_provider (context,GTK_STYLE_PROVIDER(Provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void button_connect_callback(GtkWidget *button, void *button_callback)
{
  g_signal_connect(button, "clicked", G_CALLBACK(button_callback), NULL);
}
