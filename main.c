/* BASIC LIBRARY */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "jrb.h"
#include "dllist.h"

/* GRAPHIC LIBRARY */
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define INFINITIVE_VALUE 100000

/* FRONT-END */
GtkWidget *window, *search_window, *lookup_window;
GtkWidget *from, *end, *info;
GtkWidget *search_btn, *lookup_btn;

void search_interface(GtkWidget widget, gpointer window);
void lookup_interface(GtkWidget widget, gpointer window);
void search_func(GtkWidget *widget, gpointer search_window);
void lookup_func(GtkWidget *widget, gpointer search_window);

void back_func(GtkWidget *widget, gpointer window);
void exit_func(GtkWidget *widget, gpointer window);
void close_func(GtkWidget *widget, gpointer window);

void myCSS();

/* BACK-END */
int id = 1;
int output[10000];
typedef struct{
  JRB edges;
  JRB vertices;
}Graph; 
typedef struct {
  int count;    // save count of bus
  Dllist list_of_bus;  // save all of bus go though vertex
}Vertex;
Vertex dinh[1500];
int i, length, path[100], s, t;
char s1[1000], s2[1000], place[1000];
Dllist ptr;
Graph g;
FILE *fr;

Graph creatGraph();
void addVertex(Graph g,char *name); 
int getVertex(Graph g, const char *name); /*Lay ra ID cua ten ben vd ben gia lam id 1; bach khoa id :2
 Tien cho viec dung ham short*/
char *getVertexbyId(Graph g,int id); // Lay ra ten dinh neu biet ID
void addEdge(Graph graph, char *v1,char *v2);
int getEdgeValue(Graph graph,char *v1, char *v2); // return INFINITIVE_VALUE if no edge between v1 and v2
int outdegree(Graph graph, char *v, int* output);
int indegree(Graph g, char *v, int *output);
int shortestPath(Graph graph, int s, int t, int* path,int*length);
void dropGraph(Graph graph);
void xulychuoi(char *a,Graph g,char *s);
void readFile(FILE *fr,Graph g);
int checkStation(Graph g, const char *name); //Kiem tra dia diem co ton tai
char *print_route(Vertex a,Vertex b);// In cac tuyen di qua 1 diem
char *get_info(char *place);
char *xauChuan(Graph g, const char *s); // Tra ve xau dc chuan hoa
char *khoangTrang(const char *s); // Tra ve xau sau khi loai bo space thua,tab

/* ===== DRIVE CODE ===== */
void main(int argc, char *argv[]){
  GtkWidget *fixed, *image;
  GtkWidget *btn1, *btn2, *btn3;

  for (i=1; i<1500; i++)
    dinh[i].list_of_bus= new_dllist();

  g = creatGraph();

  fr = fopen("data.txt","r");
  readFile(fr,g);
  
  gtk_init(&argc, &argv);

  myCSS();

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Hanoi bus route");
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

  fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(window), fixed);

  image = gtk_image_new_from_file("bus.jpg");
  gtk_container_add(GTK_CONTAINER(fixed), image);

  btn1 = gtk_button_new_with_label("Search");
  gtk_widget_set_name(btn1, "search_Btn");
  gtk_fixed_put(GTK_FIXED(fixed), btn1, 100, 80);
  gtk_widget_set_size_request(btn1, 200, 50);
  gtk_widget_set_tooltip_text(btn1, "Find the shortest route");
  g_signal_connect(G_OBJECT(btn1), "clicked", G_CALLBACK(search_interface), window);

  btn2 = gtk_button_new_with_label("Look up");
  gtk_widget_set_name(btn2, "lookup_Btn");
  gtk_fixed_put(GTK_FIXED(fixed), btn2, 100, 150);
  gtk_widget_set_size_request(btn2, 200, 50);
  g_signal_connect(G_OBJECT(btn2), "clicked", G_CALLBACK(lookup_interface), window);
  gtk_widget_set_tooltip_text(btn2, "See bus station information");

  btn3 = gtk_button_new_with_label("Exit");
  gtk_widget_set_name(btn3, "exit_Btn");
  gtk_fixed_put(GTK_FIXED(fixed), btn3, 100, 220);
  gtk_widget_set_size_request(btn3, 200, 50);
  gtk_widget_set_tooltip_text(btn3, "Exit app");
  g_signal_connect(G_OBJECT(btn3), "clicked", G_CALLBACK(exit_func), window);

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_widget_show_all(window);
  
  gtk_main();

  fclose(fr); 
  dropGraph(g);

  return;
}

