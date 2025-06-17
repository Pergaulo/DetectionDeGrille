#include <gtk/gtk.h>

GtkWidget *image;
GtkWidget *load_image_button;
GtkWidget *button1, *button2, *button3, *button4, *button5, *button6;
char *current_image_filename = NULL;

// Fixed size for loaded images
#define FIXED_IMAGE_WIDTH 900
#define FIXED_IMAGE_HEIGHT 800

void close_popup(GtkWidget *widget, gpointer data) {
    gtk_widget_destroy(GTK_WIDGET(data));
}

// Function "Load Image"
void on_load_image_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Veuillez choisir une image",
                                         GTK_WINDOW(data),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        // Image
        if (current_image_filename != NULL) {
            g_free(current_image_filename);
        }
        current_image_filename = g_strdup(filename); // Duplicate

        // Image Update
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);

        // Scale the loaded image to the fixed size
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, FIXED_IMAGE_WIDTH, FIXED_IMAGE_HEIGHT, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);

        gtk_widget_set_visible(GTK_WIDGET(button1), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(button2), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button3), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button4), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button5), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(button6), FALSE);


        g_print("Image chargée : %s\n", current_image_filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void show_popup() {
    // Créer une boîte de dialogue
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Information",
                                                    NULL,
                                                    GTK_DIALOG_MODAL,
                                                    NULL);

    // Ajouter un libellé à la boîte de dialogue
    GtkWidget *label = gtk_label_new("Lines detection\nThe image will be cropped and the cells will be detected");

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Ajouter le libellé à la boîte de dialogue
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show(label);

    // Ajouter un bouton "Close" à la boîte de dialogue
    GtkWidget *close_button = gtk_button_new_with_label("Close");
    g_signal_connect(close_button, "clicked", G_CALLBACK(close_popup), dialog);

    gtk_container_add(GTK_CONTAINER(content_area), close_button);
    gtk_widget_show(close_button);

    // Afficher tous les éléments de la boîte de dialogue
    gtk_widget_show(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
}

// Fonction "Black and White"
void on_black_and_white_button_clicked(GtkWidget *widget, gpointer data) {
	chdir("LesFiltres");
	system("make");
    if (current_image_filename != NULL) {
        char command[256];
        snprintf(command, sizeof(command), "./couleur %s", current_image_filename);
        system(command);
    }

    current_image_filename = g_strdup("test_blackandwhite.jpg"); // Dupliquer le nom de fichier
	system("mv test_blackandwhite.jpg ..");
	system("make clean");
	chdir("..");
    // Mettre à jour l'image
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("test_blackandwhite.jpg", NULL);

        // Scale the loaded image to the fixed size
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, FIXED_IMAGE_WIDTH, FIXED_IMAGE_HEIGHT, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);

        gtk_widget_set_visible(GTK_WIDGET(button1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button2), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(button3), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button4), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button5), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(button6), FALSE);
}

// Fonction "Rotation"
void on_rotation_button_clicked(GtkWidget *widget, gpointer data) {
	system("mv test_blackandwhite.jpg LaRotation");
	chdir("LaRotation");
	system("make");
    if (current_image_filename != NULL) {
        char command[256];
        snprintf(command, sizeof(command), "./rotation %s", current_image_filename);
        system(command);
    }
    current_image_filename = g_strdup("rotation.jpg"); // Dupliquer le nom de fichier
	system("mv rotation.jpg ..");
	system("rm test_blackandwhite.jpg");
	system("make clean");
	chdir("..");
    // Mettre à jour l'image
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(current_image_filename, NULL);

        // Scale the loaded image to the fixed size
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, FIXED_IMAGE_WIDTH, FIXED_IMAGE_HEIGHT, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);

        gtk_widget_set_visible(GTK_WIDGET(button1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button2), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button3), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(button4), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button5), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(button6), FALSE);

}

