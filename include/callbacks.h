/***************************************************************************
 *            callbacks.h
 *
 *  Mon Oct 24 11:37:26 2005
 *  Copyright  2005  Sergio Cuellar Valdes
 *  Email herrsergio@gmail.com
 *
 *  Modificado Tue Jun 5 2007
 *  Se añadieron mas botones
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#define BUFFERSIZE 1024
#define HOST "192.168.213.189"

#include <glade/glade.h>
#include <gnome.h>
#include <config.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "msocket.h"

#define M_PI	3.14159265358979323846  /* pi */
#define M_PI_2 1.57079632679489661923  /* pi/2 */
#define M_PI_4 0.78539816339744830962  /* pi/4 */

GladeXML *main_xml;
GladeXML *about_xml;
GladeXML *settings_xml;

GtkWidget *mi_pizarron;
gchar *texto;
GtkTextBuffer *buffer_text;
PangoFontDescription *font_desc;
GdkColor color;

msocketData in_socket;
const int inport = 4200;

G_LOCK_DEFINE_STATIC (received);
static volatile int received = 0;

char message[BUFFERSIZE];

gchar *host_virbot	="127.0.0.1";
gchar *host_head		="127.0.0.1";
gchar *host_control	="127.0.0.1";
gchar *host_arm		="127.0.0.1";
gchar *host_vision	="192.168.213.189";

int outport_virbot	= 9000;
int outport_head		= 6969;
int outport_control	= 2000;
int outport_vision	= 8000;
int outport_arm		= 2200;

int error;
char buffer[BUFFERSIZE];
char command[BUFFERSIZE];
//char host[BUFFERSIZE];

msocketData out_virbot_socket;
msocketData out_head_socket;
msocketData out_control_socket;
msocketData out_vision_socket;
msocketData out_arm_socket;


pid_t virbot_pid;
pid_t head_pid;
pid_t control_pid;
pid_t arm_pid;

#define XTERM_CMD		"/usr/bin/xterm"
#define VIRBOT_CMD 	"/home/robocup/atlanta/ViRbot/bin/virbot_linux"
//#define VIRBOT_CMD 	"/usr/bin/xeyes"
#define HEAD_CMD 		"/home/robocup/atlanta/cabesa/cabeza"
#define CONTROL_CMD 	"/home/robocup/atlanta/control/bin/RobotP"
#define ARM_CMD		"/home/robocup/atlanta/brazo/bin/Arm"

float step_mv = 1.0;

extern "C" {
	
void *listen_message(void *args) {
	for(;;) {
		if(in_socket.msocketRxData(message, BUFFERSIZE) > 0 ) {
			G_LOCK(received);
			received = 1;
			G_UNLOCK(received);
			//printf("> %s\n", message);
		}
	}
	return NULL;
}

void *draw_message(void *args) {
	for(;;) {
		if(received == 1) {
			G_LOCK(received);
			received = 0;
			G_UNLOCK(received);
			gdk_threads_enter();
			buffer_text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(mi_pizarron));
			gtk_text_buffer_set_text(buffer_text, message, -1);
			gdk_flush ();
			gdk_threads_leave();
		}
	}
		
	return NULL;
}

	
/* This creates a socket which listens to port 4000 */
void init_listener_socket(void) {
	int error;
	
	if( (error = in_socket.msocketInitSocket( "", inport, RX_SOCKET ) )) {
   	printf( "error opening server: %s", msocketGetError( error ) ); 
    	exit( 1 ); 
   }
}
	
/* Initialize an load the .glade file and connect signals */
void tx8_vcc_main(void) {
	GtkWidget *window   = NULL;
	gchar     *xml_file = NULL;
			
	if (!main_xml) {
		xml_file=g_build_filename("../data","tx8-vcc_glade.glade",NULL);
		main_xml=glade_xml_new(xml_file,NULL,NULL);
							    	
		glade_xml_signal_autoconnect(main_xml);
	}
												
	window = glade_xml_get_widget(main_xml,"main_window");
	mi_pizarron = glade_xml_get_widget(main_xml, "textview");
	/* Para que el texto tenga otra fuente a la de default */
	font_desc = pango_font_description_from_string ("Sans bold 20");
	/* El font que se usará en el text view */
	gtk_widget_modify_font (mi_pizarron, font_desc);
	pango_font_description_free (font_desc);
	/* Para el color de la fuente */
	gdk_color_parse ("blue", &color);
	gtk_widget_modify_text (mi_pizarron, GTK_STATE_NORMAL, &color);
	
	gtk_widget_show(GTK_WIDGET(window));																				
}

