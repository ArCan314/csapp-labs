// 3.58
long decode2(long x, long y, long z)
{
    return (((y - z) << 63) >> 63) ^ (x * y);
    // return ((y - z) < 0) ? ~(x * y) : (x * y);
}

// gcc10.2 -O2 -mavx -std=c11
/*
decode2(long, long, long):
        movq    %rsi, %rax
        imulq   %rdi, %rsi
        subq    %rdx, %rax
        salq    $63, %rax
        sarq    $63, %rax
        xorq    %rsi, %rax
        ret
*/

// 3.60
long loop(long x, long n)
{
    long result = 0;
    long mask; // edx
    for (mask = 1; mask != 0; mask = mask << n)
    {
        result |= (x & mask);
    }
    return result;
}

// gcc10.2 -O2 -mavx -std=c11
/*
loop(long, long):
        movq    %rsi, %rcx
        movl    $1, %eax
        movl    $0, %edx
.L2:
        movq    %rdi, %rsi
        andq    %rax, %rsi
        orq     %rsi, %rdx
        salq    %cl, %rax
        testq   %rax, %rax
        jne     .L2
        movq    %rdx, %rax
        ret
*/

// 3.61
long cread_alt(long *xp)
{
    // TODO
}

// 3.62

typedef enum
{
    MODE_A,
    MODE_B,
    MODE_C,
    MODE_D,
    MODE_E
} mode_t;

long switch3(long *p1, long *p2, mode_t action)
{
    long result = 0;
    switch (action)
    {
    case MODE_A:
        result = *p2;
        *p2 = *p1;
        break;
    case MODE_B:
        result = *p1 + *p2;
        *p1 = result;
        break;
    case MODE_C:
        *p1 = 59;
        result = *p2;
        break;
    case MODE_D:
        *p1 = *p2;
    case MODE_E:
        result = 27;
        break;
    default:
        result = 12;
        break;
    }
    return result;
}

// gcc10.2 -O1 -std=c11
/*
switch3(long*, long*, mode_t):
        cmpl    $4, %edx
        ja      .L9
        movl    %edx, %edx
        jmp     *.L4(,%rdx,8)
.L4:
        .quad   .L8
        .quad   .L7
        .quad   .L6
        .quad   .L5
        .quad   .L3
.L3:
        movl    $27, %eax
        ret
.L8:
        movq    (%rsi), %rax
        movq    (%rdi), %rdx
        movq    %rdx, (%rsi)
        ret
.L7:
        movq    (%rsi), %rax
        addq    (%rdi), %rax
        movq    %rax, (%rdi)
        ret
.L6:
        movq    $59, (%rdi)
        movq    (%rsi), %rax
        ret
.L5:
        movq    (%rsi), %rax
        movq    %rax, (%rdi)
        movl    $27, %eax
        ret
.L9:
        movl    $12, %eax
        ret
*/

// 3.63
long switch_prob(long x, long n)
{
    long result = x;
    switch (n)
    {
    case 0x3c: // 4005a1
    case 0x3e:
        result = 8 * x;
        break;
    case 0x3f: // 4005aa
        result = x >> 3;
        break;
    case 0x40: // 4005b2
        x = x * 15;
    case 0x41: // 4005bf
        x = x * x;
    default: // 4005c3
        result = x + 0x4b;
        break;
    }
    return result;
}

// gcc10.2 -O1 -std=c11
/*
switch_prob(long, long):
        subq    $60, %rsi
        cmpq    $5, %rsi
        ja      .L2
        jmp     *.L4(,%rsi,8)
.L4:
        .quad   .L7
        .quad   .L2
        .quad   .L7
        .quad   .L6
        .quad   .L5
        .quad   .L3
.L7:
        leaq    0(,%rdi,8), %rax
        ret
.L6:
        movq    %rdi, %rax
        sarq    $3, %rax
        ret
.L5:
        movq    %rdi, %rax
        salq    $4, %rax
        subq    %rdi, %rax
        movq    %rax, %rdi
.L3:
        imulq   %rdi, %rdi
.L2:
        leaq    75(%rdi), %rax
        ret
*/

// 3.64
#define R 7
#define S 5
#define T 13
long A[R][S][T];

long store_ele(long i, long j, long k, long *dest)
{
    *dest = A[i][j][k];
    return sizeof(A);
}

// gcc10.2 -O1 -std=c11
/*
store_ele(long, long, long, long*):
        leaq    (%rsi,%rsi,2), %rax
        leaq    (%rsi,%rax,4), %rax
        movq    %rdi, %rsi
        salq    $6, %rsi
        addq    %rdi, %rsi
        addq    %rsi, %rax
        addq    %rdx, %rax
        movq    A(,%rax,8), %rax
        movq    %rax, (%rcx)
        movl    $3640, %eax
        ret
A:
        .zero   3640
*/

// 3.65
#define M 15
// A. %rdx = &A[i][j]
// B. %rax = &A[j][i]
void transpose(long A[M][M])
{
    long i, j;
    for (i = 0; i < M; i++)
        for (j = 0; j < i; j++)
        {
            long t = A[i][j];
            A[i][j] = A[j][i];
            A[j][i] = t;
        }
}

// gcc10.2 -O1 -std=c11
/*
transpose(long (*) [15]):
        leaq    120(%rdi), %r10
        leaq    8(%rdi), %r9
        subq    $-128, %rdi
        movl    $0, %r8d
        jmp     .L2
.L3:
        addq    $120, %r10
        addq    $8, %r9
        subq    $-128, %rdi
.L2:
        addq    $1, %r8
        cmpq    $15, %r8
        je      .L7
        testq   %r8, %r8
        jle     .L3
        movq    %r9, %rdx
        movq    %r10, %rax
.L4:
        movq    (%rax), %rcx
        movq    (%rdx), %rsi
        movq    %rsi, (%rax)
        movq    %rcx, (%rdx)
        addq    $8, %rax
        addq    $120, %rdx
        cmpq    %rdi, %rax
        jne     .L4
        jmp     .L3
.L7:
        ret
*/

// 3.66

#define NR(n) (3 * (n))
#define NC(n) (4 * (n) + 1)

long sum_col(long n, long A[NR(n)][NC(n)], long j)
{
    long i;
    long result = 0;
    for (i = 0; i < NR(n); i++)
    {
        result += A[i][j];
    }
    return result;
}

// 3.67

/*
|1|2|3|4|5|6|7|8|
|               | 96
|               | 88
|       z       | 80
|       x       | 72
|       y       | 64 struct r
|               | 56
|               | 48
|               | 40
|               | 32
|       z       | 24  
|      &z       | 16  
|       y       | 8  
|       x       | 0 <-- %rsp  struct s
*/

// 3.68
#define A 5
#define B 9

typedef struct
{
    int x[A][B]; // 176 || 180
    long y; // 184
} str1;

typedef struct
{
    char array[B];// B > 4 && B < 8
    int t; // 8
    short s[A]; // 12 + A * 2 A <= 10
    long u; // 32
} str2;

void setVal(str1 *p, str2 *q)
{
    long v1 = q->t;
    long v2 = q->u;
    p->y = v1 + v2;
}

// gcc10.2 -O1 -std=c11
/*
setVal(str1*, str2*):
        movslq  8(%rsi), %rax
        addq    32(%rsi), %rax
        movq    %rax, 184(%rdi)
        ret
*/

// 3.69

