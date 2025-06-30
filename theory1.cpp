#include <iostream>
#include <bitset>
#include <unordered_map>
#include <vector>
#include <array>
#include <queue>
#include <algorithm>
using namespace std ;
#define endl '\n'

#define input_file "./input/ToSC_SarSye16_448.txt"
#define output_file "./output1/ToSC_SarSye16_448.txt"
//#define input_file "input.txt"
//#define output_file "output.txt"


//�������ĸ߶ȡ����
#define MAXHEIGHT 32
#define MAXWIDTH 32

//������ԣ����ࣩ
#define is_LEAF 0
#define Splited 1
#define To_Be_Splited 2
#define Extended 3

//extension_round  ��չ����

//����Ϊ���µľ���״̬
bool original_matrix[MAXHEIGHT][MAXWIDTH]={false} ;
//bitset<MAXWIDTH> ori_point[MAXHEIGHT] ;

unordered_map< bitset<MAXWIDTH>, int> Hash_to_point ;
unordered_map< int, bitset<MAXWIDTH>> Hash_to_bitset ;


//=====================
//ͼ�۲���
//ԭͼ�߲����Գ���������ط�����extension  ����Ҷ�ӽ����û������ĵ�����


//ԭͼ��
struct EDGE{
    int from, to, nxt ;
}Edges[MAXHEIGHT*MAXWIDTH<<1] ;
int head[MAXHEIGHT*MAXWIDTH] , idx_edge ;
int addedge(int from, int to) {
    Edges[++idx_edge] = {from,to,head[from]} ;
    head[from] = idx_edge ;
    return idx_edge ;
}

//��ͼ��
EDGE Transpose_Graph_Edges[MAXHEIGHT*MAXWIDTH<<1] ;
int head_in_Transpose_Graph[MAXHEIGHT*MAXWIDTH] , idx_edge_in_Transpose_Graph ;
int add_Transpose_Graph_edge(int from, int to) {
    Transpose_Graph_Edges[++idx_edge_in_Transpose_Graph] = {from,to,head_in_Transpose_Graph[from]} ;
    head_in_Transpose_Graph[from] = idx_edge_in_Transpose_Graph ;
    return idx_edge_in_Transpose_Graph ;
}


//��
struct NODE {//�ýṹ��洢�������
    int kind ;
    bool is_original ;  
    int size_of_original_line , *whichline ;
    ~ NODE() {
        delete whichline ;
    }
}Nodes[MAXHEIGHT*MAXWIDTH] ;
int idx_node ;
int addnode(bitset<MAXWIDTH> bs, int kind, bool is_original, int which_line) {
    //is_originalΪfalseʱ��Ĭ��whichline=-1
    //�ú���ֻ���ڴ���δ�������Ľ��ʱ����ʹ��
    idx_node++ ;
    Nodes[idx_node].kind = kind ;
    Nodes[idx_node].is_original = is_original ;

    if(is_original) {
        Nodes[idx_node].whichline = new int[MAXHEIGHT+1] ;
        Nodes[idx_node].size_of_original_line = 1 ;
        Nodes[idx_node].whichline[1] = which_line ;
    }else {
        Nodes[idx_node].size_of_original_line = 0 ;
    }
    
    Hash_to_bitset[idx_node] = bs ; 
    Hash_to_point[bs] = idx_node ;


    return idx_node ;
}
void setnode(int index_of_node, int kind) {
    Nodes[index_of_node].kind = kind ;
}






//=====================


/******************** */
//����Ϊ ������� ģ��
void input_matrix() {
    int x=0,y=0 ;
    char c = getchar() ;
    while(!(x == MAXHEIGHT)){
        if(c=='0' || c=='1') {
            if(c=='0')  original_matrix[x][y] = false ;
            else        original_matrix[x][y] = true ;
            y++ ;
            if(y==MAXWIDTH) y=0 , x++ ;
        }  
        c = getchar() ;
    }
}

void show_matrix() {
    cout << "Original Matrix:" << endl ;
    for(int i=0 ; i<MAXHEIGHT ; i++) {
        for(int j=0 ; j<MAXWIDTH ; j++)
            cout << original_matrix[i][j] << ' ' ;
        cout << endl ;
    }
}