/* Initialize the virbot socket */
void init_virbot_sockets(int outport, char *host) {
	
	if( (error = out_virbot_socket.msocketInitSocket( host, outport, TX_SOCKET ) )) {
   	printf( "error opening server: %s", msocketGetError( error ) ); 
    	exit( 1 ); 
   }
}

/* Initialize the head socket */
void init_head_sockets(int outport, char *host) {
	
	if( (error = out_head_socket.msocketInitSocket( host, outport, TX_SOCKET ) )) {
   	printf( "error opening server: %s", msocketGetError( error ) ); 
    	exit( 1 ); 
   }
}

/* Initialize the control socket */
void init_control_sockets(int outport, char *host) {
	
	if( (error = out_control_socket.msocketInitSocket( host, outport, TX_SOCKET ) )) {
   	printf( "error opening server: %s", msocketGetError( error ) ); 
    	exit( 1 ); 
   }
}

/* Initialize the vision socket */
void init_vision_sockets(int outport, char *host) {
	
	if( (error = out_vision_socket.msocketInitSocket( host, outport, TX_SOCKET ) )) {
   	printf( "error opening server: %s", msocketGetError( error ) ); 
    	exit( 1 ); 
   }
}

/* Initialize the arm socket */
void init_arm_sockets(int outport, char *host) {
	
	if( (error = out_arm_socket.msocketInitSocket( host, outport, TX_SOCKET ) )) {
   	printf( "error opening server: %s", msocketGetError( error ) ); 
    	exit( 1 ); 
   }
}

/* Close the connection and kill the socket */
void kill_sockets(void) {
	out_virbot_socket.msocketKillSocket();
	out_head_socket.msocketKillSocket();
	out_control_socket.msocketKillSocket();
	out_vision_socket.msocketKillSocket();
	out_arm_socket.msocketKillSocket();
	in_socket.msocketKillSocket();
}




/* It close the window when Quit from the File menu is clicked */
void on_quit_activate(GtkMenuItem *item, gpointer data) {
		gtk_main_quit();
}

/* It displays the about dialog */
void on_about_activate(GtkMenuItem *item, gpointer data) {
	
	GtkWidget *about;
	gchar *xml_file = NULL;
	
	
	if (!about_xml) {
		xml_file=g_build_filename("../data","about.glade",NULL);
		about_xml = glade_xml_new(xml_file, NULL,NULL);
		glade_xml_signal_autoconnect(about_xml);
	}

	about = glade_xml_get_widget(about_xml,"about");
	
	gtk_widget_show(GTK_WIDGET(about));

}

/* To close the about dialog about */
void on_about_destroy(GtkDialog *dialog,gint response, gpointer data) {
	GtkWidget *about;
	
	about = glade_xml_get_widget(about_xml,"about");
	
	gtk_widget_destroy(GTK_WIDGET(about));
	//gtk_widget_hide(GTK_WIDGET(about));
	g_object_unref(G_OBJECT (about_xml));
	
	about_xml = NULL;
		
}

/*To display the settings window */
void on_preferences_activate(GtkMenuItem *item, gpointer data) {
	
	GtkWidget *settings_window;
	gchar *xml_file = NULL;
	
	
	if (!settings_xml) {
		xml_file = g_build_filename("../data","settings.glade",NULL);
		settings_xml = glade_xml_new(xml_file, NULL, NULL);
		glade_xml_signal_autoconnect(settings_xml);
	}

	settings_window = glade_xml_get_widget(settings_xml,"settings_window");
	gtk_widget_show(GTK_WIDGET(settings_window));
}

/* To close the dialog settings */
void on_settings_window_destroy(GtkDialog *dialog,gint response, gpointer data) {
	GtkWidget *settings;
	
	settings = glade_xml_get_widget(settings_xml,"settings_window");
	
	gtk_widget_destroy(GTK_WIDGET(settings));
	//gtk_widget_hide(GTK_WIDGET(about));
	g_object_unref(G_OBJECT (settings_xml));
	
	settings_xml = NULL;
}

void on_forward_left_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "FwL");
	sprintf(buffer, "(mv %f %f)", step_mv, M_PI_4);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
	
}

