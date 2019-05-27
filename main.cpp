#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

enum type_of_lex {
    LEX_NULL,//0
    LEX_AND,//1
    LEX_STRING,//2
    LEX_BOOL,//3
    LEX_DO,//4
    LEX_ELSE,//5
    LEX_IF,//6
    LEX_FALSE,//7
    LEX_INT,//8
    LEX_NOT,//9
    LEX_OR,//10
    LEX_PROGRAM,//11
    LEX_READ,//12
    LEX_WRITE,//13
    LEX_TRUE,//14
    LEX_WHILE,//15

    LEX_OPENFIG,//16
    LEX_COMMAPOINT,//17
    LEX_COMMA,//18
    LEX_CLOSEFIG,//19
    LEX_EQUAL,//20
    LEX_OOPEN,//21
    LEX_OCLOSE,//22
    LEX_ASSIGN,//23
    LEX_LESS,//24
    LEX_MORE,//25
    LEX_PLUS,//26
    LEX_MINUS,//27
    LEX_MULT,//28
    LEX_SLASH,//29
    LEX_LESEQ,//30
    LEX_NOTEQ,//31
    LEX_MOREEQ,//32

    LEX_ID,//33
    LEX_NUM,//34
    LEX_STRCONST,//35
    LEX_CONTINUE,//36
    LEX_CASE,//37
    LEX_OF,//38
    LEX_END,//39
    LEX_FOR,//40
    LEX_REAL,//41
    LEX_RNUMB,//42
    LEX_OST,//43
    LEX_COLON,//44

    POLIZ_ADDRESS,//45
    POLIZ_FGO,//46
    POLIZ_LABEL,//47
    POLIZ_GO,//48
    LEX_UNminus,//49
    LEX_UNplus //50
};

class Lex {
    type_of_lex t_lex;
    double v_lex;
public:
    Lex(type_of_lex t = LEX_NULL, double v = 0) {

        t_lex = t;
        v_lex = v;
    }

    type_of_lex get_type() {
        return t_lex;
    }

    double get_value() {
        return v_lex;
    }

    friend ostream &operator<<(ostream &s, Lex l) {
        s << '(' << l.t_lex << ',' << l.v_lex << ")";
        return s;
    }
};


class Ident {
    char *name;
    bool declare;
    type_of_lex type;
    bool assign;
    double value;
public:
    Ident() {
        declare = false;
        assign = false;
    }

    char *get_name() {
        return name;
    }

    void put_name(const char *n) {
        name = new char[strlen(n) + 1];
        strcpy(name, n);
    }

    bool get_declare() {
        return declare;
    }

    void put_declare() {
        declare = true;
    }

    type_of_lex get_type() {
        return type;
    }

    void put_type(type_of_lex t) {
        type = t;
    }

    bool get_assign() {
        return assign;
    }

    void put_assign() {
        assign = true;
    }

    double get_value() {
        return value;
    }

    void put_value(double v) {
        value = v;
    }
};

class tabl_ident {
    Ident *p;
    int size;
    int top;
public:
    tabl_ident(int max_size) {
        p = new Ident[size = max_size];
        top = 1;
    }

    ~tabl_ident() {
        delete[]p;
    }

    Ident &operator[](int k) {
        return p[k];
    }

    int put(const char *buf) {
        for (int j = 1; j < top; ++j)
            if (!strcmp(buf, p[j].get_name())) return j;
        p[top].put_name(buf);
        ++top;
        return top - 1;
    };

    int find(const char *buf) {
        for (int j = 1; j < top; ++j)
            if (!strcmp(buf, p[j].get_name())) return j;
        return 0;
    }
};

class Scanner {
    enum state {
        H, IDENT, NUMB, REAL, TWCMP, STCONST, NOTEQ, ALONESIGN, COMMENT
    };
    static const char *TW[];
    static type_of_lex words[];
    static const char *TD[];

    static type_of_lex dlms[];
    state CS;
    FILE *fp;
    char c;
    char buf[80];
    int buf_top;

    void clear() {
        buf_top = 0;
        for (int j = 0; j < 80; ++j)
            buf[j] = '\0';
    }

    void add() {
        buf[buf_top++] = c;
    }

    int look(const char *buf, const char **list) {
        int i = 0;

        while (list[i]) {

            if (!strcmp(buf, list[i])) return i;
            ++i;
        }
        return 0;
    }

    void gc() {
        c = fgetc(fp);
    }

public:
    Lex get_lex();

    Scanner(const char *program) {
        fp = fopen(program, "r");
        CS = H;
        clear();
        gc();
    }
};

static tabl_ident TID(100);

