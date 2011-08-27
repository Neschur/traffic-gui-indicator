#include "stdlib.h"
#include <gtk/gtk.h>
#include "string.h"

GtkWidget *window;
gboolean visibl=FALSE;

char *GetTraffState()
{
    char *res=(char *)malloc(sizeof(char)*100);
    memset(res,0,100);
    FILE *fr;
    fr=popen("ifconfig ppp0 |grep bytes","r");
    if(fr==NULL){strcpy(res,"Error");return NULL;}
    char tmp[512];
    fgets(tmp,sizeof(tmp),fr);
    printf(tmp);
    pclose(fr);
    if(tmp[0]!=' '){strcpy(res,"No connetction");return res;}

    char *str1=(char *)malloc(sizeof(char)*10);
    char *str2=(char *)malloc(sizeof(char)*10);
    memset(str1,0,10);memset(str2,0,10);
    int i=0;
    for(i=0;tmp[i]!='(';i++);

    i++;
    int j=0;
    while(tmp[i]!=')'){
        str1[j]=tmp[i];
        j++;i++;
    }
    int y=0;
    for(y=i+1;tmp[y]!='(';y++);
    y++;
    j=0;
    while(tmp[y]!=')'){
        str2[j]=tmp[y];
        j++;y++;
    }

    strcpy(res,"In:");
    strcat(res,str1);
    strcat(res," ");
    strcat(res,"Out:");
    strcat(res,str2);
    free(str1);
    free(str2);
    return res;
}

gboolean refTraffState(gpointer data)
{
    GtkWidget *label = GTK_WIDGET(data);
    static int counter=1;
    char *str = GetTraffState();
    if(str==NULL){
        str=malloc(sizeof(char)*10);
        strcpy(str,"No connection");
    }
    gtk_label_set_text(GTK_LABEL(label),str);
    free(str);
    counter++;
    return TRUE;
}

GdkPixbuf *create_pixbuf(const gchar * filename)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if(!pixbuf) {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }
    return pixbuf;
}

void iconClick(GtkWidget *widget)
{
    if(visibl){
        gtk_widget_hide(window);
        visibl=FALSE;
    }else{
        gtk_widget_show(window);
        visibl=TRUE;
    }
}

void gui( int argc, char *argv[])
{
    GtkWidget *frame;
    GtkWidget* label;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "traffic");
    gtk_window_set_default_size(GTK_WINDOW(window), 100, 20);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("/usr/share/icons/hicolor/64x64/apps/traffic-gui-indicator.png"));

    frame = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), frame);

    label=gtk_label_new(NULL);
    gtk_fixed_put(GTK_FIXED(frame), label, 0,0);

    refTraffState(label);

    gtk_widget_show_all(window);
    if(visibl){
        gtk_widget_hide(window);
        }

    g_timeout_add(1000,refTraffState,label);

    g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkStatusIcon *icon=gtk_status_icon_new_from_file("/usr/share/icons/hicolor/64x64/apps/traffic-gui-indicator.png");
    gtk_status_icon_set_visible(icon,TRUE);
    g_signal_connect(GTK_STATUS_ICON (icon), "activate", GTK_SIGNAL_FUNC (iconClick), window);
    gtk_main();
}

int main( int argc, char *argv[])
{
    gboolean o=FALSE;
    if(argc>1)
    {
        if((strcmp(argv[1],"--auto")==0)||(strcmp(argv[1],"-a")==0)){
            visibl=TRUE;
            o=TRUE;
        }
        if((strcmp(argv[1],"--version")==0)||(strcmp(argv[1],"-v")==0)){
            printf("traffic gui indicator 1.0.0\n");
            return 1;
        }
        if((strcmp(argv[1],"--help")==0)||(strcmp(argv[1],"-h")==0)){
            printf("simple gui traffic indicator\n"
                    "--version(-v) - Print version\n"
                    "--help(-h) - Print this help\n"
                    "--auto(-a) - for autorun\n"
                   );
            return 1;
        }
        if(!o){
            printf("unknown\n");
            return -1;
        }
    }
    gui(argc,argv);
    return 0;
}
