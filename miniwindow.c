#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h> // Para usleep


int main(int argc, char *argv[]) {
    Display *display;
    Window window;
    GC gc;
    XEvent event;
    unsigned int width = 500, height = 250; // Tamaño de la ventana principal
    int d = 40; // Diámetro para las esquinas redondeadas
    int border = 10; // Grosor del borde interior
    char *message = "Esto funciona!";
    unsigned long foreground = 0x000000; // Color del texto
    unsigned long background = 0xffffff; // Color de fondo de la ventana
    unsigned long innerBackground = 0xdddddd; // Color de fondo del rectángulo interior


    // Procesar argumentos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            message = argv[++i];
        } else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
            width = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            height = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-fc") == 0 && i + 1 < argc) {
            foreground = strtol(argv[++i], NULL, 16);
        } else if (strcmp(argv[i], "-bc") == 0 && i + 1 < argc) {
            background = strtol(argv[++i], NULL, 16);
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            innerBackground = strtol(argv[++i], NULL, 16);
    	}
    }

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "No se puede abrir el display.\n");
        exit(1);
    }
    int screenWidth, screenHeight;
    Screen* screenX = DefaultScreenOfDisplay(display);
    screenWidth = WidthOfScreen(screenX);


    int screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), screenWidth, 0, width, height, 1,
                                 BlackPixel(display, screen), background);

    Atom windowType = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom windowTypeDialog = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    XChangeProperty(display, window, windowType, XA_ATOM, 32, PropModeReplace, (unsigned char *)&windowTypeDialog, 1);

   XMapWindow(display, window);
    XSelectInput(display, window, ExposureMask | StructureNotifyMask);
    gc = XCreateGC(display, window, 0, NULL);

    time_t startTime = time(NULL);
    int running = 1;

    while (running) {
        while (XPending(display) > 0) {
            XNextEvent(display, &event);
            if (event.type == Expose) {
                // Dibuja el rectángulo interior con bordes redondeados
                XSetForeground(display, gc, innerBackground);
                XFillRectangle(display, window, gc, border, border, width - 2*border, height - 2*border);
		border = 11;
                XSetForeground(display, gc, background);
                XFillRectangle(display, window, gc, border, border, width - 2*border, height - 2*border);

                // Dibuja el mensaje
                XSetForeground(display, gc, foreground);
                int textWidth = XTextWidth(XQueryFont(display, XGContextFromGC(gc)), message, strlen(message));
                XDrawString(display, window, gc, (width - textWidth) / 2, height / 2, message, strlen(message));
            }
        }

        if (difftime(time(NULL), startTime) > 3) {
            running = 0;
        }
    }

    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}

