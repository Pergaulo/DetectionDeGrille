#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <cairo.h>

void draw_sudoku(cairo_t *cr, int width, int height, const char puzzle[9][13], const char solution[9][13]) {
    // Calculate cell size based on width and height
    double cell_width = (double)width / 9.0;
    double cell_height = (double)height / 9.0;

    cairo_set_source_rgb(cr, 0, 0, 0); // Set color to black

    // Draw Sudoku grid
    for (int i = 0; i <= 9; ++i) {
        cairo_set_line_width(cr, (i % 3 == 0) ? 2.0 : 1.0); // thicker lines for every third line
        cairo_move_to(cr, i * cell_width, 0);
        cairo_line_to(cr, i * cell_width, height);
        cairo_stroke(cr);

        cairo_move_to(cr, 0, i * cell_height);
        cairo_line_to(cr, width, i * cell_height);
        cairo_stroke(cr);
    }

    cairo_set_font_size(cr, cell_height * 0.6); // Set font size based on cell height

    char to_print[2] = ""; // Character array to hold a single character

    int is_passed = 0;

    for (int i = 0; i < 9; ++i) {
        is_passed = 0;
        for (int j = 0; j < 12; ++j) {
            if (puzzle[i][j] == ' ' || puzzle[i][j] == '\n' || puzzle[i][j] == '\0') {
                is_passed += 1;
                continue;
            }

            if (puzzle[i][j] != '.') {
                to_print[0] = solution[i][j]; // Assign the character to the array
                cairo_move_to(cr, (j - is_passed) * cell_width + cell_width * 0.3, i * cell_height + cell_height * 0.7);
                cairo_show_text(cr, to_print); // Pass the character array
            } else {
                cairo_set_source_rgb(cr, 1, 0, 0); // Set color to red for solution numbers
                to_print[0] = solution[i][j]; // Assign the character to the array
                cairo_move_to(cr, (j - is_passed) * cell_width + cell_width * 0.3, i * cell_height + cell_height * 0.7);
                cairo_show_text(cr, to_print); // Pass the character array
                cairo_set_source_rgb(cr, 0, 0, 0); // Reset color to black for subsequent numbers
            }
        }
    }
}

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer data) {
    int width, height;
    GtkAllocation alloc;

    gtk_widget_get_allocation(widget, &alloc);
    width = alloc.width;
    height = alloc.height;

    const char *puzzle_file = ((const char **)data)[0];
    const char *solution_file = ((const char **)data)[1];

    FILE *puzzle_fp = fopen(puzzle_file, "r");
    FILE *solution_fp = fopen(solution_file, "r");

    if (puzzle_fp == NULL || solution_fp == NULL) {
        perror("Error opening files");
        return FALSE;
    }

    char puzzle[9][13];
    char solution[9][13];

    char unused_line[13];

    int is_passed = 0;
    
    // Read puzzle file
    for (int i = 0; i < 11; ++i) {
        if (i == 3 || i == 7)
        {
            fgets(unused_line, 13, puzzle_fp);
            is_passed += 1;
            continue;
        }
        if (fgets(puzzle[i - is_passed], sizeof(puzzle[i- is_passed]), puzzle_fp) == NULL) {
            fprintf(stderr, "Error reading puzzle file\n");
            return FALSE;
        }
    }

    is_passed = 0;
    // Read solution file
    for (int i = 0; i < 11; ++i) {

        if (i == 3 || i == 7)
        {
            fgets(unused_line, 13, solution_fp);
            is_passed += 1;
            continue;
        }
        if (fgets(solution[i-is_passed], sizeof(solution[i-is_passed]), solution_fp) == NULL) {
            fprintf(stderr, "Error reading solution file\n");
            return FALSE;
        }
    }

    fclose(puzzle_fp);
    fclose(solution_fp);

    draw_sudoku(cr, width, height, puzzle, solution); // Removed the '&' before puzzle and solution
    // Create a surface to hold the drawing for the window
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *surface_cr = cairo_create(surface);

    // Set the fill color for the background
    cairo_set_source_rgb(surface_cr, 255, 255, 255); // Set RGB color values (0.0 - 1.0)

    // Paint the background rectangle with the chosen color
    cairo_rectangle(surface_cr, 0, 0, width, height);
    cairo_fill(surface_cr);

    // Draw directly onto the surface associated with the window
    draw_sudoku(surface_cr, width, height, puzzle, solution);

    // Get the window's context and draw the surface onto it
    gdk_cairo_set_source_pixbuf(cr, (const GdkPixbuf *)surface, 0, 0);
    cairo_paint(cr);

    // Convert the surface to a PNG file
    cairo_surface_write_to_png(surface, "sudoku_image.png");

    // Clean up resources
    cairo_surface_destroy(surface);
    cairo_destroy(surface_cr);

    return FALSE;
}

void __destroy_window(GtkWidget *window) {
    gtk_widget_destroy(window);
}

gboolean destroy_window_delayed(GtkWidget *window) {
    __destroy_window(window);
    return G_SOURCE_REMOVE;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <puzzle_file> <solution_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    GtkWidget *window;
    GtkWidget *darea;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    darea = gtk_drawing_area_new();

    gtk_container_add(GTK_CONTAINER(window), darea);

    const char *files[2] = {argv[1], argv[2]}; // Paths to puzzle and solution files

    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), (gpointer)files);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 450, 450);
    gtk_window_set_title(GTK_WINDOW(window), "Sudoku Image");

    gtk_widget_show_all(window);

    // Schedule the window destruction after a delay (in milliseconds)
    g_timeout_add(50, (GSourceFunc)destroy_window_delayed, window);

    gtk_main();

    return EXIT_SUCCESS;
}