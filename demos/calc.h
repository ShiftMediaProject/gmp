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
#define	LCM	266
#define	NEXTPRIME	267
#define	POWM	268
#define	ROOT	269
#define	SQRT	270
#define	NUMBER	271
#define	VARIABLE	272
#define	LOR	273
#define	LAND	274
#define	EQ	275
#define	NE	276
#define	LE	277
#define	GE	278
#define	LSHIFT	279
#define	RSHIFT	280
#define	UMINUS	281


extern YYSTYPE yylval;
