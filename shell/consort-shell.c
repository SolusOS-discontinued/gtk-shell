#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>

#include "consort-shell.h"
#include "consort-shell-private.h"

G_DEFINE_TYPE (ConsortShell, consort_shell, G_TYPE_OBJECT)

#define CONSORT_SHELL_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONSORT_SHELL_TYPE, ConsortShellPrivate))

struct _ConsortShellPrivate {
    gchar           *name; /* Reserved right now */
    
    struct desktop  *desktop;
        
    PeasEngine       *engine;
    PeasExtensionSet *extensions;
    
    GtkWindow            *plugin_window;
    PeasGtkPluginManager *plugin_manager;
};


static void
on_extension_added (PeasExtensionSet *set, PeasPluginInfo *info, PeasExtension *exten, ConsortShell *cs) {
    peas_activatable_activate (PEAS_ACTIVATABLE (exten));
}

static void
on_extension_removed (PeasExtensionSet *set, PeasPluginInfo *info, PeasExtension *exten, ConsortShell *cs) {
    peas_activatable_deactivate (PEAS_ACTIVATABLE (exten));
}

void consort_shell_set_panel_window (ConsortShell *shell, gpointer panel_window) {
    GdkWindow *window;
    ConsortShellPrivate *priv;
    struct element *panel;
    struct desktop *desktop;
    
    priv = CONSORT_SHELL_GET_PRIVATE (shell);
    
    desktop = priv->desktop;
    panel = malloc(sizeof *panel);
    memset(panel, 0, sizeof *panel);
    
    window = gtk_widget_get_window (GTK_WINDOW (panel_window));
    gdk_wayland_window_set_use_custom_surface (window);
    panel->surface = gdk_wayland_window_get_wl_surface (window);
    desktop_shell_set_user_data(desktop->shell, desktop);
    desktop_shell_set_panel(desktop->shell, desktop->output, panel->surface);
    }

void consort_shell_set_background_window (ConsortShell *shell, gpointer background_window) {
    GdkWindow *window;
    ConsortShellPrivate *priv;
	struct element *background;
    struct desktop *desktop;

    priv = CONSORT_SHELL_GET_PRIVATE (shell);
    
    desktop = priv->desktop;
    background = malloc(sizeof *background);
    memset(background, 0, sizeof *background);
        
    window = gtk_widget_get_window (GTK_WINDOW (background_window));
    gdk_wayland_window_set_use_custom_surface (window);
    background->surface = gdk_wayland_window_get_wl_surface (window);
    desktop_shell_set_user_data (desktop->shell, desktop);
    desktop_shell_set_background (desktop->shell, desktop->output, background->surface);
    
    desktop->background = background;        
}

static GtkWindow * plugin_window_create (ConsortShellPrivate *priv) {
    GtkWindow *window;
    
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_add (GTK_CONTAINER(window), priv->plugin_manager);
    
    return window;
}

/* GObject methods */

static void consort_shell_init (ConsortShell *object) {
    ConsortShellPrivate *priv = CONSORT_SHELL_GET_PRIVATE (object);
    struct desktop *desktop;

    /* Set up the PeasEngine */
    priv->engine = peas_engine_get_default ();
    priv->extensions = peas_extension_set_new (peas_engine_get_default (), PEAS_TYPE_ACTIVATABLE, "object", object, NULL);
    peas_engine_add_search_path (priv->engine, CONSORT_SHELL_PLUGIN_DATA_DIR, CONSORT_SHELL_PLUGIN_DIR);
    peas_engine_enable_loader (priv->engine, "python3");
    
    /* Plugin manager */
    priv->plugin_manager = peas_gtk_plugin_manager_new (priv->engine);
    priv->plugin_window = plugin_window_create (priv);
    gtk_window_set_title (priv->plugin_window, "Consort2 Plugin Manager");
    gtk_widget_show_all (priv->plugin_window);
    
    /* Preload any plugins */
    peas_extension_set_foreach (priv->extensions, (PeasExtensionSetForeachFunc) on_extension_added, object);

    g_signal_connect (priv->extensions, "extension-added", G_CALLBACK (on_extension_added), object);
    g_signal_connect (priv->extensions, "extension-removed", G_CALLBACK (on_extension_removed), object);
    
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
    
    g_object_unref (priv->engine);
    g_clear_object (&priv->extensions);
    
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
