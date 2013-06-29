#include "consort-shell.h"

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
