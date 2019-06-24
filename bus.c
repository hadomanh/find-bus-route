#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"jrb.h"
#include"dllist.h"
#include"jval.h"
#define INFINITIVE_VALUE 100000
int id = 1;
int output[10000];

typedef struct{
    JRB edges;
    JRB vertices;
}Graph;

typedef struct 
{
  int count;    // save count of bus
  Dllist list_of_bus;  // save all of bus go though vertex
}Vertex;
Vertex dinh[1500];

Graph creatGraph();
void addVertex(Graph g,char *name); 
int getVertex(Graph g, char *name); //Lay ra ID cua ten ben vd ben gia lam id 1; bach khoa id :2
char *getVertexbyId(Graph g,int id); // Lay ra ten dinh neu biet ID
void addEdge(Graph graph, char *v1,char *v2);
int getEdgeValue(Graph graph,char *v1, char *v2); // return INFINITIVE_VALUE if no edge between v1 and v2
int outdegree(Graph graph, char *v, int* output);
int indegree(Graph g, char *v, int *output);
int shortestPath(Graph graph, int s, int t, int* path,int*length);
void dropGraph(Graph graph);
void xulychuoi(char *a,Graph g,char *s);
void readFile(FILE *fr,Graph g);
int checkStation(Graph g,char *name); //Kiem tra dia diem co ton tai
void print_route(Vertex a,Vertex b);// In cac tuyen di qua 1 diem
void menu();
char *xauChuan(Graph g,char *s); // Cha ve xau dc chuan hoa
char *khoangTrang(char *s); // Tra ve xau sau khi loai bo space thua,tab

int main(){
    int i, length, path[100], s, t;
    char s1[1000],s2[1000],place[1000];
    Dllist ptr;
    int input;
    for (i=1;i<1500;i++)
    {
      dinh[i].list_of_bus= new_dllist();
    }
    Graph g = creatGraph();
    FILE *fr = fopen("data.txt","r");
    readFile(fr,g);  
  
    do{
      menu();
      printf("Yeu cau cua ban: ");
      scanf("%d",&input);
      switch(input){
      case 1:{
        tt:__fpurge(stdin);
        printf("Diem bat dau: ");
        gets(s1);
        strcpy(s1,khoangTrang(s1));
        if(checkStation(g,s1)== 0) 
        {
          printf("Khong ton tai diem dung %s\n",s1 );
          printf("Nhap lai ");
          goto tt;
        }
        strcpy(s1,xauChuan(g,s1));

        tt2:__fpurge(stdin);
        printf("Diem ket thuc: ");
        gets(s2);
        strcpy(s2,khoangTrang(s2));
        if(checkStation(g,s2) == 0)
        {
          printf("Khong ton tai diem dung %s\n",s2 );
          printf("Nhap lai ");
          goto tt2;
        }
        strcpy(s2,xauChuan(g,s2));

        int v1 = getVertex(g,s1);
        int v2 = getVertex(g,s2); 
        int w = shortestPath(g, v1, v2, path, &length);
        if (w == INFINITIVE_VALUE)
        {
           printf("Ko ton tai duong tu %s den %s\n", s1, s2);
        }
        
        else
        {
         printf("===============================\n");
         printf("Tu %s den %s (khoang cach %d)\n", s1, s2, w);
         for (i = 1; i < length; i++)
         {
          printf("%s => %s\nTuyen: ",getVertexbyId(g,path[i-1]), getVertexbyId(g,path[i]));
          print_route(dinh[path[i-1]],dinh[path[i]]);                        
         }     
        } 
        break;
      }
        
      case 2:{
        tt3:__fpurge(stdin);
        printf("Nhap dia diem ban can tim: ");
        gets(place);
        strcpy(place,khoangTrang(place));

        if(checkStation(g,place)==0)
        {
          printf("Khong ton tai diem dung %s\n",place );
          printf("Nhap lai ");
          goto tt3;
        }
        
        strcpy(place,xauChuan(g,place));
        dll_traverse(ptr,dinh[getVertex(g,place)].list_of_bus)
          printf("%s ",jval_s(ptr->val));
        
        printf("\n\n"); 

        JRB node;
        printf("Cac diem lien ket voi %s\n",place);
        
        jrb_traverse(node,g.vertices)
        {
          if(getEdgeValue(g,place,jval_s(node->key)) == 1)
            printf(" => %s\n",jval_s(node->key));
        }
        break;
      }
      
      case 0:
      {
        printf("Cam on ban da dung chuong trinh\n");
        break;
      }

      default: 
        printf("Ban nhap cac so (0,1,2).Moi nhap lai!\n");
        
      }
    }while(input!=0);

    for (i=1;i<1500;i++)
      free_dllist(dinh[i].list_of_bus);
    
    fclose(fr); 
    dropGraph(g);
}