void search_interface(GtkWidget widget, gpointer window){
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *back_btn;

  // tao cua so
  search_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(search_window), "Find bus route");
  gtk_window_set_default_size(GTK_WINDOW(search_window), 400, 200);
  gtk_window_set_position(GTK_WINDOW(search_window), GTK_WIN_POS_CENTER);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();

  /* Pack the container in the window */
  gtk_container_add (GTK_CONTAINER (search_window), grid);
  
  // space
  label = gtk_label_new(" ");
  gtk_grid_attach (GTK_GRID (grid), label, 1, 1, 1, 1);

  label = gtk_label_new("\tStarting point: ");
  gtk_grid_attach (GTK_GRID (grid), label, 1, 2, 1, 1);

  from = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(from), "From...");
  gtk_grid_attach (GTK_GRID (grid), from, 2, 2, 4, 1);

  // space
  label = gtk_label_new(" ");
  gtk_grid_attach (GTK_GRID (grid), label, 1, 3, 1, 1);

  label = gtk_label_new("\tEnd point: ");
  gtk_grid_attach (GTK_GRID (grid), label, 1, 4, 1, 1);

  end = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(end), "To...");
  gtk_grid_attach (GTK_GRID (grid), end, 2, 4, 4, 1);

  // space
  label = gtk_label_new(" ");
  gtk_grid_attach (GTK_GRID (grid), label, 1, 5, 1, 1);

  search_btn = gtk_button_new_with_label("Search");
  gtk_grid_attach (GTK_GRID (grid), search_btn, 3, 6, 1, 1);

  // space
  label = gtk_label_new(" ");
  gtk_grid_attach (GTK_GRID (grid), label, 1, 7, 1, 1);

  // space
  label = gtk_label_new(" ");
  gtk_grid_attach (GTK_GRID (grid), label, 1, 8, 1, 1);

  // space
  label = gtk_label_new("\t");
  gtk_grid_attach (GTK_GRID (grid), label, 8, 8, 1, 1);

  back_btn = gtk_button_new_with_label("Back");
  gtk_grid_attach (GTK_GRID (grid), back_btn, 9, 8, 1, 1);

  g_signal_connect(G_OBJECT(back_btn), "clicked", G_CALLBACK(back_func), search_window);
  
  g_signal_connect(G_OBJECT(search_btn), "clicked", G_CALLBACK(search_func), search_window);

  g_signal_connect(G_OBJECT(end), "activate", G_CALLBACK(search_func), search_window);
  
  g_signal_connect(G_OBJECT(search_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_widget_show_all(search_window);

  gtk_main();

  return;
  
}

void lookup_interface(GtkWidget widget, gpointer window){
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *back_btn;

  // tao cua so
  lookup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(lookup_window), "Look up information");
  gtk_window_set_default_size(GTK_WINDOW(lookup_window), 500, 100);
  gtk_window_set_position(GTK_WINDOW(lookup_window), GTK_WIN_POS_CENTER);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();

  /* Pack the container in the window */
  gtk_container_add (GTK_CONTAINER (lookup_window), grid);
  
  // space
  label = gtk_label_new("\n");
  gtk_grid_attach (GTK_GRID (grid), label, 1, 1, 1, 1);

  label = gtk_label_new("\tBus station: ");
  gtk_grid_attach (GTK_GRID (grid), label, 1, 2, 1, 1);

  info = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(info), "Enter location...");
  gtk_grid_attach (GTK_GRID (grid), info, 2, 2, 4, 1);

  // space
  label = gtk_label_new("  ");
  gtk_grid_attach (GTK_GRID (grid), label, 7, 2, 1, 1);
  
  lookup_btn = gtk_button_new_with_label("Check it");
  gtk_grid_attach (GTK_GRID (grid), lookup_btn, 8, 2, 2, 1);

  // space
  label = gtk_label_new("  ");
  gtk_grid_attach (GTK_GRID (grid), label, 10, 2, 1, 1);

  back_btn = gtk_button_new_with_label("Back");
  gtk_grid_attach (GTK_GRID (grid), back_btn, 11, 2, 2, 1);

  g_signal_connect(G_OBJECT(back_btn), "clicked", G_CALLBACK(back_func), lookup_window);
  
  g_signal_connect(G_OBJECT(lookup_btn), "clicked", G_CALLBACK(lookup_func), lookup_window);

  g_signal_connect(G_OBJECT(info), "activate", G_CALLBACK(lookup_func), lookup_window);
  
  g_signal_connect(G_OBJECT(lookup_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_widget_show_all(lookup_window);

  gtk_main();

  return;
}

void search_func(GtkWidget *widget, gpointer search_window){
  GtkWidget *result, *label;
  const gchar *x1, *x2;
  x1 = gtk_entry_get_text(GTK_ENTRY(from));
  x2 = gtk_entry_get_text(GTK_ENTRY(end));

  char *s1 = strdup(khoangTrang(x1));
  char *s2 = strdup(khoangTrang(x2));

  if(s1[0] == '\0' || s2[0] == '\0'){
    result = gtk_message_dialog_new(GTK_WINDOW(search_window),
  				  GTK_DIALOG_DESTROY_WITH_PARENT,
  				  GTK_MESSAGE_ERROR,
  				  GTK_BUTTONS_OK,
  				  "Not found!");
    gtk_window_set_title(GTK_WINDOW(result), "Fail");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(result),
					     "Please fill in the search bar.");
    gtk_dialog_run(GTK_DIALOG(result));
    gtk_widget_destroy(result);
    return;
  }

  if(checkStation(g,s1)==0){
    result = gtk_message_dialog_new(GTK_WINDOW(search_window),
  				  GTK_DIALOG_DESTROY_WITH_PARENT,
  				  GTK_MESSAGE_ERROR,
  				  GTK_BUTTONS_OK,
  				  "Bus station not exist!");
    gtk_window_set_title(GTK_WINDOW(result), "Fail");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(result),
					     "There're no bus station at \"%s\".", s1);
    gtk_dialog_run(GTK_DIALOG(result));
    gtk_widget_destroy(result);
    return;
  }

  if(checkStation(g,s2)==0){
    result = gtk_message_dialog_new(GTK_WINDOW(search_window),
  				  GTK_DIALOG_DESTROY_WITH_PARENT,
  				  GTK_MESSAGE_ERROR,
  				  GTK_BUTTONS_OK,
  				  "Bus station not exist!");
    gtk_window_set_title(GTK_WINDOW(result), "Fail");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(result),
					     "There're no bus station at \"%s\".", s2);
    gtk_dialog_run(GTK_DIALOG(result));
    gtk_widget_destroy(result);
    return;
  }

  strcpy(s1,xauChuan(g,s1));
  strcpy(s2,xauChuan(g,s2));

  int v1 = getVertex(g,s1);
  int v2 = getVertex(g,s2);
  int w = shortestPath(g, v1, v2, path, &length);
  if (w == INFINITIVE_VALUE){
    result = gtk_message_dialog_new(GTK_WINDOW(search_window),
  				  GTK_DIALOG_DESTROY_WITH_PARENT,
  				  GTK_MESSAGE_ERROR,
  				  GTK_BUTTONS_OK,
  				  "Bus route not exist!");
    gtk_window_set_title(GTK_WINDOW(result), "Fail");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(result),
					     "There're no bus route from %s to %s", s1, s2);
    gtk_dialog_run(GTK_DIALOG(result));
    gtk_widget_destroy(result);
    return;
  }
  else{
    result = gtk_assistant_new();
    gtk_window_set_default_size(GTK_WINDOW(result) , 400, 300);
    gtk_window_set_position(GTK_WINDOW(result), GTK_WIN_POS_CENTER);

    gchar *content_;
    content_ = g_strdup_printf("Path from \"%s\" to \"%s\"\n\nTotal distance: %d km\nTime expected: %d minutes", s1, s2, w, 2*w);
    label = gtk_label_new(content_);
    gtk_widget_show(label);
    gtk_assistant_append_page(GTK_ASSISTANT(result), label);
    gtk_assistant_set_page_type(GTK_ASSISTANT(result), label, GTK_ASSISTANT_PAGE_INTRO);
    gtk_assistant_set_page_complete(GTK_ASSISTANT(result), label, TRUE);
    gtk_assistant_set_page_title(GTK_ASSISTANT(result), label, " Overview: ");

    for (i = 1; i < length; i++){
      gchar *title, *content;
      
      title = g_strdup_printf(" Step %d: ", i);

      content = g_strdup_printf("\"%s\" => \"%s\"\n\n%s\nTime expected: 2 minutes",
				getVertexbyId(g, path[i-1]),
				getVertexbyId(g, path[i]),
				print_route(dinh[path[i-1]], dinh[path[i]]));

      label = gtk_label_new(content);
      gtk_widget_show(label);
      gtk_assistant_append_page(GTK_ASSISTANT(result), label);

      if(i == length-1)
	gtk_assistant_set_page_type(GTK_ASSISTANT(result), label, GTK_ASSISTANT_PAGE_SUMMARY);

      else
	gtk_assistant_set_page_type(GTK_ASSISTANT(result), label, GTK_ASSISTANT_PAGE_CONTENT);

      gtk_assistant_set_page_complete(GTK_ASSISTANT(result), label, TRUE);
      gtk_assistant_set_page_title(GTK_ASSISTANT(result), label, title);

      g_free(title);
      g_free(content);
    }

    g_signal_connect(G_OBJECT(result), "close", G_CALLBACK(close_func), result);
    g_signal_connect(G_OBJECT(result), "cancel", G_CALLBACK(close_func), result);
    gtk_widget_show_all(result);
    gtk_main();

    return;
  }
}

