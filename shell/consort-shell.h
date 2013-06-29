#ifndef __CONSORT_SHELL_H__
#define __CONSORT_SHELL_H__

/**
 * TODO: Add GPL header
 */

#include <glib.h>
#include <glib-object.h>
#include <girepository.h>
#include <stdlib.h>
#include <libpeas/peas.h>
#include <libpeas-gtk/peas-gtk.h>

#define CONSORT_SHELL_TYPE              (consort_shell_get_type())
#define CONSORT_SHELL(o)                (G_TYPE_CHECK_INSTANCE_CAST ((o), CONSORT_SHELL_TYPE, ConsortShell))
#define CONSORT_SHELL_CLASS             (G_TYPE_CHECK_CLASS_CAST ((c), CONSORT_SHELL_TYPE, ConsortShellClass))
#define IS_CONSORT_SHELL(o)             (G_TYPE_CHECK_INSTANCE_TYPE ((o), CONSORT_SHELL_TYPE))
#define IS_CONSORT_SHELL_CLASS(o)       (G_TYPE_CHECK_CLASS_TYPE ((c),  CONSORT_SHELL_TYPE))
#define CONSORT_SHELL_GET_CLASS(o)      (G_TYPE_INSTANCE_GET_CLASS ((o), CONSORT_SHELL_TYPE, ConsortShellClass))

typedef struct _ConsortShell        ConsortShell;

typedef struct _ConsortShellPrivate ConsortShellPrivate;

typedef struct _ConsortShellClass   ConsortShellClass;

struct _ConsortShell {

    GObject parent;

};

struct _ConsortShellClass {

    GObjectClass parent;

};

GType           consort_shell_get_type	() G_GNUC_CONST;

ConsortShell*   consort_shell_new       (void);

void     consort_shell_set_background_window (ConsortShell *shell, gpointer background_window);

#endif /* __CONSORT_SHELL_H__ */
