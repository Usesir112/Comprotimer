#include <gtk/gtk.h>

enum {
    STARTED,
    PAUSED,
    STOPPED
};

enum {
    N_LAP,
    TIME,
    N_COLUMNS
};

enum {
    STOPWATCHF,
    EGG,
    MAMA,
    INFI
};

GTimer *stopwatch;

gint count = INFI;
gint state = STOPPED;
gint laps = 0;
gint choose = STOPWATCHF;

GtkWidget *stopwatch_display;
GtkWidget *button_stopwatch;
GtkWidget *button_egg;
GtkWidget *button_mama;
GtkWidget *button_funcs;
GtkWidget *tree;
GSimpleAction *message_action;
GtkWidget *message_dialog;
GtkListStore *liststore;
GtkTreeSelection *selection;
GtkTreeIter selection_iter, iter;
GdkColor color;
gboolean show_milliseconds = TRUE;

char output[100];
char output_old[100];


static void
on_response (GtkDialog *dialog,
             gint       response_id,
             gpointer   user_data)
{

  /* ปิดหน้าต่างแสดงข้อความแจ้งเตือน */
  gtk_widget_destroy (GTK_WIDGET (dialog));

}

gboolean stopwatch_function (void) {
	/* ฟังก์ชันคำนวณเวลา */
	gchar *markup;
	int hours;
	int minutes;
	double seconds;

	seconds = g_timer_elapsed(stopwatch, NULL);

	hours = seconds / 3600;
	seconds -= 3600 * hours;
	minutes = seconds / 60;
	seconds -= 60 * minutes;
	if(show_milliseconds)
		sprintf(output, "%02d:%02d:%.1f", hours, minutes, seconds);
	else
		sprintf(output, "%02d:%02d:%02d", hours, minutes, (int)seconds);

	if(strcmp(output, output_old) != 0) {
		gtk_label_set_text(GTK_LABEL(stopwatch_display), output);
		markup = g_markup_printf_escaped("<span font=\"48\" weight=\"heavy\"><tt>%s</tt></span>", output);
		gtk_label_set_markup(GTK_LABEL(stopwatch_display), markup);
		g_free (markup);
	}

	strcpy(output_old, output);	
	/* แจ้งเตือนเมื่อต้มไข่เสร็จพร้อมกับหยุดเวลา */
	if(minutes>=5 && choose == EGG){
		state == STARTED;
		on_egg_button_clicked();

		on_funcs_button_clicked();
		count = INFI;
		choose = STOPWATCHF;
		message_dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, 
                                            GTK_MESSAGE_INFO, 
                                            GTK_BUTTONS_OK, 
                                            "ไข่ต้มของคุณสุกแล้วพร้อมรับประทาน กดตกลงแล้วไปกินกันเถอะ~~");
		gtk_widget_show_all (message_dialog);
		g_signal_connect (GTK_DIALOG (message_dialog), "response", 
                    G_CALLBACK (on_response), NULL);
	}
	/* แจ้งเตือนเมื่อต้มมาม่าเสร็จพร้อมกับหยุดเวลา */
	else if(minutes>=3 && choose == MAMA){
		state == STARTED;
		on_mama_button_clicked();

		on_funcs_button_clicked();
		count = INFI;
		choose = STOPWATCHF;
		message_dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, 
                                            GTK_MESSAGE_INFO, 
                                            GTK_BUTTONS_OK, 
                                            "มาม่าของคุณสุกแล้วพร้อมรับประทาน กดตกลงแล้วไปปรุงรสกันเถอะ~~");
		gtk_widget_show_all (message_dialog);
		g_signal_connect (GTK_DIALOG (message_dialog), "response", 
                    G_CALLBACK (on_response), NULL);
	}
	return TRUE;
}

void toggle_millisecond(void) {
	/* แสดงเสี้ยววินาที */
		if(show_milliseconds) 
			show_milliseconds = FALSE;
		else
			show_milliseconds = TRUE;
}

void add_lap (void) {
	/* เพิ่มรอบเวลาแสดงในด้านล่าง */
	laps++;
	/* ดึงเวลามาแสดง */
	gtk_list_store_append(GTK_LIST_STORE(liststore), &iter);
	gtk_list_store_set(GTK_LIST_STORE(liststore), &iter, N_LAP, laps, TIME, output, -1);
	gtk_tree_selection_select_iter(gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)), &iter);
}

