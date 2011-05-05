/* Created by Anjuta version 1.2.2 */
/*	This file will not be overwritten */

#include <gnome.h>
#include <config.h>
#include "callbacks.h"


int main(int argc, char *argv[])
{
	GnomeProgram *tx8_vcc;
	GError *error = NULL;
	
	tx8_vcc = gnome_program_init("tx8-vcc",VERSION,LIBGNOMEUI_MODULE,argc,argv,NULL);
	
	/* init threads */
  	if (!g_thread_supported ()) 
		g_thread_init (NULL);
  	gdk_threads_init();
	
	init_listener_socket();
	tx8_vcc_main();
	
	/* create the threads */
	if (!g_thread_create(listen_message, NULL, FALSE, &error)) {
   	g_printerr ("Failed to create \"listen_message\" thread: %s\n", error->message);
      return 1;
   }
	
	if(!g_thread_create(draw_message, &mi_pizarron, FALSE, &error)) {
		g_printerr("Failed to create \"draw_message\" thread: %s\n", error->message);
		return 1;
	}
	
	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();
	
	kill_sockets();
	
	return(0);
}
