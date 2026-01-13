#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <stdint.h>

int main(void)
{

        xcb_connection_t *conn;
        xcb_screen_t *sc;
        xcb_generic_event_t *ev;
        uint32_t mask;
        uint32_t vals[1];
        
        // Connection to X Server
        conn = xcb_connect(NULL, NULL);
        if (xcb_connection_has_error(conn)) {
                perror("ERR: Cloud not connect to the X Server");
                return 1;
        }
        

        // Fetch the first screen
        sc = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;

        // Substructure redirect
        mask = XCB_CW_EVENT_MASK;
        vals[0] = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;

        xcb_void_cookie_t cookie = xcb_change_window_attributes_checked(conn, sc->root, mask, vals);
        xcb_generic_error_t *error = xcb_request_check(conn, cookie);

        if (error) {
                perror("Register WM failed (maybe other WM is running).");
                free(error);
                xcb_disconnect(conn);
                return 1;
        }

        xcb_flush(conn);

        while (ev = xcb_wait_for_event(conn)) {
                switch (ev->response_type & ~0x80) {
                case XCB_MAP_REQUEST: { 
                        xcb_map_request_event_t *me = (xcb_map_request_event_t *)ev;
                        printf("FET [map_request], ID: %u\n", me->window);

                        xcb_map_window(conn, me->window);

                        uint32_t border_vals[] = {2};
                        xcb_configure_window(conn, me->window, XCB_CONFIG_WINDOW_BORDER_WIDTH, border_vals);

                        xcb_flush(conn);
                        break;
                        }
                }

                free(ev);
        }
        xcb_disconnect(conn);

        return 0;
}