Lex Scanner::get_lex() {
    // cout<<"  ZAQ  "<<endl;
    int d, j, k = 1;
    double f;
    CS = H;
    do {
        switch (CS) {
            case H:
                if (c == EOF) return Lex();
                if (c == ' ' || c == '\n' || c == '\r' || c == '\t') gc();
                else if (isalpha(c)) {
                    clear();
                    add();
                    gc();
                    CS = IDENT;
                } else if (isdigit(c)) {
                    d = c - '0';
                    gc();
                    CS = NUMB;
                } else if (c == '{' || c == '}' || c == '(' || c == ')') {

                    clear();
                    add();
                    gc();
                    CS = ALONESIGN;
                } else if (c == '=' || c == '<' || c == '>') {
                    clear();
                    add();
                    gc();
                    CS = TWCMP;
                } else if (c == '!') {
                    clear();
                    add();
                    gc();
                    CS = NOTEQ;
                } else if (c == '"') {
                    clear();
                    gc();
                    CS = STCONST;
                } else if (c == '*' || c == '-' || c == '+' || c == ',' || c == ';' || c == '%' || c == ':') {
                    clear();
                    add();
                    gc();
                    CS = ALONESIGN;
                } else if (c == '/') {
                    clear();
                    add();
                    gc();
                    if (c == '*') {
                        clear();
                        CS = COMMENT;
                        gc();
                    } else CS = ALONESIGN;
                } else {
                    cout << " " << c << " ";
                    throw c;
                }


                break;
            case IDENT:
                if (isalpha(c) || isdigit(c)) {
                    add();
                    gc();
                } else if (j = look(buf, TW)) return Lex(words[j], j);
                else {
                    j = TID.put(buf);
                    return Lex(LEX_ID, j);
                }
                break;
            case NUMB:
                if (isdigit(c)) {
                    d = d * 10 + (c - '0');
                    gc();
                } else if (c == '.') {
                    f = d;
                    CS = REAL;
                    gc();
                } else if (isalpha(c)) throw c;
                else return Lex(LEX_NUM, d);
                break;

            case REAL:
                if (isdigit(c)) {
                    k = k * 10;
                    f = (double) (c - '0') / (double) k + f;
                    gc();
                } else if (isalpha(c)) throw c;
                else return Lex(LEX_RNUMB, f);
                break;

            case TWCMP:
                if (c == '=') {
                    add();
                    gc();
                    j = look(buf, TD);
                    return Lex(dlms[j], j);
                } else {//одинарные < > =
                    j = look(buf, TD);
                    return Lex(dlms[j], j);
                }
                break;
            case NOTEQ:
                if (c == '=') {
                    add();
                    gc();
                    j = look(buf, TD);
                    return Lex(LEX_NOTEQ, j);
                } else throw '!';
                break;
            case STCONST:
                if (c != '"') {
                    if (c == EOF) throw "unecpected EOF";
                    add();
                    gc();
                } else {
                    gc();
                    if (j = TID.find(buf)) {
                        if (TID[j].get_type() == LEX_STRCONST) return Lex(TID[j].get_type(), j);
                        else {
                            j = TID.put(buf);
                            return Lex(LEX_STRCONST, j);
                        }
                    } else {
                        j = TID.put(buf);
                        return Lex(LEX_STRCONST, j);
                    }
                }
                break;

            case ALONESIGN:
                j = look(buf, TD);
                return Lex(dlms[j], j);
            case COMMENT:
                if (c == '*') {
                    gc();
                    if (c == '/') {
                        CS = H;
                        gc();
                    } else gc();
                } else if (c == EOF) throw "unecpected EOF";
                else gc();

        } // end switch
    } while (true);
}


type_of_lex Scanner::dlms[] = {
        LEX_NULL,//0
        LEX_OPENFIG,//1
        LEX_COMMAPOINT,//2
        LEX_COMMA,//3
        LEX_CLOSEFIG,//4
        LEX_EQUAL,//5
        LEX_OOPEN,//6
        LEX_OCLOSE,//7
        LEX_ASSIGN,//8
        LEX_LESS,//9
        LEX_MORE,//10
        LEX_PLUS,//11
        LEX_MINUS,//12
        LEX_MULT,//13
        LEX_SLASH,//14
        LEX_LESEQ,//15
        LEX_NOTEQ,//16
        LEX_MOREEQ,//17
        LEX_OST,//18
        LEX_COLON,//19
        LEX_NULL //20
};

const char *Scanner::TD[] = {
        "",// 0 позиция 0 не используется
        "{",//1
        ";",// 2
        ",",// 3
        "}",// 4
        "==",// 5;
        "(",// 6
        ")",// 7
        "=",// 8
        "<",// 9
        ">",// 10
        "+",// 11
        "-",// 12
        "*",// 13
        "/", // 14
        "<=",// 15;
        "!=",// 16
        ">=",// 17
        "%",//18
        ":",//19
        nullptr,
};

type_of_lex Scanner::words[] = {
        LEX_NULL,//0
        LEX_AND,//1
        LEX_STRING,//2
        LEX_BOOL,//3
        LEX_DO,//4
        LEX_ELSE,//5
        LEX_IF,//6
        LEX_FALSE,//7
        LEX_INT,//8
        LEX_NOT,//9
        LEX_OR,//10
        LEX_PROGRAM,//11
        LEX_READ,//12
        LEX_WRITE,//13
        LEX_TRUE,//14
        LEX_WHILE,//15
        LEX_CONTINUE,//16
        LEX_CASE,//17
        LEX_OF,//18
        LEX_END,//19
        LEX_FOR,//20
        LEX_REAL,//21
        LEX_NULL //22
};

