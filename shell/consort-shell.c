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


/* GObject methods */

static void consort_shell_init (ConsortShell *object) {
    ConsortShellPrivate *priv = CONSORT_SHELL_GET_PRIVATE (object);
    struct desktop *desktop;
    
    desktop = priv->desktop;
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