void on_forward_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Fw");
	sprintf(buffer, "(mv %f 0)", step_mv);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_left_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Left");
	sprintf(buffer, "(mv 0 %f)", M_PI_2);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_forward_right_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "FwR");
	sprintf(buffer, "(mv %f %f)", step_mv, -M_PI_4);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_stop_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Stop");
	sprintf(buffer, "(loop_off)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_right_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Right");
	sprintf(buffer, "(mv 0 %f)", -M_PI_2);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_backward_left_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "BwL");
	sprintf(buffer, "(mv %f %f)", step_mv, 3.0*M_PI_4);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_backward_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Bw");
	sprintf(buffer, "(mv -%f 0)", step_mv);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_backward_right_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "BwR");
	sprintf(buffer, "(mv %f %f)", step_mv, -3.0 * M_PI_4);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_make_map_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Make map");
	sprintf(buffer, "(mkmap)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_stop_map_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Stop map");
	sprintf(buffer, "(stopmap)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_followme_map_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Stop map");
	sprintf(buffer, "(followme)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_enough_map_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Stop map");
	sprintf(buffer, "(enough)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_goback_map_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Stop map");
	sprintf(buffer, "(goback)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}


void on_find_zones_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Find zones");
	sprintf(buffer, "(addzn)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_look_around_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Look around");
	sprintf(buffer, "(see)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_find_position_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Find position");
	sprintf(buffer, "(loc)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_where_r_u_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Where are you?");
	sprintf(buffer, "(where_are_you)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_stop_nav_button_clicked(GtkButton *boton, gpointer data) {
	//sprintf(buffer, "Stop nav");
	sprintf(buffer, "(loop_off)");
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_send_cmd_button_clicked(GtkButton *boton, gpointer data) {
	GtkWidget *entry_widget;
	gchar *comando;
	
	entry_widget = glade_xml_get_widget(main_xml,"command_entry");
	
	comando = (gchar*)gtk_entry_get_text(GTK_ENTRY(entry_widget));
	
	sprintf(buffer, comando);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
	
	gtk_entry_set_text(GTK_ENTRY(entry_widget), "");
}

void on_step_hscale_value_changed(GtkAdjustment *adj, gpointer scale) {
	GtkAdjustment *madj;
	GtkWidget *step_range;
	
	step_range = glade_xml_get_widget(main_xml, "step_hscale");
	
	madj = gtk_range_get_adjustment(GTK_RANGE(step_range));
	
	step_mv = (madj->value)/10;
	
	printf("range = %.1d\n", (int)madj->value);
	
	sprintf(buffer, "(step %d)", (int)step_mv);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_zone_size_hscale_value_changed(GtkAdjustment *adj, gpointer scale) {
	GtkAdjustment *madj;
	GtkWidget *step_range;
	
	step_range = glade_xml_get_widget(main_xml, "zone_size_hscale");
	
	madj = gtk_range_get_adjustment(GTK_RANGE(step_range));
	
	printf("range = %.1d\n", (int)madj->value);
	sprintf(buffer, "(sizezn %d)", (int)madj->value);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;

}

void on_blocked_neigh_scale_value_changed(GtkAdjustment *adj, gpointer scale) {
	GtkAdjustment *madj;
	GtkWidget *step_range;
	
	step_range = glade_xml_get_widget(main_xml, "blocked_neigh_scale");
	
	madj = gtk_range_get_adjustment(GTK_RANGE(step_range));
	
	printf("range = %.1d\n", (int)madj->value);
	sprintf(buffer, "(maxocc %d)", (int)madj->value);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;

}

void on_repulsive_hscale_value_changed(GtkAdjustment *adj, gpointer scale) {
	GtkAdjustment *madj;
	GtkWidget *step_range;
	
	step_range = glade_xml_get_widget(main_xml, "repulsive_hscale");
	
	madj = gtk_range_get_adjustment(GTK_RANGE(step_range));
	
	printf("range = %.1d\n", (int)madj->value);
	sprintf(buffer, "(frepul %d)", (int)madj->value);
	out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
	

}

void on_cont_mvment_checkbutton_toggled(GtkWidget *widget, gpointer data) {
	GtkWidget *check_button;
	
	check_button = glade_xml_get_widget(main_xml, "cont_mvment_checkbutton");
	
	if (GTK_TOGGLE_BUTTON (check_button)->active) {
		sprintf(buffer, "(cont_on)");
		out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
		buffer[0] = 0;
	}
	else {
		sprintf(buffer, "(cont_off)");
		out_virbot_socket.msocketTxData( buffer, BUFFERSIZE);
		buffer[0] = 0;
	}
}

/*****************************************************/
void on_clear_button_setts_clicked(GtkButton *boton, gpointer data) {
	GtkWidget *ip_virbot_entry, *ip_head_entry, *ip_control_entry, *ip_vision_entry, *ip_arm_entry;
	GtkWidget *port_virbot_entry, *port_head_entry, *port_control_entry, *port_vision_entry, *port_arm_entry;
	
	ip_virbot_entry = glade_xml_get_widget(settings_xml,"ip_virbot_entry_setts");
	port_virbot_entry = glade_xml_get_widget(settings_xml,"port_virbot_entry_setts");
	
	ip_head_entry = glade_xml_get_widget(settings_xml,"ip_head_entry_setts");
	port_head_entry = glade_xml_get_widget(settings_xml,"port_head_entry_setts");
	
	ip_control_entry = glade_xml_get_widget(settings_xml,"ip_control_entry_setts");
	port_control_entry = glade_xml_get_widget(settings_xml,"port_control_entry_setts");
	
	ip_vision_entry = glade_xml_get_widget(settings_xml,"ip_vision_entry_setts");
	port_vision_entry = glade_xml_get_widget(settings_xml,"port_vision_entry_setts");
	
	ip_arm_entry = glade_xml_get_widget(settings_xml,"ip_arm_entry_setts");
	port_arm_entry = glade_xml_get_widget(settings_xml,"port_arm_entry_setts");
	
	gtk_entry_set_text(GTK_ENTRY(ip_virbot_entry), "");
	gtk_entry_set_text(GTK_ENTRY(port_virbot_entry), "");
	
	gtk_entry_set_text(GTK_ENTRY(ip_head_entry), "");
	gtk_entry_set_text(GTK_ENTRY(port_head_entry), "");
	
	gtk_entry_set_text(GTK_ENTRY(ip_control_entry), "");
	gtk_entry_set_text(GTK_ENTRY(port_control_entry), "");
	
	gtk_entry_set_text(GTK_ENTRY(ip_vision_entry), "");
	gtk_entry_set_text(GTK_ENTRY(port_vision_entry), "");
	
	gtk_entry_set_text(GTK_ENTRY(ip_arm_entry), "");
	gtk_entry_set_text(GTK_ENTRY(port_arm_entry), "");
}

void on_cancel_button_setts_clicked(GtkButton *boton, gpointer data) {
	GtkWidget *settings;
	
	settings = glade_xml_get_widget(settings_xml,"settings_window");
	
	gtk_widget_destroy(GTK_WIDGET(settings));
	g_object_unref(G_OBJECT (settings_xml));
	
	settings_xml = NULL;
		
}

void on_ok_button_setts_clicked(GtkButton *boton, gpointer data) {
	GtkWidget *ip_virbot_entry, *ip_head_entry, *ip_control_entry, *ip_vision_entry, *ip_arm_entry;
	GtkWidget *port_virbot_entry, *port_head_entry, *port_control_entry, *port_vision_entry, *port_arm_entry;
	GtkWidget *settings;
	
	gchar *outport_virbot_char, *outport_head_char, *outport_control_char, *outport_vision_char, *outport_arm_char;
	
	ip_virbot_entry = glade_xml_get_widget(settings_xml,"ip_virbot_entry_setts");
	port_virbot_entry = glade_xml_get_widget(settings_xml,"port_virbot_entry_setts");
	
	ip_head_entry = glade_xml_get_widget(settings_xml,"ip_head_entry_setts");
	port_head_entry = glade_xml_get_widget(settings_xml,"port_head_entry_setts");
	
	ip_control_entry = glade_xml_get_widget(settings_xml,"ip_control_entry_setts");
	port_control_entry = glade_xml_get_widget(settings_xml,"port_control_entry_setts");
	
	ip_vision_entry = glade_xml_get_widget(settings_xml,"ip_vision_entry_setts");
	port_vision_entry = glade_xml_get_widget(settings_xml,"port_vision_entry_setts");
	
	ip_vision_entry = glade_xml_get_widget(settings_xml,"ip_arm_entry_setts");
	port_vision_entry = glade_xml_get_widget(settings_xml,"port_arm_entry_setts");
	
	settings = glade_xml_get_widget(settings_xml,"settings_window");
	
	host_virbot = (gchar*)gtk_entry_get_text(GTK_ENTRY(ip_virbot_entry));
	outport_virbot_char = (gchar*)gtk_entry_get_text(GTK_ENTRY(port_virbot_entry));
	
	host_head = (gchar*)gtk_entry_get_text(GTK_ENTRY(ip_head_entry));
	outport_head_char = (gchar*)gtk_entry_get_text(GTK_ENTRY(port_head_entry));
	
	host_control = (gchar*)gtk_entry_get_text(GTK_ENTRY(ip_control_entry));
	outport_control_char = (gchar*)gtk_entry_get_text(GTK_ENTRY(port_control_entry));
	
	host_vision = (gchar*)gtk_entry_get_text(GTK_ENTRY(ip_vision_entry));
	outport_vision_char = (gchar*)gtk_entry_get_text(GTK_ENTRY(port_vision_entry));
	
	host_arm = (gchar*)gtk_entry_get_text(GTK_ENTRY(ip_arm_entry));
	outport_arm_char = (gchar*)gtk_entry_get_text(GTK_ENTRY(port_arm_entry));
	
	outport_virbot		= atoi(outport_virbot_char);
	outport_head		= atoi(outport_head_char);
	outport_control	= atoi(outport_control_char);
	outport_vision		= atoi(outport_vision_char);
	outport_arm		= atoi(outport_arm_char);
	
	printf("host virbot  = %s\n", host_virbot);
	printf("outport virbot = %d\n", outport_virbot);
	
	printf("host head  = %s\n", host_head);
	printf("outport head = %d\n", outport_head);
	
	printf("host control  = %s\n", host_control);
	printf("outport control = %d\n", outport_control);
	
	printf("host vision  = %s\n", host_vision);
	printf("outport vision = %d\n", outport_vision);
	
	printf("host arm  = %s\n", host_arm);
	printf("outport arm = %d\n", outport_arm);
	
	init_virbot_sockets	(outport_virbot,	host_virbot);
	init_head_sockets		(outport_head,		host_head);
	init_control_sockets	(outport_control,	host_control);
	init_vision_sockets	(outport_vision,	host_vision);
	init_arm_sockets	(outport_arm,	host_arm);
	
	gtk_widget_destroy(GTK_WIDGET(settings));
	g_object_unref(G_OBJECT (settings_xml));
	
	settings_xml = NULL;
}

void on_button_Sad_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "sad");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Angry_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "angry");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Happy_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "happy");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Yes_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "yes");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_No_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "no");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Left_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "left");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Right_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "right");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Up_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "up");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Down_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "down");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Close_Mouth_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "close_mouth");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Open_Mouth_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "open_mouth");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Left_Eyes_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "left_eyes");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Right_Eyes_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "right_eyes");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Up_Eyes_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "up_eyes");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Down_Eyes_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "down_eyes");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_Talk_Head_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "say 1 2 3 4 testing");
	out_head_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_raise_arm_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "raise");
	out_arm_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_close_arm_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "close");
	out_arm_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_lower_arm_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "lower");
	out_arm_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_popen_arm_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "popen");
	out_arm_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_open_arm_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "open");
	out_arm_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_pclose_arm_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "pclose");
	out_arm_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_take_arm_clicked(GtkButton *boton, gpointer data) {
	sprintf(buffer, "take");
	out_arm_socket.msocketTxData( buffer, BUFFERSIZE);
	buffer[0] = 0;
}

