#include "debug_ui.h"
#include <string.h>

static  char logbuf[64000];
static   int logbuf_updated = 0;

static void write_log(const char *text) {
  if (logbuf[0]) { strcat(logbuf, "\n"); }
  strcat(logbuf, text);
  logbuf_updated = 1;
}

void test_window(mu_Context* context) {
    if (mu_begin_window(context, "Log Window", mu_rect(350, 40, 300, 200))) {
        /* output text panel */
        mu_layout_row(context, 1, (int[]) { -1 }, -25);
        mu_begin_panel(context, "Log Output");
        mu_Container *panel = mu_get_current_container(context);
        mu_layout_row(context, 1, (int[]) { -1 }, -1);
        mu_text(context, logbuf);
        mu_end_panel(context);
        if (logbuf_updated) {
            panel->scroll.y = panel->content_size.y;
            logbuf_updated = 0;
        }

        /* input textbox + submit button */
        static char buf[128];
        int submitted = 0;
        mu_layout_row(context, 2, (int[]) { -70, -1 }, 0);
        if (mu_textbox(context, buf, sizeof(buf)) & MU_RES_SUBMIT) {
            mu_set_focus(context, context->last_id);
            submitted = 1;
        }
        if (mu_button(context, "Submit")) { submitted = 1; }
        if (submitted) {
            write_log(buf);
            buf[0] = '\0';
        }

        mu_end_window(context);
    }
}

void debug_ui_process_frame(mu_Context* context) {
    mu_begin(context);
    // test_window(context);
    mu_end(context);
}