/******************** */
int to_be_splited_point[(MAXWIDTH)+1],   sizeof_to_be_splited_point ;
int extended_point[MAXWIDTH*(MAXHEIGHT*MAXHEIGHT+1)+1],        sizeof_extened_point ; 

/******************** */
//����Ϊ ����ִ�� ģ��
void initialization() {
    for(int idx=0; idx<MAXWIDTH; idx++) {
        bitset<MAXWIDTH> bs(0) ;
        bs[idx] = 1 ;
        addnode(bs, is_LEAF, false, -1) ;
    }//����x_0 ~ x_{MAXWIDTH-1}

    for(int line=0; line<MAXHEIGHT; line++) {
        bitset<MAXWIDTH> bs(0) ;
        for(int column=0; column<MAXWIDTH; column++) {
            if(original_matrix[line][column])
                bs[column] = true ;
        }
        if(bs.count() <= 1) continue ;
        if(Hash_to_point.find(bs)!=Hash_to_point.end()) {
            int index_of_point = Hash_to_point[bs] ;

            //ע��!!!���������д����������������Ÿõ����г�ʼ�е�����½��еģ���Ҫ��֤��ʱ�õ����г�ʼ�У���������Ұָ��
            Nodes[index_of_point].size_of_original_line++ ;
            Nodes[index_of_point].whichline[Nodes[index_of_point].size_of_original_line] = line ;
        }else {
            to_be_splited_point[++sizeof_to_be_splited_point] = 
                    addnode(bs, To_Be_Splited, true, line) ;
        }
        
    }//�������Ϊ0�ĳ�ʼ��
}

bitset<MAXWIDTH> intersection[MAXWIDTH * MAXWIDTH + 1] ;    int sizeof_intersection ;
bitset<MAXHEIGHT+1> to_be_splited_in_each_intersection[MAXWIDTH * MAXWIDTH + 1] ;
array<int,4> status_of_intersection[MAXWIDTH * MAXWIDTH + 1] ;


bool compare_extended(int idx1, int idx2) {
    return Hash_to_bitset[idx1].count() < Hash_to_bitset[idx2].count() ;
}

void turn_extended_point_into_to_be_splited_point() {
    sort(extended_point+1, extended_point+sizeof_extened_point+1 , compare_extended) ;
    while(sizeof_extened_point > 0 && sizeof_to_be_splited_point<MAXHEIGHT) {
        setnode(extended_point[sizeof_extened_point], To_Be_Splited);
        to_be_splited_point[++sizeof_to_be_splited_point] = extended_point[sizeof_extened_point] ;
        sizeof_extened_point-- ;
    }//extended_point(MAXHEIGHT) -> to_be_splited_point
}
void clear_rubbish() {
    while(sizeof_to_be_splited_point) {
        to_be_splited_point[sizeof_to_be_splited_point] = 0 ;
        sizeof_to_be_splited_point-- ;
    }    
    while(sizeof_intersection) {
        intersection[sizeof_intersection] = 0 ;
        to_be_splited_in_each_intersection[sizeof_intersection] = 0 ;
        status_of_intersection[sizeof_intersection] = {0,0,0,0} ;
        sizeof_intersection-- ;
    }
}