// Fonction "Detection"
void on_detection_button_clicked(GtkWidget *widget, gpointer data) {
	system("mv rotation.jpg LeHough");
	chdir("LeHough");
	system("make");
    if (current_image_filename != NULL) {
        char command[256];
        snprintf(command, sizeof(command), "./hough %s", current_image_filename);
        system(command);
    }

    current_image_filename = g_strdup("line.jpg"); // Dupliquer le nom de fichier
	system("mv cropped.jpg ..");
	system("mv line.jpg ..");
	system("rm rotation.jpg");
	system("make clean");
	chdir("..");
    // Mettre à jour l'image
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("line.jpg", NULL);

        // Scale the loaded image to the fixed size
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, FIXED_IMAGE_WIDTH, FIXED_IMAGE_HEIGHT, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);

        gtk_widget_set_visible(GTK_WIDGET(button1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button2), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button3), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button4), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(button5), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(button6), FALSE);


	show_popup();
	current_image_filename = g_strdup("cropped.jpg"); // Dupliquer le nom de fichier

	GdkPixbuf *pixbu = gdk_pixbuf_new_from_file("cropped.jpg", NULL);

	system("mv cropped.jpg LaDetectionCase");
	chdir("LaDetectionCase");
	system("make");
	if (current_image_filename != NULL) {
        char command[256];
        snprintf(command, sizeof(command), "./segmentation %s", current_image_filename);
        system(command);
	}
	system("mv segment.jpg ..");
	system("mv cropped.jpg ..");
	system("mv image_test_nn ..");
	system("make clean");
	chdir("..");

        // Scale the loaded image to the fixed size
        GdkPixbuf *scaled_pixbu = gdk_pixbuf_scale_simple(pixbu, FIXED_IMAGE_WIDTH, FIXED_IMAGE_HEIGHT, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbu);

        gtk_widget_set_visible(GTK_WIDGET(button1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button2), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button3), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button4), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(button5), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(button6), FALSE);

	show_popup();
	current_image_filename = g_strdup("segment.jpg"); // Dupliquer le nom de fichier
	GdkPixbuf *pixb = gdk_pixbuf_new_from_file("segment.jpg", NULL);

	// Scale the loaded image to the fixed size
        GdkPixbuf *scaled_pixb = gdk_pixbuf_scale_simple(pixb, FIXED_IMAGE_WIDTH, FIXED_IMAGE_HEIGHT, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixb);

        gtk_widget_set_visible(GTK_WIDGET(button1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button2), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button3), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button4), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(button5), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(button6), FALSE);

}

// Fonction "Solver"
void on_solver_button_clicked(GtkWidget *widget, gpointer data) {
	system("mv image_test_nn neural_network");
	chdir("neural_network");
	system("make");
    if (current_image_filename != NULL) {
        char command[256];
        snprintf(command, sizeof(command), "./main");
        system(command);
    }
    system("mv grid_00 ../Resolve/Solver");
    system("make clean");
    system("rm -r image_test_nn");
    chdir("../Resolve/Solver");
    system("make");
    if (current_image_filename != NULL) {
        char command[256];
        snprintf(command, sizeof(command), "./solver %s", "grid_00");
        system(command);
    }
    system("mv grid_00.result ../Reconstrucion");
    system("mv grid_00 ../Reconstrucion");
    system("make clean");
    chdir("../Reconstrucion");
    system("gcc plain.c -o plain $(pkg-config --cflags --libs gtk+-3.0)");
    system("./plain grid_00 grid_00.result");
    system("mv sudoku_image.png ../..");
    system("rm grid_00");
    system("rm grid_00.result");
    system("rm plain");
    chdir("../..");
    system("make clean");
    // Dupliquer le nom de fichier
    // Mettre à jour l'image
    	current_image_filename = g_strdup("sudoku_image.png"); // Dupliquer le nom de fichier
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(current_image_filename, NULL);

        // Scale the loaded image to the fixed size
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, FIXED_IMAGE_WIDTH, FIXED_IMAGE_HEIGHT, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);

        gtk_widget_set_visible(GTK_WIDGET(button1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button2), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button3), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button4), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button5), TRUE);
	gtk_widget_set_visible(GTK_WIDGET(button6), FALSE);
}

// Fonction "Save"
void on_save_button_clicked(GtkWidget *widget, gpointer data) {
	current_image_filename = g_strdup("sudoku_image.png"); // Dupliquer le nom de fichier
	
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(current_image_filename, NULL);
        // Scale the loaded image to the fixed size
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, FIXED_IMAGE_WIDTH, FIXED_IMAGE_HEIGHT, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);

        gtk_widget_set_visible(GTK_WIDGET(button1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button2), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button3), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button4), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button5), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(button6), FALSE);

}

