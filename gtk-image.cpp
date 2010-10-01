#include <gtk/gtk.h>
#include "image.hpp"

static void exit_main_loop (GtkWidget  *widget, GtkWidget **widget_pointer)
{
    gtk_widget_destroyed(widget, widget_pointer);
    gtk_main_quit ();
}

void display (Image & im, const char *title)
{
    GtkWidget *win;
    GtkWidget *preview = gtk_preview_new (GTK_PREVIEW_GRAYSCALE);
    int height = im.get_height();
    int width = im.get_width();
    gtk_preview_size (GTK_PREVIEW (preview), width, height);
    grey **matrix=im.get_matrix();
    for (int i = 0; i < height; i++)
        gtk_preview_draw_row (GTK_PREVIEW (preview),
                              matrix[i], 0, i, width);
    gtk_widget_show (preview);
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_signal_connect (GTK_OBJECT (win), "destroy",
                        GTK_SIGNAL_FUNC(exit_main_loop),
                        &win);
    gtk_window_set_title (GTK_WINDOW (win), title);
    gtk_container_border_width (GTK_CONTAINER (win), 5);
    gtk_container_add (GTK_CONTAINER (win),
                       preview);
    gtk_widget_show (win);
    gtk_main ();
}
