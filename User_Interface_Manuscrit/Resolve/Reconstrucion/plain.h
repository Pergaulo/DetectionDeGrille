#ifndef PLAIN_H
#define PLAIN_H

void draw_sudoku(cairo_t *cr, int width, int height, const char puzzle[9][13], const char solution[9][13]);

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer data);



#endif