int checkStation(Graph g,char *name){
  char station[50];
  if(xauChuan(g,name) == NULL) 
    return 0;
  JRB node = jrb_find_str(g.vertices,strdup(xauChuan(g,name)));
  if(node == NULL)   
      return 0;
   // not exist
  else 
    return 1;// exist
}

void menu(){
  printf("===============================\n");
  printf("\t\t\t***CHUONG TRINH DUONG DI XE BUS(HA NOI)***\n");
  printf("\t\t0.THOAT CHUONG TRINH\n");
  printf("\t\t1.TIM DUONG DI NGAN NHAT GIUA 2 DIEM DUNG\n");
  printf("\t\t2.TRA CUU THONG TIN TRAM BUS\n");
}

void print_route(Vertex a,Vertex b){
  Dllist ptr;
  Dllist ptr1;
  dll_val(a.list_of_bus);
   if(a.count <= b.count){
    dll_traverse(ptr,a.list_of_bus){
      dll_traverse(ptr1,b.list_of_bus){
        if(strcmp(jval_s(ptr->val),jval_s(ptr1->val))==0) printf("%s ",jval_s(ptr->val) );
      }
    }
   } else {
    dll_traverse(ptr,b.list_of_bus){
      dll_traverse(ptr1,a.list_of_bus){
        if(strcmp(jval_s(ptr->val),jval_s(ptr1->val))==0) printf("%s ",jval_s(ptr->val) );
      }
    }
   }
   printf("\n\n");
}