void on_stopwatch_button_clicked (void) {
	/* ฟังก์ชันสำหรับปุ่มจับเวลาในแต่ละสถานะ */
	if(state == STOPPED) {
		gdk_color_parse("#C73333", &color);
		gtk_widget_modify_fg(GTK_WIDGET(button_stopwatch), GTK_STATE_NORMAL, &color);
		gtk_button_set_label(GTK_BUTTON(button_stopwatch), "หยุด");
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_mama), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_egg), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "รอบ");
		g_timer_start(stopwatch);
		state = STARTED;
		choose = STOPWATCHF;
	} else if(state == PAUSED) {
		gdk_color_parse("#C73333", &color);
		gtk_widget_modify_fg(GTK_WIDGET(button_stopwatch), GTK_STATE_NORMAL, &color);
		gtk_button_set_label(GTK_BUTTON(button_stopwatch), "หยุด");
		gtk_widget_set_sensitive(GTK_WIDGET(button_mama), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_egg), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "รอบ");
		g_timer_continue(stopwatch);
		state = STARTED;
	} else if(state == STARTED) {
		gdk_color_parse("#67953C", &color);
		gtk_widget_modify_fg(GTK_WIDGET(button_stopwatch), GTK_STATE_NORMAL, &color);
		gtk_button_set_label(GTK_BUTTON(button_stopwatch), "จับเวลาต่อ");
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_mama), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_egg), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "รีเซ็ต");
		g_timer_stop(stopwatch);
		state = PAUSED;
	}
}

void on_egg_button_clicked (void) {
	/* ฟังก์ชันสำหรับปุ่มจับเวลาต้มไข่ */
	if(state == STOPPED) {
		gdk_color_parse("#C73333", &color);
		gtk_widget_modify_fg(GTK_WIDGET(button_egg), GTK_STATE_NORMAL, &color);
		gtk_button_set_label(GTK_BUTTON(button_egg), "หยุดต้มไข่");
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_mama), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_stopwatch), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "รอบ");
		g_timer_start(stopwatch);
		state = STARTED;
		count = 5;
		choose = EGG;
	} else if(state == PAUSED) {
		gdk_color_parse("#C73333", &color);
		gtk_widget_modify_fg(GTK_WIDGET(button_egg), GTK_STATE_NORMAL, &color);
		gtk_button_set_label(GTK_BUTTON(button_egg), "หยุดต้มไข่");
		gtk_widget_set_sensitive(GTK_WIDGET(button_mama), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_stopwatch), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "รอบ");
		g_timer_continue(stopwatch);
		state = STARTED;
		count = 5;
		choose = EGG;
	} else if(state == STARTED) {
		gdk_color_parse("#67953C", &color);
		gtk_widget_modify_fg(GTK_WIDGET(button_egg), GTK_STATE_NORMAL, &color);
		gtk_button_set_label(GTK_BUTTON(button_egg), "ต้มไข่ต่อ");
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_mama), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_stopwatch), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "รีเซ็ต");
		g_timer_stop(stopwatch);
		state = PAUSED;
		choose = EGG;
	}
}

void on_mama_button_clicked (void) {
	/* ฟังก์ชันสำหรับปุ่มจับเวลาต้มมาม่า */
	if(state == STOPPED) {
		gdk_color_parse("#C73333", &color);
		gtk_widget_modify_fg(GTK_WIDGET(button_mama), GTK_STATE_NORMAL, &color);
		gtk_button_set_label(GTK_BUTTON(button_mama), "หยุดต้มมาม่า");
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_egg), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_stopwatch), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "Lap");
		g_timer_start(stopwatch);
		state = STARTED;
		count = 3;
		choose = MAMA;
	} else if(state == PAUSED) {
		gdk_color_parse("#C73333", &color);
		gtk_widget_modify_fg(GTK_WIDGET(button_mama), GTK_STATE_NORMAL, &color);
		gtk_button_set_label(GTK_BUTTON(button_mama), "หยุดต้มมาม่า");
		gtk_button_set_label(GTK_BUTTON(button_funcs), "รอบ");
		gtk_widget_set_sensitive(GTK_WIDGET(button_egg), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_stopwatch), FALSE);
		g_timer_continue(stopwatch);
		state = STARTED;
		count = 3;
		choose = MAMA;
	} else if(state == STARTED) {
		gdk_color_parse("#67953C", &color);
		gtk_widget_modify_fg(GTK_WIDGET(button_mama), GTK_STATE_NORMAL, &color);
		gtk_button_set_label(GTK_BUTTON(button_mama), "ต้มมาม่าต่อ");
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_egg), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_stopwatch), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "รีเซ็ต");
		g_timer_stop(stopwatch);
		state = PAUSED;
		choose = MAMA;
	}
}

