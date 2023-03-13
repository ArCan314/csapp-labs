3.6

6 + x
x + y
x + 4 * y
9 * x + 7
4 * y + 10
x + 2 * y + 9

3.7

long scale2(long x, long y, long z)
{
    long t = 5 * x + 2 * y + 8 * z;
    return t;
}

3.8

DEST        SRC
0x100       0x100
0x108       0xA8
0x118       0x110
0x110       0x14
%rcx        0
%rax        0xfd

3.9

salq $4, %rax

sarq %cl, %rax 

3.10

long arith2(long x, long y, long z)
{
    long t1 = x | y;
    long t2 = t1 >> 3;
    long t3 = ~t2;
    long t4 = z - t3;
    return t4;
}

3.11

A.B. subq %rdx, %rdx

3.12

idivq --> divq

3.13

data_t          COMP
int             <
short           >=
unsigned char   <=
long/unsigned long/ptr !=

3.14

data_t          COMP
long  >=
short/unsigned short ==
unsigned char   >
int             <=

3.15

A. 0x4003fe
B. 0x400425
C. ja: 400543 pop: 400545
D. 0x400560

3.16

A.
void cond(long a, long *p)
{
    if (!(p && a > *p))
        goto LABEL_FALSE;
    *p = a;
    goto DONE:
_3_16_FALSE:
_3_16_DONE:
}

3.18

long test(long x, long y, long z)
{
    long val = x + y + z;
    if (x < -3)
    {
        if (y < z)
            val = x * y;
        else
            val = y * z;
    }
    else if (x > 2)
        val = x * z;
    return val;
}

3.20

#define OP /

3.21

long test(long x, long y)
{
    long val = 8 * x;
    if (y > 0)
    {
        if (x >= y)
            val = x & y;
        else
            val = y - x;
    }
    else if (y <= -2)
        val = x + y;
    return val;
}

3.24

long loop_while(long a, long b)
{
    long result = 1;
    while (a < b)
    {
        result *= a + b;
        a++;
    }
    return result;
}

3.25

long loop_while2(long a, long b)
{
    long result = b;
    while (b > 0)
    {
        result = result * a;
        b = b - a;
    }
    return result;
}

3.26
long fun_a(unsigned long x)
{
    long val = 0;
    while (x != 0)
    {
        val = val ^ x;
        x >>= 1;
    }
    return val & 1;
}

3.28

long fun_b(unsigned long x)
{
    long val = 0;
    long i;
    for (i = 64; i != 0; i--)
    {
        val = (val + val) | (x & 1);
        x >>= 1;
    }
    return val;
}

3.29

x > 7 ==> .L2
switch (x + 1)
{
case 0:
    break;
case 1:
case 8:
    break;
case 2:
    break;
case 3:
case 5:
    break;
case 6:
    break;
default:
    break;
}

3.31

void switcher(long a, long b, long c, long *dest)
{
    long val;
    switch (a)
    {
    case 5:
        c = b ^ 15;
    case 0:
         val = c + 112;
         break;
    case 2:
    case 7:
        val = (b + c) << 2;
        break;
    case 4:
        val = a;
        break;
    default:
        val = b;
    }
    *dest = val;
}

3.33

size_t procprob(int a, short b, long *u, char *v)

3.34

A.
%rbp = x + 5
%rbx = x
%r12 = x + 4
%r13 = x + 3
%r14 = x + 2
%r15 = x + 1

B.
(%rsp) = x + 6
8(%rsp) = x + 7

3.35

long rfun(unsigned long x)
{
    if (x == 0)
        return 0;
    unsigned long nx = x >> 2;
    long rv = rfun(nx);
    return rv + x;
}

3.37

EXPR    TYPE    VAL       ASM
S+1     short * Xs+2      leaq 2(%rdx), %rax
S[3]    short   M(Xs+6)   movw 6(%rdx), %ax
&S[i]   short * Xs+2*i    leaq (%rdx,%rcx,2), %rax
S[4*i+1]short   M(Xs+8*i+2) movw 2(%rdx,%rcx,8), %ax
S+i-5   short * Xs+2*i-10  leaq -10(%rdx,%rcx,2), %rax

3.38

long P[5][7];
long Q[7][5];

3.39

void fix_set_diag_opt(fix_matrix A, int val)
{
    int i = 0;
    do
    {
        *(A + i) = val;
        i += N + 1;
    } while (i != N * N + N);
}

3.41

A.
p       0
s.x     8
s.y     12
next    16

B.
24

C.

void sp_init(struct prob *sp)
{
    sp->s.x = sp->s.y;
    sp->p = &(sp->s.x);
    sp->next = sp;
}

3.42

long fun(struct ELE *ptr)
{
    int result = 0;
    while (ptr)
    {
        result += ptr->v;
        ptr = ptr->p;
    }
    return result;
}

3.44

A.
i       0
c       4
j       8
d       12
size    16

B.
i       0
c       4
d       5
j       8
size    16

C.
w       0    
c       6
size    10

D.
w       0
c       16
size    40

E.
a       0
t       24
size    40

3.45

a       0
b       8
c       16
d       24
e       28
f       32
g       40
h       48
size    56

|1|2|3|4|5|6|7|8|
|    char * a   |
| b |d|f|   e   |
|       c       |
|       g       |
|   h   |  pad  |

3.50

val1 d
val2 i
val3 l
val4 f

3.51

Tx      Ty          I
long    double      vcvtsi2sdq %rdi, %xmm0
double  int         vcvttsd2si %xmm0, %eax
double  float       vmovddup %xmm0, %xmm0; vcvtpd2psx %xmm0, %xmm0
long    float       vcvtsi2ssq %rdi, %xmm0, %xmm0
float   long        vcvttss2siq %xmm0, %rax

3.52

A. %xmm0, %rdi, %xmm1, %esi
B. %edi, %rsi, %rdx, %rcx
C. %rdi, %xmm0, %esi, %xmm1
D. %xmm0, %rdi, %xmm1, %xmm2

3.53

arg1_t int/unsigned
arg2_t long/unsigned long
arg3_t float
arg4_t double

3.54

double funct2(double w, int x, float y, long z)
{
    w = w / z;
    return x * y - w;
}

3.56

A. (((x) < 0) ? (-(x)) : (x))
B. (0)
C. (-(x))

3.57

//                  rdi     xmm0    rsi     rdx
double funct3(int *ap, double b, long c, float *dp)
{
    float res = *dp;
    if (*ap > b)
        res = res * c;
    else
        res = c + 2 * res;
    return res;
}