const char *Scanner::TW[] = {
        "",// 0  позиция 0 не используется
        "and",// 1
        "string",// 2
        "bool",// 3
        "do",// 4
        "else",// 5
        "if",// 6
        "false",// 7
        "int",// 8
        "not",// 9
        "or",// 10
        "program",// 11
        "read",// 12
        "write",// 13
        "true",// 14
        "while",// 15
        "continue",//16
        "case",//17
        "of",//18
        "end",//19
        "for",//20
        "real",//21
        nullptr
};


template<class T, int max_size>
class Stack {
    T s[max_size];
    int top;
public:
    Stack() { top = 0; }

    void reset() { top = 0; }

    void push(T i);

    T pop();

    bool is_empty() { return top == 0; }

    bool is_full() { return top == max_size; }
};

template<class T, int max_size>
void Stack<T, max_size>::push(T i) {
    if (!is_full()) {
        s[top] = i;
        ++top;
    } else throw "Stack_is_full";
}

template<class T, int max_size>
T Stack<T, max_size>::pop() {
    if (!is_empty()) {
        --top;
        return s[top];
    } else throw "Stack_is_empty";
}

class Poliz {
    Lex *p;
    int size;
    int free;
public:
    Poliz(int max_size) {
        p = new Lex[size = max_size];
        free = 0;
    };

    ~Poliz() { delete[]p; };

    void put_lex(Lex l) {
        p[free] = l;
        ++free;
    };

    void put_lex(Lex l, int place) { p[place] = l; };

    void blank() { ++free; };

    int get_free() { return free; };

    Lex &operator[](int index) {
        if (index > size) throw "POLIZ:out of array";
        else if (index > free) throw "POLIZ:indefinite element of array";
        else return p[index];
    };

    void print() {
        for (int i = 0; i < free; ++i) cout << p[i];
    };
};

class Parser {
    Lex curr_lex;// текущая лексема
    type_of_lex c_type;
    Lex vr;
    double c_val;
    bool flag;
    Scanner scan;
    Stack<double, 100> st_int;
    Stack<type_of_lex, 100> st_lex;
    int pl0, counter = 0;
    int pp[100];
    int z, i = 0;

    void P();// процедуры РС-метода
    void D1();

    void D();

    void B();

    void S();

    void E();

    void E1();

    void T();

    void F();

    void E2();

    void E3();

    void V();

    void V1();

    void V2();

    void dec(type_of_lex type);// семантичиеские действия
    void check_id();

    void check_op();

    void check_not();

    void eq_type();

    void eq_bool();


    void check_exp_in_case();

    void check_id_in_read();

    void check_unmp();

    bool is_for_string();//проверка сравнений и +
    bool writestr();

    void gl() // получить очередную лексему
    {
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
    }

public:
    Poliz prog;// внутреннее представление программы
    Parser(const char *program) : scan(program), prog(1000) {}
    //Parser(const char *program) : scan(program) {}

    void analyze();// анализатор с действиями
};

void Parser::analyze() {
    gl();
    P();
    prog.print();
    cout << endl << "All_Right" << endl;
}

void Parser::P() {
    if (c_type == LEX_PROGRAM) gl();
    else throw curr_lex;
    if (c_type == LEX_OPENFIG) gl();
    else throw curr_lex;
    D1();//описания
    B();//операторы

    if (c_type != LEX_CLOSEFIG) throw curr_lex;
    gl();
    if (c_type != LEX_NULL) throw curr_lex;
}

void Parser::D1() {
    D();
    while (c_type == LEX_COMMAPOINT) {
        gl();
        D();
    }
}