void lookup_func(GtkWidget *widget, gpointer lookup_window){
  GtkWidget *result, *label;
  const gchar *a;
  a = gtk_entry_get_text(GTK_ENTRY(info));

  char *x = strdup(a);
  strcpy(x, khoangTrang(x));

  if(x[0] == '\0'){
    result = gtk_message_dialog_new(GTK_WINDOW(lookup_window),
  				  GTK_DIALOG_DESTROY_WITH_PARENT,
  				  GTK_MESSAGE_ERROR,
  				  GTK_BUTTONS_OK,
  				  "Not found!");
    gtk_window_set_title(GTK_WINDOW(result), "Fail");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(result),
					     "Please fill in the search bar.");
    gtk_dialog_run(GTK_DIALOG(result));
    gtk_widget_destroy(result);
    return;
  }

  if(checkStation(g,x)==0){
    result = gtk_message_dialog_new(GTK_WINDOW(lookup_window),
  				  GTK_DIALOG_DESTROY_WITH_PARENT,
  				  GTK_MESSAGE_ERROR,
  				  GTK_BUTTONS_OK,
  				  "Bus station not exist!");
    gtk_window_set_title(GTK_WINDOW(result), "Fail");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(result),
					     "There're no bus station at \"%s\".", x);
    //gtk_window_set_transient_for(GTK_WINDOW(result), lookup_window);
    gtk_dialog_run(GTK_DIALOG(result));
    gtk_widget_destroy(result);
    free(x);
    return;
  }

  else{
    strcpy(x, xauChuan(g, x));
    gchar *xx = g_strdup(get_info(x));
    result = gtk_message_dialog_new(GTK_WINDOW(lookup_window),
  				  GTK_DIALOG_DESTROY_WITH_PARENT,
  				  GTK_MESSAGE_INFO,
  				  GTK_BUTTONS_OK,
				    "About \"%s\" bus station", x);
    gtk_window_set_title(GTK_WINDOW(result), "Found");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(result),
					     "%s", xx);
    //gtk_window_set_transient_for(GTK_WINDOW(result), lookup_window);
    gtk_dialog_run(GTK_DIALOG(result));
    gtk_widget_destroy(result);
    free(x);
    return;
  }
}