// Fonction "Rota"
void on_rota_button_clicked(GtkWidget *widget, gpointer data) {
    
    // Mettre à jour l'image
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(current_image_filename, NULL);
	GdkPixbuf *rotated_pixbuf = gdk_pixbuf_rotate_simple(pixbuf, GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);

	gdk_pixbuf_save(rotated_pixbuf, "rotation.jpg", "jpg", NULL, NULL);

	current_image_filename = g_strdup("rotation.jpg"); // Dupliquer le nom de fichier
        // Scale the loaded image to the fixed size
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(rotated_pixbuf, FIXED_IMAGE_WIDTH, FIXED_IMAGE_HEIGHT, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);

        gtk_widget_set_visible(GTK_WIDGET(button1), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button2), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button3), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(button4), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(button5), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(button6), TRUE);
}





// Fonction "configure-event" window
gboolean on_window_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
    if (image != NULL) {
        GdkPixbuf *current_pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
        if (current_pixbuf != NULL) {
            // Window new size
            int width = event -> width;
            int height = event -> height;

            // Image new size
            GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(current_pixbuf, width, height, GDK_INTERP_BILINEAR);
            gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);
        }
    }

    return FALSE;
}

int main(int argc, char *argv[]) 
{
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *grid;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    //GdkRGBA color;
    //gdk_rgba_parse(&color, "rgb(30, 30, 30)");
    //gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &color);

    gtk_window_set_title(GTK_WINDOW(window), "Application De Solver De Sudoku (OCR)");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "configure-event", G_CALLBACK(on_window_configure_event), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);


    // Button "Load Image"
    load_image_button = gtk_button_new_with_label("Load Image");
    g_signal_connect(load_image_button, "clicked", G_CALLBACK(on_load_image_button_clicked), window);

    // Button "Black and White"
    button1 = gtk_button_new_with_label("Pre process");
    g_signal_connect(button1, "clicked", G_CALLBACK(on_black_and_white_button_clicked), NULL);

    // Others Buttons
    button2 = gtk_button_new_with_label("Rotation");
    g_signal_connect(button2, "clicked", G_CALLBACK(on_rotation_button_clicked), NULL);

    button3 = gtk_button_new_with_label("Grid detection");
    g_signal_connect(button3, "clicked", G_CALLBACK(on_detection_button_clicked), NULL);

    button4 = gtk_button_new_with_label("Solver");
    g_signal_connect(button4, "clicked", G_CALLBACK(on_solver_button_clicked), NULL);

    button5 = gtk_button_new_with_label("Save image");
    g_signal_connect(button5, "clicked", G_CALLBACK(on_save_button_clicked), NULL);

    button6 = gtk_button_new_with_label("Click here if the numbers are in the wrong direction");
    g_signal_connect(button6, "clicked", G_CALLBACK(on_rota_button_clicked), NULL);

    // Image widget
    image = gtk_image_new();

    // Set horizontal and vertical alignment of the image to center
    gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(image, GTK_ALIGN_CENTER);

    // Set hexpand and vexpand to TRUE for buttons
    gtk_widget_set_hexpand(load_image_button, TRUE);
    gtk_widget_set_vexpand(load_image_button, TRUE);

    gtk_widget_set_hexpand(button1, TRUE);
    gtk_widget_set_vexpand(button1, TRUE);

    gtk_widget_set_hexpand(button2, TRUE);
    gtk_widget_set_vexpand(button2, TRUE);

    gtk_widget_set_hexpand(button3, TRUE);
    gtk_widget_set_vexpand(button3, TRUE);

    gtk_widget_set_hexpand(button4, TRUE);
    gtk_widget_set_vexpand(button4, TRUE);

    gtk_widget_set_hexpand(button5, TRUE);
    gtk_widget_set_vexpand(button5, TRUE);

    gtk_widget_set_hexpand(button6, TRUE);
    gtk_widget_set_vexpand(button6, TRUE);


    gtk_grid_attach(GTK_GRID(grid), load_image_button, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button1, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button2, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button3, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button4, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button5, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button6, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), image, 0, 2, 3, 1);

    gtk_widget_show_all(window);
    // Hide buttons initially
    gtk_widget_set_visible(GTK_WIDGET(button1), FALSE);
    gtk_widget_set_visible(GTK_WIDGET(button2), FALSE);
    gtk_widget_set_visible(GTK_WIDGET(button3), FALSE);
    gtk_widget_set_visible(GTK_WIDGET(button4), FALSE);
    gtk_widget_set_visible(GTK_WIDGET(button5), FALSE);
    gtk_widget_set_visible(GTK_WIDGET(button6), FALSE);


    gtk_main();

    if (current_image_filename != NULL) {
        g_free(current_image_filename);
    }

    return 0;
}