void Parser::D() {
    st_int.reset();
    if (c_type == LEX_INT) {
        gl();
        while (c_type == LEX_ID) {
            st_int.push(c_val);
            int val = (int) c_val;
            gl();
            if (c_type == LEX_ASSIGN) {
                prog.put_lex(Lex(POLIZ_ADDRESS, val));
                gl();
                if (c_type == LEX_MINUS) {
                    gl();
                    if (c_type == LEX_NUM) {
                        prog.put_lex(curr_lex);
                        prog.put_lex(Lex(LEX_UNminus));
                        gl();
                    } else throw curr_lex;

                } else if (c_type == LEX_NUM) {
                    prog.put_lex(curr_lex);
                    gl();
                } else throw curr_lex;
                prog.put_lex(Lex(LEX_ASSIGN));
            };
            if (c_type == LEX_COMMA) gl();
            else break;
        }
        dec(LEX_INT);
    } else if (c_type == LEX_STRING) {
        gl();
        while (c_type == LEX_ID) {
            st_int.push(c_val);
            int val = (int) c_val;
            gl();
            if (c_type == LEX_ASSIGN) {
                prog.put_lex(Lex(POLIZ_ADDRESS, val));
                gl();
                if (c_type == LEX_STRCONST) {
                    prog.put_lex(curr_lex);
                    gl();
                } else throw curr_lex;
                prog.put_lex(Lex(LEX_ASSIGN));
            };
            if (c_type == LEX_COMMA) {
                gl();
            } else break;
        }
        dec(LEX_STRING);
    } else if (c_type == LEX_BOOL) {
        gl();
        while (c_type == LEX_ID) {
            st_int.push(c_val);
            int val = (int) c_val;
            gl();
            if (c_type == LEX_ASSIGN) {
                prog.put_lex(Lex(POLIZ_ADDRESS, val));
                gl();
                if (c_type == LEX_TRUE || c_type == LEX_FALSE) {
                    prog.put_lex(curr_lex);
                    gl();
                } else throw curr_lex;
                prog.put_lex(Lex(LEX_ASSIGN));
            };
            if (c_type == LEX_COMMA) gl();
            else break;
        }
        dec(LEX_BOOL);
    } else if (c_type == LEX_REAL) {
        gl();
        while (c_type == LEX_ID) {
            st_int.push(c_val);
            double val = c_val;
            gl();
            if (c_type == LEX_ASSIGN) {
                prog.put_lex(Lex(POLIZ_ADDRESS, val));
                gl();
                if (c_type == LEX_MINUS) {
                    gl();
                    if (c_type == LEX_RNUMB) {
                        prog.put_lex(curr_lex);
                        prog.put_lex(Lex(LEX_UNminus));
                        gl();
                    } else throw curr_lex;

                } else if (c_type == LEX_RNUMB) {
                    prog.put_lex(curr_lex);
                    gl();
                } else throw curr_lex;
                prog.put_lex(Lex(LEX_ASSIGN));
            };
            if (c_type == LEX_COMMA) gl();
            else break;
        }
        dec(LEX_REAL);

    }
}

void Parser::B() {
    S();
    if (c_type != LEX_COMMAPOINT) throw curr_lex;
    while (c_type == LEX_COMMAPOINT) {
        gl();
        if (c_type == LEX_CLOSEFIG) break;
        S();
        if (c_type == LEX_CLOSEFIG) throw curr_lex;
    }
}