void extension() {
    unordered_map<bitset<MAXWIDTH>,int>intersections ; 
    for(int i=1 ; i<=sizeof_to_be_splited_point ; i++) {
        for(int j=1 ; j<i ; j++) {
            bitset<MAXWIDTH>intersec = Hash_to_bitset[to_be_splited_point[i]] & Hash_to_bitset[to_be_splited_point[j]] ;
            if(intersec.count() <= 1)   continue ;
            if(intersections.find(intersec) == intersections.end()) {
                intersection[++sizeof_intersection] = intersec ;
                intersections.insert({intersec, sizeof_intersection}) ;
                to_be_splited_in_each_intersection[sizeof_intersection] = 0 ;
                to_be_splited_in_each_intersection[sizeof_intersection][i] = to_be_splited_in_each_intersection[sizeof_intersection][j] = 1 ;
            }else {
                int idx = intersections[intersec] ;
                to_be_splited_in_each_intersection[idx][i] = to_be_splited_in_each_intersection[idx][j] = 1 ;
            }
        }
    }
    for(int i=1 ; i<=sizeof_intersection ; i++) {
        int useful_length = intersection[i].count() ;
        int number_of_use = to_be_splited_in_each_intersection[i].count() ;
        int value = useful_length * (number_of_use ) ;
        status_of_intersection[i] = {value, useful_length, number_of_use, i} ;
    }

    array<int,4> info = {0,0,0,0} ;

    for(int i=1 ; i<=sizeof_intersection ; i++) {
        if(status_of_intersection[i][0] > info[0] ||
            status_of_intersection[i][0]==info[0] && status_of_intersection[i][1]>=info[1])
            info = status_of_intersection[i] ;
    }

    if(info[3] != 0 && info[2] > 1) {
        bitset<MAXWIDTH>bs = intersection[info[3]] ;
        //�� intersection ��չ��Ϊ extended_point 
        if(Hash_to_point.find(bs) == Hash_to_point.end()) {
            extended_point[++sizeof_extened_point] = 
                    addnode(bs, Extended, false, -1) ;
        }else {
            if(Nodes[Hash_to_point[bs]].kind == To_Be_Splited ) {
                setnode(Hash_to_point[bs], Extended) ;
                extended_point[++sizeof_extened_point] = Hash_to_point[bs] ;
            }
            //����� extended point, splited point ��������չ�׶�
        }

        //�������ĵ��Ϊsplited, ���ܲ��� extended
        //ע�⣺�������н���
        for(int i=1 ; i<=sizeof_to_be_splited_point ; i++) {
            if( to_be_splited_in_each_intersection[info[3]][i] == 0
                || Hash_to_bitset[to_be_splited_point[i]] == bs)    continue ;
            
            setnode(to_be_splited_point[i], Splited) ;
            //to_be_splited point �� extended point ����
            if(to_be_splited_point[i] != Hash_to_point[bs])
                addedge(to_be_splited_point[i], Hash_to_point[bs]) ;

            bitset<MAXWIDTH> fr = Hash_to_bitset[to_be_splited_point[i]] ^ bs ;
            if(fr.count() >= 2) {
                int index_of_fr ;
                if(Hash_to_point.find(fr) == Hash_to_point.end()) {
                    index_of_fr = addnode(fr, Extended, false, -1) ;
                    extended_point[++sizeof_extened_point] = index_of_fr ;
                }
                else    index_of_fr = Hash_to_point[fr] ;
                //to_be_splited point �� extended ����   
                addedge(to_be_splited_point[i], index_of_fr) ;
                
            }

        }
    }
        
        
    //��δ����ֵ� to_be_splited ֱ��ת���� extended
    for(int i=1 ; i<=sizeof_to_be_splited_point ; i++) {
        int idx = to_be_splited_point[i] ;
        if(Nodes[idx].kind == To_Be_Splited) {
            setnode(idx, Extended) ;
            extended_point[++sizeof_extened_point] = idx ;
        }
    }


    //β���������������Դ(ֻ���� extended_point  )
    clear_rubbish() ;
}


void final_connection_in_the_graph() {
    for(int i=MAXWIDTH+1 ; i<=idx_node ; i++) {
        bitset<MAXWIDTH>bs = Hash_to_bitset[i] ;
        for(int e=head[i] ; e ; e=Edges[e].nxt) {
            int to_point = Edges[e].to ;
            bs ^= Hash_to_bitset[to_point] ;
        }
        for(int bit=0 ; bit<MAXWIDTH ; bit++) {
            if(bs[bit] == 0)    continue ;
            bitset<MAXWIDTH> bs_to_point(0) ;
            bs_to_point[bit] = 1 ;
            int to_point = Hash_to_point[bs_to_point] ;
            
            addedge(i,to_point) ;
            bs[bit] = 0 ;
        }
    }
}

void build_transpose_graph() {
    for(int from=1 ; from<=idx_node ; from++) {//����ԭͼ�еĳ��㣨��ͼ����㣩
        for(int e=head[from] ; e ; e=Edges[e].nxt) {//ԭͼ�ϵı�
            int to = Edges[e].to ;
            add_Transpose_Graph_edge(to, from) ;
        }
    }
}
/******************** */