void exit_func(GtkWidget *widget, gpointer window) {
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
				  GTK_DIALOG_DESTROY_WITH_PARENT,
				  GTK_MESSAGE_QUESTION,
				  GTK_BUTTONS_YES_NO,
				  "Are you sure to quit?");
  gtk_window_set_title(GTK_WINDOW(dialog), "Quit");
  
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
    gtk_widget_destroy(GTK_WIDGET(window));
  else gtk_widget_destroy(dialog);
  return;
}

void back_func(GtkWidget *widget, gpointer window)  {
    gtk_widget_destroy(GTK_WIDGET(window));
    return;
}

void close_func(GtkWidget *widget, gpointer window)  {
    gtk_widget_destroy(window);
    return;
}

int checkStation(Graph g, const char *name){
  char station[50];
  if(xauChuan(g,name) == NULL) 
    return 0;
  return 1;// exist
}

char *print_route(Vertex a,Vertex b){
  int count = 1;
  Dllist ptr;
  Dllist ptr1;
  char *result = (char*)malloc(1000*sizeof(char));
  strcpy(result, "Tuyen: ");
  dll_val(a.list_of_bus);
  if(a.count <= b.count){
    dll_traverse(ptr,a.list_of_bus){
      dll_traverse(ptr1,b.list_of_bus){
        if(strcmp(jval_s(ptr->val),jval_s(ptr1->val))==0) {
	  strcat(result, jval_s(ptr->val));
	  strcat(result, " ");
	  if((count % 5) == 0)
	    strcat(result, "\n\t\t");
	  count++;
	}
      }
    }
  }

  else {
    dll_traverse(ptr,b.list_of_bus){
      dll_traverse(ptr1,a.list_of_bus){
        if(strcmp(jval_s(ptr->val),jval_s(ptr1->val))==0) {
	  strcat(result, jval_s(ptr->val));
	  strcat(result, " ");
	  if((count % 5) == 0)
	    strcat(result, "\n\t\t");
	  count++;
	}
      }
    }
  }
  strcat(result, "\0");
  return result;
}

