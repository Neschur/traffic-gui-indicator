#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <getopt.h>

GtkWidget *window;
gboolean visible=TRUE;
char *interface;

char *GetTraffState()
{
    char *res=(char *)malloc(sizeof(char)*100);
    memset(res,0,100);
    FILE *fr;
    char *pn=(char *)malloc(sizeof(char)*100);
    strcpy(pn,"ifconfig ");
    strcat(pn,interface);
    strcat(pn," |grep bytes");
    fr=popen(pn,"r");
    free(pn);
    char buf1[512];
    char buf2[512];
    if(fr==NULL)
        return NULL;
    fgets(buf1,sizeof(buf1),fr);
    fgets(buf2,sizeof(buf2),fr);
    pclose(fr);
    if(buf1[0]!=' ')
        return NULL;
    char *str1=(char *)malloc(sizeof(char)*10);
    char *str2=(char *)malloc(sizeof(char)*10);
    int i=0;
    for(i=0; buf1[i]!='('; i++)
        if(buf1[i]==0)
            return NULL;
    i++;
    int j=0;
    while(buf1[i]!=')'){
        if(buf1[i]==0)
            return NULL;
        str1[j]=buf1[i];
        j++;
        i++;
    }
    str1[j]=0;
    for(i=0; buf2[i]!='('; i++);
        if(buf2[i]==0)
            return NULL;
    i++;
    j=0;
    while(buf2[i]!=0){
        if(buf2[i]==0)
            return NULL;
        str2[j]=buf2[i];
        j++;
        i++;
    }
    str2[j-2]=0;
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
    char *str = GetTraffState();
    if(str==NULL){
        str=malloc(sizeof(char)*10);
        strcpy(str,"No connection");
    }
    gtk_label_set_text(GTK_LABEL(label),str);
    free(str);
    return TRUE;
}

GdkPixbuf *create_pixbuf(const gchar * filename)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if(!pixbuf){
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }
    return pixbuf;
}

void iconClick(GtkWidget *widget)
{
    if(visible){
        gtk_widget_hide_all(window);
        visible=FALSE;
    }else{
        gtk_widget_show_all(window);
        visible=TRUE;
    }
}

void gui( int argc, char *argv[])
{
    GtkWidget *frame;
    GtkWidget *label;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "traffic");
    gtk_window_set_default_size(GTK_WINDOW(window), 100, 20);
    gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("/usr/share/icons/hicolor/64x64/apps/traffic-gui-indicator.png"));
    frame = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), frame);
    label=gtk_label_new(NULL);
    gtk_fixed_put(GTK_FIXED(frame), label, 0,0);
    refTraffState(label);
    if(visible)
        gtk_widget_show_all(window);
    g_timeout_add(1000,refTraffState,label);
    g_signal_connect_swapped(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), window);
    g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkStatusIcon *icon=gtk_status_icon_new_from_file("/usr/share/icons/hicolor/64x64/apps/traffic-gui-indicator.png");
    gtk_status_icon_set_visible(icon,TRUE);
    g_signal_connect(GTK_STATUS_ICON (icon), "activate", GTK_SIGNAL_FUNC (iconClick), window);
    gtk_main();
}

const struct option long_options[] = {
		{"version",no_argument,NULL,'v'},
		{"interface",required_argument,NULL,'i'},
		{"help",no_argument,NULL,'h'},
		{"hidewin",no_argument,NULL,'w'},
		{NULL,0,NULL,0}
	};

int main(int argc, char *argv[])
{
    interface=(char *)malloc(sizeof(char)*16);
    strcpy(interface,"eth0");
    char c;
    int option_index;
    while ((c = getopt_long (argc, argv, "wi", long_options, &option_index))!=-1){
        switch(c){
        case 'h':
            printf("simple gui traffic indicator\n"
                    "--help - Print this help\n"
                    "--version - Print version\n"
                    "--hidewin(-w) - start with hided window\n"
                    "--interface(-i) - interface(eth0, wlan0, ppp0, etc)\n"
                );
            return 0;
        case 'v':
            printf("simple gui traffic indicator 1.1.0\n");
            return 0;
        case 'i':
            strcpy(interface,optarg);
            break;
        case 'w':
            visible=FALSE   ;
            break;
        default:
            fprintf (stderr, ("Try '%s --help' for more information.\n"), "fb2rn");
            return 0;
        }
    }
    gui(argc,argv);
    return 0;
}
