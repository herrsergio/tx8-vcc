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
 
#include <gnome.h>
#include <glade/glade.h>
#include <config.h>
#include "tx8_vcc.h"
#include "callbacks.h"

void tx8_vcc_main(void) {
	GtkWidget *window   = NULL;
	gchar     *xml_file = NULL;
			
	if (!main_xml) {
		xml_file=g_build_filename("../data","tx8-vcc_glade.glade",NULL);
		main_xml=glade_xml_new(xml_file,NULL,NULL);
							    	
		glade_xml_signal_autoconnect(main_xml);
	}
												
	window = glade_xml_get_widget(main_xml,"main_window");
	
	gtk_widget_show(GTK_WIDGET(window));																				
}

/*
int init_sockets(void) {
	if( error = out_socket.msocketInitSocket( HOST, outport, TX_SOCKET ) ) {
   	printf( "error opening server: %s", msocketGetError( error ) ); 
    	exit( 1 ); 
   }
}

void kill_sockets(void) {
	out_socket.msocketKillSocket();
}
*/