char *get_info(char *place){
  int count = 1;
  char *result = (char*)malloc(10000*sizeof(char));
  strcpy(result, "Cac tuyen di qua \"");
  strcat(result, place);
  strcat(result, "\":\n");
  dll_traverse(ptr,dinh[getVertex(g,place)].list_of_bus){
    strcat(result, jval_s(ptr->val));
    strcat(result, " ");
    if((count % 5) == 0)
      strcat(result, "\n");
    count++;
  }
        
  strcat(result, "\n\n");

  JRB node;
  strcat(result, "Cac diem lien ket voi \"");
  strcat(result, place);
  strcat(result, "\":\n");
  
  jrb_traverse(node,g.vertices){
    if(getEdgeValue(g,place,jval_s(node->key)) == 1){
      strcat(result, " => ");
      strcat(result, jval_s(node->key));
      strcat(result, "\n");
    }
  }
  strcat(result, "\0");
  /* g_print("%ld\n", strlen(result)); */
  return result;
}

void xulychuoi(char *a,Graph g,char *s){
  Dllist ptr;  
  int j;
  int index = 0;
  char **b = (char **)malloc(10000*sizeof(char));
  char *p;
  p = strtok(a, "-"); //cat chuoi bang cac ky tu ,. va space
  
  while(p != NULL){
    b[index] = p;
    index++;
    p = strtok(NULL, "-"); //cat chuoi tu vi tri dung lai truoc do
  }
  
  for(j = 0; j < index; j++){
    while(b[j][0] == ' ') strcpy(&(b[j][0]),&(b[j][1])); // Loai bo ca ky tu trong o dau  
    if(j != index  - 1)
      b[j][strlen(b[j]) - 1] = '\0';
    
    addVertex(g,b[j]);
    int flag=0;
    if(dll_empty(dinh[getVertex(g,b[j])].list_of_bus)) {
      dll_append( dinh[getVertex(g,b[j])].list_of_bus,new_jval_s(strdup(s)));
      dinh[getVertex(g,b[j])].count++;
    }
    dll_traverse(ptr,dinh[getVertex(g,b[j])].list_of_bus){
      char *tmp= (char*)malloc(sizeof(char)*1000) ;
      strcpy(tmp,jval_s(ptr->val));
      if(strcmp(tmp,s)==0) flag++;
    }
    if(flag==0){
      dll_append( dinh[getVertex(g,b[j])].list_of_bus,new_jval_s(strdup(s)));
      dinh[getVertex(g,b[j])].count++;
    }   
  }
  for(int i = index - 1; i - 1 >=0; i--)
    addEdge(g,b[i-1],b[i]);       
}