void xulychuoi(char *a,Graph g,char *s)
{       Dllist ptr;  
        int j;
        int index = 0;
        char **b = (char **)malloc(10000*sizeof(char));
        char *p;
        p = strtok(a, "-"); //cat chuoi bang cac ky tu ,. va space
    
        while(p != NULL)
        {
            b[index] = p;
            index++;
            p = strtok(NULL, "-"); //cat chuoi tu vi tri dung lai truoc do
        }

        for(j = 0; j < index; j++)
        {
          while(b[j][0] == ' ') 
            strcpy(&(b[j][0]),&(b[j][1])); // Loai bo ca ky tu trong o dau  
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

char *khoangTrang(char *A){
  int ptr = strlen(A);
  for(int i = 0;i < ptr; i++)
    {
      if(A[i]== ' ' && A[i+1] == ' ')
	    {
        int k=i;
	
        while(k < ptr)
	      { 
          A[k]=A[k+1];
	        k++;
        }
	      ptr= ptr-1;
	      i--;
      }

      else if(A[i] == 9)
      {
        A[i] = 32;
        strcpy(A,khoangTrang(A));
      }
    } 

  if(isspace(A[ptr-1]) != 0)
    A[ptr-1] = '\0';
  return A;
}

void readFile(FILE *fr,Graph g){
    while( !feof(fr))
    {   char s[20],*a,*b;
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

char *xauChuan(Graph g,char *s){
  for(int i = 1; i < id; i++)
  { 
    if(strcasecmp(s,getVertexbyId(g,i)) == 0)
      return getVertexbyId(g,i);      
  }
  return NULL;
}

Graph creatGraph(){
	Graph g;
	g.edges = make_jrb();
	g.vertices = make_jrb();
	return g;
}

void addVertex(Graph g,char *name){
    JRB node = jrb_find_str(g.vertices,strdup(name));
	if(node == NULL)
    {
        jrb_insert_str(g.vertices,strdup(name),new_jval_i(id));
        id = id + 1;
    }
}

int getVertex(Graph g, char *name ){
	JRB node;
	node = jrb_find_str(g.vertices,strdup(name));
	if(node == NULL) return 0;
	else
		return jval_i(node->val);
}

char *getVertexbyId(Graph g,int id){
    JRB node;
    jrb_traverse(node,g.vertices)
    {
        if(jval_v(node->val) == id)
            return jval_s(node->key);
    }
}

void addEdge(Graph g, char *v1, char *v2){ 
	JRB node,tree;
	if(getEdgeValue(g,v1,v2) == INFINITIVE_VALUE)
	{
		node = jrb_find_str(g.edges,strdup(v1));
		if(node == NULL)
		{
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
	jrb_traverse(node,tree)
	{       
		output[total] = getVertex(g,jval_s(node->key)); // Lay ra ID
		total++;
	} 
	return total;
}

int indegree(Graph g, char *v, int *output){
	JRB node,tree;
	int total = 0;
	jrb_traverse(node,g.edges)
	{
		tree = (JRB) jval_v(node->val);
		if(jrb_find_str(tree,strdup(v)))
		{
			output[total] = getVertex(g,jval_s(node->key));
			total++;
		}
	}
	return total;
}

int shortestPath(Graph g, int s, int t, int* path, int*length)
{   
	int distance[10000]; // Luu khoang cach tu s den dinh dang xet
	int min;
    int previous[10000], tmp[10000]; // previos lu dinh o trc no
	int visit[10000];
    int output[10000], i, u;
    Dllist ptr, queue, node;

   for (i=0; i < 10000; i++)
   {
	   distance[i] = INFINITIVE_VALUE;
	   visit[i] = 0;
   }
       
   distance[s] = 0;
   previous[s] = s;
   visit[s] = 1;

   queue = new_dllist();
   dll_append(queue, new_jval_i(s));
   
   while ( !dll_empty(queue) )
   {
      // get u from the priority queue   
      min = INFINITIVE_VALUE;	  
      dll_traverse(ptr, queue) //Duyet queue
      {
        u = jval_i(ptr->val);              
        if (min > distance[u])
        {
            min = distance[u];
            node = ptr; //node de luu phan tu nho nhat
        }                
      }
	  u = jval_i(node->val);
	  visit[u] = 1;
      dll_delete_node(node);
      
      if (u == t) break; // stop at target
      
      char v1[1000],v2[1000];
      strcpy(v1,getVertexbyId(g,u));
      int n = outdegree(g, v1, output);
      JRB node; 
      for (i = 0; i < n; i++)
      {
        int v = output[i];
        strcpy(v2,getVertexbyId(g,v));
        int w = getEdgeValue(g, v1, v2);
        if ( distance[v] > distance[u] + w )
        {    
            distance[v] = distance[u] + w;
            previous[v] = u;
        }   
		if(visit[v] == 0) 
            dll_append(queue, new_jval_i(v));
      }
   }
   
   int total = distance[t]; 
   if (total != INFINITIVE_VALUE)
   {
       tmp[0] = t;
       int n = 1;              
       while (t != s)
       {
             t = previous[t];
             tmp[n++] = t;
       }
       for (i= n-1; i >= 0; i--)
           path[n-i-1] = tmp[i];
       *length = n;                
   }
   return total;   
}

void dropGraph(Graph graph)
{
    JRB node, tree;
    jrb_traverse(node, graph.edges)
    {
        tree = (JRB) jval_v(node->val);
        jrb_free_tree(tree);
    }
    jrb_free_tree(graph.edges);
    jrb_free_tree(graph.vertices);
}