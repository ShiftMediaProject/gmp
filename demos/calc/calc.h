typedef union {
  char  *str;
  int   var;
} YYSTYPE;
#define	EOS	257
#define	BAD	258
#define	HEX	259
#define	DECIMAL	260
#define	QUIT	261
#define	ABS	262
#define	BIN	263
#define	FIB	264
#define	GCD	265
#define	KRON	266
#define	LCM	267
#define	NEXTPRIME	268
#define	POWM	269
#define	ROOT	270
#define	SQRT	271
#define	NUMBER	272
#define	VARIABLE	273
#define	LOR	274
#define	LAND	275
#define	EQ	276
#define	NE	277
#define	LE	278
#define	GE	279
#define	LSHIFT	280
#define	RSHIFT	281
#define	UMINUS	282


extern YYSTYPE yylval;