void readFile(FILE *fr,Graph g){
  while( !feof(fr)){
    char s[20],*a,*b;
    a = (char *)malloc(10000*sizeof(char));
    b = (char *)malloc(10000*sizeof(char));
    fscanf(fr,"%s",s);
    fscanf(fr,"\n");
    fscanf(fr,"\t");
    fgets(a,10000,fr);
    a[strlen(a) - 1] = '\0';
    xulychuoi(a,g,s);
    fscanf(fr,"\t");
    fgets(b,10000,fr);
    b[strlen(b) - 1] = '\0';
    xulychuoi(b,g,s);
    fscanf(fr,"\n");
  }
}

Graph creatGraph(){
  Graph g;
  g.edges = make_jrb();
  g.vertices = make_jrb();
  return g;
}

void addVertex(Graph g,char *name){
  JRB node = jrb_find_str(g.vertices,strdup(name));
  if(node == NULL){
    jrb_insert_str(g.vertices,strdup(name),new_jval_i(id));
    id = id + 1;
  }
}

int getVertex(Graph g, const char *name ){
  JRB node;
  node = jrb_find_str(g.vertices,strdup(name));
  if(node == NULL) return 0;
  else
    return jval_i(node->val);
}

char *getVertexbyId(Graph g,int id){
  JRB node;
  jrb_traverse(node,g.vertices){
    if(jval_i(node->val) == id)
      return jval_s(node->key);
  }
}

void addEdge(Graph g, char *v1, char *v2){ 
  JRB node,tree;
  if(getEdgeValue(g,v1,v2) == INFINITIVE_VALUE){
    node = jrb_find_str(g.edges,strdup(v1));
    if(node == NULL){
      tree = make_jrb();
      jrb_insert_str(g.edges,strdup(v1), new_jval_v(tree));
    }
    else
      tree = (JRB) jval_v(node->val);
    jrb_insert_str(tree,strdup(v2),new_jval_i(1));
  }
}

int getEdgeValue(Graph g, char *v1, char *v2){ 
  JRB node,tree;
  node = jrb_find_str(g.edges,strdup(v1));
  if(node == NULL)
    return INFINITIVE_VALUE;
  tree = (JRB) jval_v(node->val);
  node = jrb_find_str(tree,strdup(v2));
  if(node == NULL)
    return INFINITIVE_VALUE;
  else
    return jval_i(node->val);
}

int outdegree(Graph g,char *v, int *output){
  JRB node,tree;
  int total = 0; 
  node = jrb_find_str(g.edges,strdup(v));
  if(node == NULL)
    return 0;
  tree = (JRB) jval_v(node->val);
  jrb_traverse(node,tree){       
    output[total] = getVertex(g,jval_s(node->key)); // Lay ra ID
    total++;
  } 
  return total;
}