void Parser::S() {
    int pl0, pl1, pl2, pl3;
    if (c_type == LEX_IF) {
        gl();
        if (c_type == LEX_OOPEN) {
            gl();
            E();
            eq_bool();
            pl2 = prog.get_free();
            prog.blank();
            prog.put_lex(Lex(POLIZ_FGO));
            if (c_type == LEX_OCLOSE) {
                gl();
            } else throw curr_lex;

            S();
            pl3 = prog.get_free();
            prog.blank();
            prog.put_lex(Lex(POLIZ_GO));
            prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl2);

            if (c_type == LEX_ELSE) {
                // cout<<"A";
                gl();
                S();
                prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl3);
            } else throw curr_lex;
        } else throw curr_lex;
    }//end if
    else if (c_type == LEX_FOR) {
        gl();
        if (c_type == LEX_OOPEN) {
            gl();
            if (c_type != LEX_COMMAPOINT) {     //если есть первый аргумент
                if (c_type != LEX_ID) throw "first arg of for is wrong(par)";
                check_id();
                prog.put_lex(Lex(POLIZ_ADDRESS, c_val));
                gl();
                if (c_type != LEX_ASSIGN) throw "first arg of for is wrong(eq)";
                gl();
                E();
                eq_type();
                prog.put_lex(LEX_ASSIGN);
                if (c_type != LEX_COMMAPOINT) {
                    throw curr_lex;
                }
                gl();
                if (c_type != LEX_COMMAPOINT) { // если есть второй аргумент
                    pl0 = prog.get_free();
                    E();
                    eq_bool();
                    pl1 = prog.get_free();
                    prog.blank();
                    prog.put_lex(Lex(POLIZ_FGO));
                    pl2 = prog.get_free();
                    prog.blank();
                    prog.put_lex(Lex(POLIZ_GO));
                    pl3 = prog.get_free();
                    if (c_type != LEX_COMMAPOINT) {
                        throw curr_lex;
                    }
                    gl();
                    if (c_type != LEX_OCLOSE) {
                        if (c_type != LEX_ID) throw "first arg of for is wrong(par)";
                        check_id();
                        prog.put_lex(Lex(POLIZ_ADDRESS, c_val));
                        gl();
                        if (c_type != LEX_ASSIGN) throw "first arg of for is wrong(eq)";
                        gl();
                        E();
                        eq_type();
                        prog.put_lex(LEX_ASSIGN);
                    }

                } else {  // eсли неть второго аргумента
                    throw "lost second arg of for";
                }
            } else { // если нет первого аргумента
                gl();
                if (c_type != LEX_COMMAPOINT) { //если есть второй
                    pl0 = prog.get_free();
                    E();
                    eq_bool();
                    pl1 = prog.get_free();
                    prog.blank();
                    prog.put_lex(Lex(POLIZ_FGO));
                    pl2 = prog.get_free();
                    prog.blank();
                    prog.put_lex(Lex(POLIZ_GO));
                    pl3 = prog.get_free();
                    if (c_type != LEX_COMMAPOINT) {
                        throw curr_lex;
                    }
                    gl();
                    if (c_type != LEX_OCLOSE) {
                        if (c_type != LEX_ID) throw "first arg of for is wrong(par)";
                        check_id();
                        prog.put_lex(Lex(POLIZ_ADDRESS, c_val));
                        gl();
                        if (c_type != LEX_ASSIGN) throw "first arg of for is wrong(eq)";
                        gl();
                        E();
                        eq_type();
                        prog.put_lex(LEX_ASSIGN);
                    }
                } else {  // если нет второго
                    throw "lost second arg of for";

                }
            }
            if (c_type == LEX_OCLOSE) {
                gl();
                //prog.put_lex(Lex(LEX_READ));
            } else throw curr_lex;
            prog.put_lex(Lex(POLIZ_LABEL, pl0));
            prog.put_lex(Lex(POLIZ_GO));
            prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl2);
            S();
            prog.put_lex(Lex(POLIZ_LABEL, pl3));
            prog.put_lex(Lex(POLIZ_GO));
            prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl1);
            // prog.put_lex(Lex(POLIZ_LABEL, pl0));
            // prog.put_lex(Lex(POLIZ_GO));
            // prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl1);
        } else throw curr_lex;

    }//end for
    else if (c_type == LEX_CASE) {
        gl();
        if (c_type == LEX_OOPEN) {
            gl();
            E();
            vr = prog[prog.get_free() - 1];
            // cout<<" VR "<<vr<<endl;
            // prog.put_lex(vr);
            //check_exp_in_case();
            if (c_type == LEX_OCLOSE) {
                gl();

            } else throw curr_lex;
            if (c_type == LEX_OF) {
                gl();
            } else throw curr_lex;

            V1();

            if (c_type != LEX_END) throw curr_lex;
            while (i < counter) {
                prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pp[i]);
                i++;
            }
            counter = 0;

            gl();

        } else throw curr_lex;
    } else if (c_type == LEX_DO) {
        pl0 = prog.get_free();
        gl();
        S();
        if (c_type == LEX_WHILE) {
            gl();
            if (c_type == LEX_OOPEN) {
                gl();
                E();
                eq_bool();
                pl1 = prog.get_free();
                prog.blank();
                prog.put_lex(Lex(POLIZ_FGO));
                prog.put_lex(Lex(POLIZ_LABEL, pl0));
                prog.put_lex(Lex(POLIZ_GO));
                prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl1);
                if (c_type == LEX_OCLOSE) {
                    gl();
                } else throw curr_lex;
            } else throw curr_lex;
        } else throw curr_lex;
    }//end do
    else if (c_type == LEX_READ) {
        gl();
        if (c_type == LEX_OOPEN) {
            gl();
            if (c_type == LEX_ID) {
                check_id_in_read();
                prog.put_lex(Lex(POLIZ_ADDRESS, c_val));
                gl();
            } else throw curr_lex;
            if (c_type == LEX_OCLOSE) {
                gl();
                prog.put_lex(Lex(LEX_READ));
            } else throw curr_lex;
        } else throw curr_lex;
    }//end read
    else if (c_type == LEX_WRITE) {
        gl();
        if (c_type == LEX_OOPEN) {
            gl();
            E();
            z = 0;
            if (writestr()) z = 1;
            if (c_type == LEX_OCLOSE) {
                gl();
                prog.put_lex(Lex(LEX_WRITE, z));
            } else throw curr_lex;
        } else throw curr_lex;
    }//end write
    else if (c_type == LEX_ID) {
        check_id();
        prog.put_lex(Lex(POLIZ_ADDRESS, c_val));
        gl();
        if (c_type == LEX_ASSIGN) {
            gl();
            E();
            eq_type();
            prog.put_lex(Lex(LEX_ASSIGN));
        } else throw curr_lex;
    }//assign-end
    else if (c_type == LEX_OPENFIG) {
        gl();
        B();
        if (c_type == LEX_CLOSEFIG) gl();
        else throw curr_lex;
    };
}


void Parser::E() {
    E3();
    while (c_type == LEX_OR) {
        type_of_lex m = c_type;
        st_lex.push(c_type);
        gl();
        E3();
        check_op();
        prog.put_lex(Lex(m));
    }
}


void Parser::E3() {
    E2();
    while (c_type == LEX_AND) {
        type_of_lex m = c_type;
        st_lex.push(c_type);
        gl();
        E2();
        check_op();
        prog.put_lex(Lex(m));
    }
}

void Parser::E2() {
    E1();
    if (c_type == LEX_EQUAL || c_type == LEX_LESS || c_type == LEX_MORE || c_type == LEX_LESEQ || c_type == LEX_NOTEQ ||
        c_type == LEX_MOREEQ) {
        type_of_lex m = c_type;
        st_lex.push(c_type);
        gl();
        E1();
        z = 0;
        if (is_for_string()) z = 1;
        check_op();

        prog.put_lex(Lex(m, z));
    }
}

void Parser::E1() {
    T();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS) {
        type_of_lex m = c_type;
        st_lex.push(c_type);
        gl();
        T();
        z = 0;
        if (is_for_string()) z = 1;
        check_op();
        prog.put_lex(Lex(m, z));
    }
}