int indgree[MAXHEIGHT*MAXWIDTH] ;

int x_of_node[MAXHEIGHT*MAXWIDTH] , idx_x ;

array<int,4> result[MAXHEIGHT*MAXWIDTH] ;
int sizeof_result ;

queue<int> node_queue ;


void add_result(int to, int from, bool show_y) {
    if(x_of_node[to] == 0) {
        x_of_node[to] = x_of_node[from] ;
    }else {
        result[++sizeof_result] = {++idx_x, x_of_node[to], x_of_node[from], (show_y ? to : 0) } ;
        x_of_node[to] = idx_x ;
    }
}
void get_result_through_topu() {
    //���趨 x_0 ~ x_{MAXWIDTH-1} ��Ӧ�ĵ� �� x_of_node Ϊ 1 ~ MAXWIDTH
    for(int i=0 ; i<MAXWIDTH ; i++) {
        bitset<MAXWIDTH> bs(0) ;
        bs[i] = 1 ;
        x_of_node[Hash_to_point[bs]] = ++idx_x ;
    }

    //���±������ڷ�ͼ�Ͻ��е�
    for(int from=1 ; from<=idx_node ; from++) {
        for(int e=head_in_Transpose_Graph[from] ; e ; e=Transpose_Graph_Edges[e].nxt) {
            int to = Transpose_Graph_Edges[e].to ;
            indgree[to]++ ;
        }
    }
    for(int i=1 ; i<=idx_node ; i++) {
        if(indgree[i] == 0)
            node_queue.push(i) ;
    }
    while(!node_queue.empty()) {
        int from = node_queue.front() ; node_queue.pop() ;
        for(int e=head_in_Transpose_Graph[from] ; e ; e=Transpose_Graph_Edges[e].nxt) {
            int to = Transpose_Graph_Edges[e].to ;

            indgree[to]-- ;
            if(indgree[to] == 0)    node_queue.push(to) ;

            add_result(to, from, (indgree[to] == 0) ) ;
            
            
        }    
    }
}

void print_answer() {
    for(int i=1 ; i<=sizeof_result ; i++) {
        array<int,4> info = result[i] ;
        cout << info[0]-1 << ' ' << info[1]-1 << ' ' << info[2]-1 << endl ;
    }
}
void show_answer() {
    cout << "Xor Count = " << sizeof_result << endl ;
    for(int i=1 ; i<=sizeof_result ; i++) {
        array<int,4> info = result[i] ;
        cout << "x[" << info[0]-1 << "] = x[" << info[1]-1 << "] ^ x[" << info[2]-1 << "]" << '\t' ;
        if(info[3] && Nodes[info[3]].is_original) {
            for(int j=1 ; j<=Nodes[info[3]].size_of_original_line ; j++) {
                cout << "y[" << Nodes[info[3]].whichline[j] << "] " ;
            }
        }

        putchar('\n') ;
    }
}

signed main() {
    freopen(input_file, "r", stdin) ;
    freopen(output_file, "w", stdout) ;

    //step1: ���ձ�׼��ʽ�������
    input_matrix() ;
    //չʾ�����ľ���
    //show_matrix() ;

    //step2: ��ʼ�������� x_0 ~ x_{MAXWIDTH-1} �� ���Ϊ0�ĵ㣨������У�
    initialization() ;
    

    //step3: ָ��������ʹ�ý������̰�Ĳ��Խ�������
    for(int extension_round=1 ; extension_round<=MAXHEIGHT*MAXHEIGHT; extension_round++) {
        extension() ;

        if(sizeof_extened_point == 0)   break ;
        turn_extended_point_into_to_be_splited_point() ;
    }



    //step4: �������������� ��ͼ�е���û����������Чλ �� x_0 ~ x_{MAXWIDTH-1} ����
    final_connection_in_the_graph() ;


    //step5: ������ͼ
    build_transpose_graph() ;

    //step6: ���շ�ͼ�����򱣴���
    get_result_through_topu() ;


    //step7: ������
    //print_answer() ;
    show_answer() ;

    return 0 ;
}