int indegree(Graph g, char *v, int *output){
  JRB node,tree;
  int total = 0;
  jrb_traverse(node,g.edges){
    tree = (JRB) jval_v(node->val);
    if(jrb_find_str(tree,strdup(v))){
      output[total] = getVertex(g,jval_s(node->key));
      total++;
    }
  }
  return total;
}

int shortestPath(Graph g, int s, int t, int* path, int*length){   
  int distance[10000]; // Luu khoang cach tu s den dinh dang xet
  int min;
  int previous[10000], tmp[10000]; // previos lu dinh o trc no
  int visit[10000];
  int output[10000], i, u;
  Dllist ptr, queue, node;
  
  for (i=0; i < 10000; i++){
    distance[i] = INFINITIVE_VALUE;
    visit[i] = 0;
  }
  
  distance[s] = 0;
  previous[s] = s;
  visit[s] = 1;
  
  queue = new_dllist();
  dll_append(queue, new_jval_i(s));
  
  while ( !dll_empty(queue) ){
    // get u from the priority queue   
    min = INFINITIVE_VALUE;	  
    dll_traverse(ptr, queue){ //Duyet queue
      u = jval_i(ptr->val);              
      if (min > distance[u]){
	min = distance[u];
	node = ptr; //node de luu phan tu nho nhat
      }                
    }
    u = jval_i(node->val);
    visit[u] = 1;
    dll_delete_node(node);
    
    if (u == t)
      break; // stop at target
    
    char v1[1000],v2[1000];
    strcpy(v1,getVertexbyId(g,u));
    int n = outdegree(g, v1, output);
    JRB node; 
    for (i = 0; i < n; i++){
      int v = output[i];
      strcpy(v2,getVertexbyId(g,v));
      int w = getEdgeValue(g, v1, v2);
      if ( distance[v] > distance[u] + w ){    
	distance[v] = distance[u] + w;
	previous[v] = u;
      }   
      if(visit[v] == 0) 
	dll_append(queue, new_jval_i(v));
    }
  }
  
  int total = distance[t]; 
  if (total != INFINITIVE_VALUE){
    tmp[0] = t;
    int n = 1;              
    while (t != s){
      t = previous[t];
      tmp[n++] = t;
    }
    for (i= n-1; i >= 0; i--)
      path[n-i-1] = tmp[i];

    *length = n;                
  }
  return total;   
}

void dropGraph(Graph graph){
  JRB node, tree;
  jrb_traverse(node, graph.edges){
    tree = (JRB) jval_v(node->val);
    jrb_free_tree(tree);
  }
  jrb_free_tree(graph.edges);
  jrb_free_tree(graph.vertices);
  return;
}

char *xauChuan(Graph g, const char *s){
  for(int i = 1; i < id; i++)
  { 
    if(strcasecmp(s,getVertexbyId(g,i)) == 0)
      return getVertexbyId(g,i);      
  }
  return NULL;
}

char *khoangTrang(const char *x){
  char *A = strdup(x);
  int ptr = strlen(A);
  for(int i = 0;i < ptr; i++){
    if(A[i]== ' ' && A[i+1] == ' '){
      int k=i;
      
      while(k < ptr){ 
	A[k]=A[k+1];
	k++;
      }
      ptr= ptr-1;
      i--;
    }
    
    else if(A[i] == 9){
      A[i] = 32;
      strcpy(A,khoangTrang(A));
    }
  } 
  
  if(isspace(A[ptr-1]) != 0)
    A[ptr-1] = '\0';
  while(1){
    if(isspace(A[0]) == 0) break;
    for(int i = 0; i < strlen(A); i++)
      A[i] = A[i+1] ;
  }
  return A;
}

void myCSS(){
  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;
  
  provider = gtk_css_provider_new ();
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  
  const gchar *myCssFile = "style.css";
  GError *error = 0;
  
  gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile), &error);
  g_object_unref (provider);
}
