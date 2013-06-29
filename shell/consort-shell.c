#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>

#include "consort-shell.h"
#include "consort-shell-private.h"

G_DEFINE_TYPE (ConsortShell, consort_shell, G_TYPE_OBJECT)

#define CONSORT_SHELL_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONSORT_SHELL_TYPE, ConsortShellPrivate))

struct _ConsortShellPrivate {
    gchar           *name; /* Reserved right now */
    
    struct desktop  *desktop;
    GSettings       *background_settings;
};


/* Expose callback for the drawing area */
static gboolean
draw_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	struct desktop *desktop = data;

	gdk_cairo_set_source_pixbuf (cr, desktop->background->pixbuf, 0, 0);
	cairo_paint (cr);

	return TRUE;
}

/* Destroy handler for the window */
static void
destroy_cb (GObject *object, gpointer data)
{
	gtk_main_quit ();
}

static const char*
get_background_picture (ConsortShellPrivate *priv)
{
    const char *uri;
    GFile *file;
    char *filename;
    
    uri = g_settings_get_string (priv->background_settings, DESKTOP_PICTURE_KEY);
    file = g_file_new_for_uri (uri);
    /* TODO: Query whether the file exists, otherwise return a default blank image */
    filename = g_file_get_path (file);
    
    g_object_unref (file);
    return filename;
}

static void
background_create(ConsortShell *shell)
{
    ConsortShellPrivate *priv = CONSORT_SHELL_GET_PRIVATE (shell);
	GdkWindow *gdk_window;
    struct desktop *desktop;
	struct element *background;
    const char *picture_uri;
    
    desktop = priv->desktop;
	background = malloc(sizeof *background);
	memset(background, 0, sizeof *background);

	picture_uri = get_background_picture (priv);
    
	background->pixbuf = gdk_pixbuf_new_from_file (picture_uri, NULL);
	if (!background->pixbuf) {
		g_message ("Could not load background: %s", picture_uri);
		exit (EXIT_FAILURE);
	}
    g_free (picture_uri);

	background->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect (background->window, "destroy",
			  G_CALLBACK (destroy_cb), NULL);

	g_signal_connect (background->window, "draw",
			  G_CALLBACK (draw_cb), desktop);

	gtk_window_set_title(GTK_WINDOW(background->window), "gtk shell");
	gtk_window_set_decorated(GTK_WINDOW(background->window), FALSE);
	gtk_widget_realize(background->window);

	gdk_window = gtk_widget_get_window(background->window);
	gdk_wayland_window_set_use_custom_surface(gdk_window);

	background->surface = gdk_wayland_window_get_wl_surface(gdk_window);
	desktop_shell_set_user_data(desktop->shell, desktop);
	desktop_shell_set_background(desktop->shell, desktop->output,
		background->surface);

	desktop->background = background;

	gtk_widget_show_all(background->window);
}

/* GObject methods */

static void consort_shell_init (ConsortShell *object) {
    ConsortShellPrivate *priv = CONSORT_SHELL_GET_PRIVATE (object);
    struct desktop *desktop;
    
    priv->background_settings = g_settings_new (DESKTOP_BACKGROUND_SCHEMA);
    
    desktop = malloc(sizeof *desktop);
    desktop->output = NULL;
    desktop->shell = NULL;

    desktop->gdk_display = gdk_display_get_default();
    desktop->display =
        gdk_wayland_display_get_wl_display(desktop->gdk_display);
    if (desktop->display == NULL) {
        fprintf(stderr, "failed to get display: %m\n");
        return -1;
    }

    desktop->registry = wl_display_get_registry(desktop->display);
    wl_registry_add_listener(desktop->registry,
            &registry_listener, desktop);

    /* Wait until we have been notified about the compositor and shell
     * objects */
    while (!desktop->output || !desktop->shell)
        wl_display_roundtrip (desktop->display);
        
    priv->desktop = desktop;
    /* Create our background image */
    background_create (object);
}

static void consort_shell_finalize (GObject *object) {
    ConsortShellPrivate *priv = CONSORT_SHELL_GET_PRIVATE (object);
    
    /* Clean up private */
    if (priv->name) {
        g_free (priv->name);
    }
    
    if (priv->desktop) {
        free (priv->desktop);
        priv->desktop = NULL;
    }
    
    g_object_unref (priv->background_settings);
    
    G_OBJECT_CLASS (consort_shell_parent_class)->finalize (object);
}

static void consort_shell_class_init (ConsortShellClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = consort_shell_finalize;

    g_type_class_add_private (object_class, sizeof (ConsortShellPrivate));

}

/**
 * Create a new ConsortShell instance
 */
ConsortShell* consort_shell_new () {
    ConsortShell *shell;

    shell = g_object_new (CONSORT_SHELL_TYPE, NULL);

    return shell;
}

int main (int argc, char *argv[]) {
    
    ConsortShell *shell;
    GOptionContext *ctx;
    
    GError *error = NULL;

    ctx = g_option_context_new (NULL);
    g_option_context_add_group (ctx, g_irepository_get_option_group ());  

    if (!g_option_context_parse (ctx, &argc, &argv, &error)) {
        g_print ("consort-shell: %s\n", error->message);
        return 1;
    }
    gtk_init(&argc, &argv);

    shell = consort_shell_new ();
    
    gtk_main ();
    return 0;
}