void Parser::T() {
    F();
    while (c_type == LEX_MULT || c_type == LEX_SLASH || c_type == LEX_OST) {
        type_of_lex m = c_type;
        st_lex.push(c_type);
        gl();
        F();
        check_op();
        prog.put_lex(Lex(m));
    }
}

void Parser::F() {
    if (c_type == LEX_ID) {
        check_id();
        prog.put_lex(Lex(LEX_ID, c_val));
        gl();
    } else if (c_type == LEX_STRCONST) {
        st_lex.push(LEX_STRING);
        prog.put_lex(curr_lex);
        gl();
    } else if (c_type == LEX_NUM) {
        st_lex.push(LEX_INT);
        prog.put_lex(curr_lex);
        gl();
    } else if (c_type == LEX_RNUMB) {
        st_lex.push(LEX_REAL);
        prog.put_lex(curr_lex);
        gl();
    } else if (c_type == LEX_TRUE) {
        st_lex.push(LEX_BOOL);
        prog.put_lex(Lex(LEX_TRUE, 1));
        gl();
    } else if (c_type == LEX_FALSE) {
        st_lex.push(LEX_BOOL);
        prog.put_lex(Lex(LEX_FALSE, 0));
        gl();
    } else if (c_type == LEX_NOT) {
        gl();
        F();
        check_not();
        prog.put_lex(Lex(LEX_NOT));
    } else if (c_type == LEX_MINUS) {
        gl();
        F();
        check_unmp();
        prog.put_lex(Lex(LEX_UNminus));
    } else if (c_type == LEX_PLUS) {
        gl();
        F();
        check_unmp();
        prog.put_lex(Lex(LEX_UNplus));
    } else if (c_type == LEX_OOPEN) {
        gl();
        E();
        if (c_type == LEX_OCLOSE) gl();
        else throw curr_lex;
    } else throw curr_lex;
}

void Parser::V1() {
    V();
    while (c_type == LEX_COMMAPOINT) {
        gl();
        if (c_type != LEX_END) {
            prog.put_lex(vr);
            V();
        } else prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl0);
    }
}

void Parser::V2() {
    // if (c_type == LEX_PLUS || c_type == LEX_MINUS) { gl(); }
    if (c_type == LEX_NUM || c_type == LEX_RNUMB || c_type == LEX_STRCONST || c_type == LEX_TRUE ||
        c_type == LEX_FALSE) {
        if (counter != 0) prog.put_lex(Lex(POLIZ_LABEL, prog.get_free() - 1), pl0);
        prog.put_lex(curr_lex);
        prog.put_lex(Lex(LEX_EQUAL));
        gl();

        pl0 = prog.get_free();
        prog.blank();
        prog.put_lex(Lex(POLIZ_FGO));

        /*  pp[counter] = prog.get_free();
          counter++;
          prog.blank();
          prog.put_lex(Lex(POLIZ_GO));*/


    } else throw curr_lex;

}

void Parser::V() {
    V2();
    if (c_type != LEX_COLON) throw curr_lex;
    gl();
    S();
    pp[counter] = prog.get_free();
    counter++;
    prog.blank();
    prog.put_lex(Lex(POLIZ_GO));
}

void Parser::dec(type_of_lex type) {
    double i;
    while (!st_int.is_empty()) {
        i = st_int.pop();
        if (TID[i].get_declare())throw "twice declaration of ind";
        else {
            TID[i].put_declare();
            TID[i].put_type(type);
        }
    }
}

void Parser::check_id() {
    if (TID[c_val].get_declare()) st_lex.push(TID[c_val].get_type());
    else throw "not declared";
}

void Parser::check_op() {
    type_of_lex t1, t2, op, i = LEX_INT, s = LEX_STRING;
    t2 = st_lex.pop();
    op = st_lex.pop();
    t1 = st_lex.pop();
    if (op == LEX_PLUS && t1 == s) {
        if (t1 == t2) st_lex.push(s);
        else throw "wrong types are in operation +";
    } else {

        if (op == LEX_OST && t1 == i) {
            if (t1 == t2) st_lex.push(i);
            else throw "wrong types are in operation %";
        } else {

            if (op == LEX_EQUAL || op == LEX_LESS || op == LEX_MORE || op == LEX_LESEQ || op == LEX_NOTEQ ||
                op == LEX_MOREEQ) {
                if (t1 == t2 && (t1 == i || t1 == s || t1 == LEX_REAL) || (t1 == LEX_REAL && t2 == LEX_INT) ||
                    (t1 == LEX_INT && t2 == LEX_REAL))
                    st_lex.push(LEX_BOOL);
                else throw "wrong types are in operation +";
            } else {
                if (op == LEX_MINUS || op == LEX_MULT || op == LEX_SLASH || op == LEX_PLUS) {
                    if (t1 == t2 && t1 == i) st_lex.push(i);
                    else if ((t1 == LEX_INT || t1 == LEX_REAL) && (t2 == LEX_INT || t2 == LEX_REAL)) {
                        st_lex.push(LEX_REAL);
                    } else throw "wrong types are in operation +,-,*,/";
                } else if (op == LEX_OR || op == LEX_AND) {
                    if (t1 == t2 && t1 == LEX_BOOL) {
                        st_lex.push(LEX_BOOL);
                    } else throw "wrong types in operation or/and";
                }

            }
        }
    }
}