void on_button_virbot_start_clicked	(GtkButton *boton, gpointer data) {
	virbot_pid = fork();
	
	if(virbot_pid == 0) {
		//execl(XTERM_CMD, VIRBOT_CMD, NULL);
		execl("/usr/bin/xterm", "xterm", "/home/robocup/atlanta/ViRbot/bin/virbot_linux", NULL);
		_exit(EXIT_FAILURE);
	}
	
}

void on_button_virbot_quit_clicked	(GtkButton *boton, gpointer data) {
	kill(virbot_pid, SIGTERM);
}

void on_head_start_clicked	(GtkButton *boton, gpointer data) {
	head_pid = fork();
	
	if(head_pid == 0) {
		execl(XTERM_CMD, HEAD_CMD, NULL);
		_exit(EXIT_FAILURE);
	}
}

void on_button_head_quit_clicked		(GtkButton *boton, gpointer data) {
	kill(head_pid, SIGTERM);
}

void on_button_control_start_clicked(GtkButton *boton, gpointer data) {
	control_pid = fork();
	
	if(control_pid == 0) {
		execl("/usr/bin/xterm", "xterm", "/home/robocup/atlanta/control/bin/RobotP", NULL);
		_exit(EXIT_FAILURE);
	}
}

void on_button_control_quit_clicked	(GtkButton *boton, gpointer data) {
	kill(control_pid, SIGTERM);
}

void on_button_arm_start_clicked	(GtkButton *boton, gpointer data) {
	virbot_pid = fork();
	
	if(virbot_pid == 0) {
		//execl(XTERM_CMD, VIRBOT_CMD, NULL);
		execl("/usr/bin/xterm", "xterm", "/home/robocup/atlanta/brazo/bin/Arm", NULL);
		_exit(EXIT_FAILURE);
	}
	
}

void on_button_arm_quit_clicked	(GtkButton *boton, gpointer data) {
	kill(arm_pid, SIGTERM);
}


}

#endif /* _CALLBACKS_H */
