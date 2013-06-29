#include "consort-shell.h"

G_DEFINE_TYPE (ConsortShell, consort_shell, G_TYPE_OBJECT)

#define CONSORT_SHELL_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONSORT_SHELL_TYPE, ConsortShellPrivate))

struct _ConsortShellPrivate {
    gchar *name; /* Reserved right now */
};


static void consort_shell_init (ConsortShell *object) {
    ConsortShellPrivate *priv = CONSORT_SHELL_GET_PRIVATE (object);
    
    /* Actually init our self */
}

static void consort_shell_finalize (GObject *object) {
    ConsortShellPrivate *priv = CONSORT_SHELL_GET_PRIVATE (object);
    
    /* Clean up private */
    if (priv->name) {
        g_free (priv->name);
    }
    
    G_OBJECT_CLASS (consort_shell_parent_class)->finalize (object);
}

static void
consort_shell_class_init (ConsortShellClass *klass) {
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

int main (int argc, char **argv) {
    ConsortShell *shell;
    GOptionContext *ctx;
    
    GError *error = NULL;

    ctx = g_option_context_new (NULL);
    g_option_context_add_group (ctx, g_irepository_get_option_group ());  

    if (!g_option_context_parse (ctx, &argc, &argv, &error)) {
        g_print ("consort-shell: %s\n", error->message);
        return 1;
    }
        
    shell = consort_shell_new ();
    
    return 0;
}