void on_funcs_button_clicked (void) {
	/* ฟังก์ชันสำหรับปุ่มรีเซ็ตและจับรอบเวลา */
	if(state == STARTED)
		add_lap();
	else if(state == PAUSED) {
		g_timer_start(stopwatch);
		g_timer_stop(stopwatch);
		laps = 0;
		state = STOPPED;
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "รีเซ็ต");
		gtk_button_set_label(GTK_BUTTON(button_stopwatch), "เริ่ม");
		gtk_button_set_label(GTK_BUTTON(button_egg), "ต้มไข่ 5 นาที");
		gtk_button_set_label(GTK_BUTTON(button_mama), "ต้มมาม่า 3 นาที");
		gtk_widget_set_sensitive(GTK_WIDGET(button_stopwatch), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_egg), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_mama), TRUE);
		gtk_list_store_clear(GTK_LIST_STORE(liststore));
		count = -1;
		choose = STOPWATCHF;
	}
}

int main (void) {
	/* ฟังก์ชันหลัก(หน้าต่าง+เมนู) */
	GtkWidget *window, *vbox, *hbox, *scroll, *click;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

	gtk_init(NULL, NULL);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	stopwatch_display = gtk_label_new("");

	button_stopwatch = gtk_button_new_with_label("เริ่ม");
	gdk_color_parse("#67953C", &color);
	gtk_widget_modify_fg(GTK_WIDGET(button_stopwatch), GTK_STATE_NORMAL, &color);
	button_egg = gtk_button_new_with_label("ต้มไข่ 5 นาที");
	gdk_color_parse("#67953C", &color);
	gtk_widget_modify_fg(GTK_WIDGET(button_egg), GTK_STATE_NORMAL, &color);
	button_mama = gtk_button_new_with_label("ต้มมาม่า 3 นาที");
	gdk_color_parse("#67953C", &color);
	gtk_widget_modify_fg(GTK_WIDGET(button_mama), GTK_STATE_NORMAL, &color);
	button_funcs = gtk_button_new_with_label("รีเซ็ต");
	gtk_widget_set_sensitive(button_funcs, FALSE);
	scroll = gtk_scrolled_window_new (NULL, NULL);
	g_object_set (scroll, "shadow-type", GTK_SHADOW_IN, NULL);

	tree = gtk_tree_view_new();

	liststore = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree), TRUE);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(liststore));

	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column, "รอบที่");

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer, "text", N_LAP, NULL);
	gtk_tree_view_column_set_expand(column, TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("เวลา", renderer, "text", TIME, NULL);
	gtk_tree_view_column_set_expand(column, TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(tree), FALSE);

	click = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(click), stopwatch_display);
	gtk_box_pack_start(GTK_BOX(vbox), click, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_stopwatch, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_egg, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_mama, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_funcs, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	gtk_container_add(GTK_CONTAINER(scroll), tree);
	gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window), "ตัวจับเวลาอัจฉริยะ");
	gtk_window_set_default_icon_name("clocks");
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);

	stopwatch = g_timer_new();
	g_timer_stop(stopwatch);


	g_timeout_add_full(G_PRIORITY_HIGH, 50, (GSourceFunc) stopwatch_function, NULL, NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(button_stopwatch, "clicked", G_CALLBACK(on_stopwatch_button_clicked), NULL);
	g_signal_connect(button_egg, "clicked", G_CALLBACK(on_egg_button_clicked), NULL);
	g_signal_connect(button_mama, "clicked", G_CALLBACK(on_mama_button_clicked), NULL);
	g_signal_connect(button_funcs, "clicked", G_CALLBACK(on_funcs_button_clicked), NULL);
	

	g_signal_connect(click, "button_press_event", G_CALLBACK(toggle_millisecond), NULL);
	gtk_main();
	g_timer_destroy(stopwatch);
	
	return 0;
}