void Parser::check_not() {
    if (st_lex.pop() != LEX_BOOL) throw "wrong type is in not";
    else { st_lex.push(LEX_BOOL); }
}

void Parser::check_unmp() {
    if (st_lex.pop() == LEX_INT) {
        st_lex.push(LEX_INT);
    } else if (st_lex.pop() == LEX_REAL) {
        st_lex.push(LEX_REAL);
    } else throw "wrong type is in UNminusPlus";
}

void Parser::eq_type() {
    type_of_lex firstArg = st_lex.pop(), secondArg = st_lex.pop();
    if (!(firstArg == secondArg || (firstArg == LEX_REAL && secondArg == LEX_INT) ||
          (firstArg == LEX_INT && secondArg == LEX_REAL)))
        throw "wrong types are in =";
}

void Parser::eq_bool() {
    if (st_lex.pop() != LEX_BOOL) throw "expression is not boolean";
}

void Parser::check_exp_in_case() {
    type_of_lex lubaya = st_lex.pop();
    if (lubaya != LEX_INT && lubaya != LEX_STRING && lubaya != LEX_REAL) {
        throw "bad type in case";
    }
}

void Parser::check_id_in_read() {
    if (!TID[c_val].get_declare()) throw "not declared in read";
}

bool Parser::is_for_string() {
    type_of_lex t1, t2;
    t2 = st_lex.pop();
    t1 = st_lex.pop();
    if (t1 == LEX_STRING || t2 == LEX_STRING) {
        st_lex.push(t1);
        st_lex.push(t2);
        return true;
    } else {
        st_lex.push(t1);
        st_lex.push(t2);
        return false;
    }
}

bool Parser::writestr() {
    type_of_lex t = st_lex.pop();
    if (t == LEX_STRING) {
        st_lex.push(t);
        return true;
    } else {
        st_lex.push(t);
        return false;
    }
}

class Executer {
    Lex pc_el;
public:
    void execute(Poliz &prog);
};


void Executer::execute(Poliz &prog) {
    Stack<double, 100> args;
    Stack<char *, 100> argsstring;
    double i, v, v1, v2, j, index = 0, size = prog.get_free();
    const char *i1;
    const char *i2;
    type_of_lex temp;
    while (index < size) {
        pc_el = prog[index];
        temp = pc_el.get_type();
        switch (temp) {

            case LEX_TRUE:
            case LEX_FALSE:
            case LEX_NUM:
            case LEX_RNUMB:
            case POLIZ_ADDRESS:
            case POLIZ_LABEL:
                i = pc_el.get_value();
                //cout<<i<<"PUSH"<<endl;
                args.push(i);
                //cout<<"adr";
                break;
            case LEX_STRCONST:
                // cout<<"strconst";
                v = pc_el.get_value();
                args.push(v);
                //argsstring.push(TID[v].get_name());
                break;
            case LEX_ID:
                v = pc_el.get_value();
                if (TID[v].get_assign()) {
                    //cout<<"oooo";
                    //if (TID[i].get_type ()==LEX_STRING){
                    //int v=TID[i].get_value ();

                    //argsstring.push(TID[v].get_name());
                    //}
                    //else
                    args.push(TID[v].get_value());
                    break;
                } else throw "POLIZ: indefinite identifier";
            case LEX_NOT:
                args.push(!args.pop());
                break;
            case LEX_OR:
                i = args.pop();
                args.push(args.pop() || i);
                break;
            case LEX_AND:
                i = args.pop();
                args.push(args.pop() && i);
                break;
            case POLIZ_GO:
                index = args.pop() - 1;
                break;
            case POLIZ_FGO:
                i = args.pop();
                if (!args.pop()) index = i - 1;
                break;
            case LEX_WRITE:
                if (pc_el.get_value() == 1) {
                    v = args.pop();
                    argsstring.push(TID[v].get_name());
                    cout << argsstring.pop() << endl;
                } else {
                    v = args.pop();
                    cout << v << endl;
                }
                break;
            case LEX_READ: {
                int k;
                i = args.pop();
                if (TID[i].get_type() == LEX_INT) {
                    cout << "Input int value for";
                    cout << TID[i].get_name() << endl;
                    cin >> k;
                } else if (TID[i].get_type() == LEX_BOOL) {
                    char j[20];
                    rep:
                    cout << "Input boolean value;cout << (true or false) for";
                    cout << TID[i].get_name() << endl;
                    cin >> j;
                    if (!strcmp(j, "true")) k = 1;
                    else if (!strcmp(j, "false")) k = 0;
                    else {
                        cout << "Error in input:true/false";
                        cout << endl;
                        goto rep;
                    }
                } else {
                    char s[80];
                    cout << "Input string value for ";
                    cout << TID[i].get_name() << endl;
                    //s=(char*) malloc(100*sizeof(char));
                    cin >> s;
                    k = TID.put(s);
                }
                TID[i].put_value(k);
                TID[i].put_assign();
                break;
            }
            case LEX_PLUS:

                if (pc_el.get_value() == 1) {
                    v1 = args.pop();
                    v2 = args.pop();
                    argsstring.push(TID[v2].get_name());
                    argsstring.push(TID[v1].get_name());
                    i2 = argsstring.pop();
                    i1 = argsstring.pop();
                    char *str;
                    str = new char[strlen(i1) + strlen(i2) + 1];
                    strcpy(str, i1);
                    str[strlen(i1)] = '\0';
                    strcat(str, i2);
                    str[strlen(i1) + strlen(i2)] = '\0';
                    argsstring.push(str);
                    v1 = TID.put(str);
                    args.push(v1);
                } else {
                    v1 = args.pop();
                    v2 = args.pop();
                    args.push(v1 + v2);
                }
                break;
            case LEX_MULT:
                args.push(args.pop() * args.pop());
                break;
            case LEX_MINUS:
                i = args.pop();
                args.push(args.pop() - i);
                break;
            case LEX_SLASH:
                i = args.pop();
                if (i) {
                    args.push(args.pop() / i);
                    break;
                } else throw "POLIZ:divide by zero";
            case LEX_EQUAL:

                if (pc_el.get_value() == 1) {
                    v1 = args.pop();
                    v2 = args.pop();
                    argsstring.push(TID[v2].get_name());
                    argsstring.push(TID[v1].get_name());
                    i1 = argsstring.pop();
                    i2 = argsstring.pop();
                    args.push(strcmp(i1, i2) == 0);
                } else args.push(args.pop() == args.pop());
                break;
            case LEX_LESS:
                if (pc_el.get_value() == 1) {
                    v1 = args.pop();
                    v2 = args.pop();
                    argsstring.push(TID[v2].get_name());
                    argsstring.push(TID[v1].get_name());
                    i1 = argsstring.pop();
                    i2 = argsstring.pop();
                    args.push(strcmp(i1, i2) < 0);
                } else {
                    i = args.pop();
                    args.push(args.pop() < i);
                }
                break;
            case LEX_MORE:
                if (pc_el.get_value() == 1) {
                    v1 = args.pop();
                    v2 = args.pop();
                    argsstring.push(TID[v2].get_name());
                    argsstring.push(TID[v1].get_name());
                    i1 = argsstring.pop();
                    i2 = argsstring.pop();
                    args.push(strcmp(i1, i2) > 0);
                } else {
                    i = args.pop();
                    args.push(args.pop() > i);
                }
                break;
            case LEX_LESEQ:
                if (pc_el.get_value() == 1) {
                    v1 = args.pop();
                    v2 = args.pop();
                    argsstring.push(TID[v2].get_name());
                    argsstring.push(TID[v1].get_name());
                    i1 = argsstring.pop();
                    i2 = argsstring.pop();
                    args.push(strcmp(i1, i2) <= 0);
                } else {
                    i = args.pop();
                    args.push(args.pop() <= i);
                }
                break;
            case LEX_MOREEQ:
                if (pc_el.get_value() == 1) {
                    v1 = args.pop();
                    v2 = args.pop();
                    argsstring.push(TID[v2].get_name());
                    argsstring.push(TID[v1].get_name());
                    i1 = argsstring.pop();
                    i2 = argsstring.pop();
                    args.push(strcmp(i1, i2) >= 0);
                } else {
                    i = args.pop();
                    args.push(args.pop() >= i);
                }
                break;
            case LEX_NOTEQ:
                if (pc_el.get_value() == 1) {
                    v1 = args.pop();
                    v2 = args.pop();
                    argsstring.push(TID[v2].get_name());
                    argsstring.push(TID[v1].get_name());
                    i1 = argsstring.pop();
                    i2 = argsstring.pop();
                    args.push(strcmp(i1, i2) != 0);
                } else args.push(args.pop() != args.pop());
                break;
            case LEX_ASSIGN:
                //cout<<"1";
                v1 = args.pop();
                //cout<<"3";
                v2 = args.pop();
                //cout<<"4";
                TID[v2].put_value(v1);
                TID[v2].put_assign();
                //cout<<"2";

                break;
            case LEX_UNminus:
                args.push(-args.pop());
                break;

            default:
                cout << temp;
                throw "POLIZ: unexpected elem";
        }//end of switch
        ++index;
    };//end of while
    cout << "Finish of executing!!!" << endl;
}

class Interpretator {
    Parser pars;
    Executer E;
public:
    Interpretator(const char *program) : pars(program) {};

    void interpretation();
};

void Interpretator::interpretation() {
    pars.analyze();
    E.execute(pars.prog);
}


int main() {

    try {
        Interpretator I("test");
        I.interpretation();
        return 0;
        // Parser OD("test");
        //OD.analyze();
    }
    catch (Lex o) { cout << "unexpected symbal " << o; }
    catch (char const *o) { cout << o; }
    catch (char o) {
        if (isalpha(o)) cout << "letter " << o << " after number";
        else cout << "unknown symbal " << o;
    }
    return 1;
}