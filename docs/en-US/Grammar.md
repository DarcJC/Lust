# Grammar

> This version is translated by GPT4.

The design goal of this grammar is to enable parsing through the LL(k) method.

- L: Scan from left to right
- L: Use leftmost derivation
- k: Finite k token lookahead

As it utilizes leftmost derivation, the grammar rules require elimination of left recursion, if present.

## Terminals
Terminals halt further derivation, resulting in the creation of a token. Terminals are denoted using uppercase letters.

- LET: Keyword let
- CONST: Keyword const
- MUT: Keyword mut
- FN: Keyword fn
- STRUCT: Keyword struct
- TRAIT: Keyword trait
- IMPL: Keyword impl
- FOR: Keyword for
- TYPE: Keyword type
- IF: Keyword if
- ELSE: Keyword else
- LOOP: Keyword loop
- WHILE: Keyword while
- BREAK: Keyword break
- CONTINUE: Keyword continue
- IN: Keyword in
- ENUM: Keyword enum
- RANGE: ..
- RANGEEQ: ..=
- IDENT: identifier
- EQ: =
- LPAREN: (
- RPAREN: )
- LBRACE: {
- RBRACE: }
- SEMICOLON: ;
- COLON: :
- COLONCOLON: ::
- ARROW: ->
- COMMA: ,
- HASH: #
- DOT: .
- SQ: '
- DQ: "
- LBRACKET: [
- RBRACKET: ]
- EOF: end of file
- OR: || or keyword or
- AND: && or keyword and
- NOT: ! or keyword not
- EQEQ: ==
- NEQ: !=
- LT: <
- LTE: <=
- GT: >
- GTE: >=
- PLUS: +
- PLUSPLUS: ++
- MINUS: -
- MINUSMINUS: --
- STAR: *
- STARSTAR: **
- SLASH: /
- COMMENT: //
- BITOR: |
- BITXOR: ^
- BITAND: &
- BITINV: ~
- INT: integer literal
- FLOAT: floating point literal
- STRING: string literal
- NEWLINE: newline
- COMMENTVAL: comment content

## Nonterminals

Nonterminals continue to derive until all symbols are terminals. Nonterminal names start with an uppercase letter, distinguishing them from the all-uppercase terminals.

## Grammar Rules

### Basic Grammar
```
# Program, the start of the grammar
# Composed of any number of statements
Program -> Statement* EOF

# Statement
# Can be variable declaration, function declaration, list statement, structure declaration, Trait declaration, implementation declaration, type alias, enumeration declaration, control flow, expression statement
Statement -> VarDecl | FnDecl | StructDecl | TraitDecl | ImplDecl | TypeAliasDecl | EnumDecl | ControlFlow | ExprStatement
# Expression statement
ExprStatement -> Expr SEMICOLON

# Comment statement
CommentStatement -> COMMENT COMMENTVAL NEWLINE

# Variable declaration
VarDecl -> Attributes (LET | CONST) MUT? IDENT (COLON TypeExpr)? EQ (Expr SEMICOLON | Block)

# Type alias
TypeAliasDecl -> TYPE IDENT EQ TypeExpr SEMICOLON

# Structure declaration
StructDecl -> Attributes STRUCT IDENT Generics? (StructBody | SEMICOLON)
# Structure block
StructBody -> LBRACE StructField? RBRACE
# Structure field list for optional comma
StructFieldList -> StructField (COMMA StructField)* COMMA?
# Structure field
StructField -> Attributes MUT? IDENT COLON TypeExpr (SEMICOLON | COMMA)

# Trait declaration
TraitDecl -> Attributes TRAIT IDENT Generics? LBRACE TraitItem* RBRACE
# Inside a Trait block
TraitItem -> TraitFn | TypeAliasDecl SEMICOLON
# Trait function
TraitFn -> Attributes FN IDENT Generics? LPAREN Params RPAREN (ARROW TypeExpr)? (Block | SEMICOLON)

# Implementation declaration
ImplDecl -> Attributes IMPL Generics? (TypeTrait FOR)? TypeExpr LBRACE ImplItem* RBRACE
# Inside an implementation block
ImplItem -> FnDecl | TypeAliasDecl SEMICOLON

# Function declaration
FnDecl -> Attributes FN IDENT Generics? LPAREN Params RPAREN (ARROW TypeExpr)? Block

# Enumeration declaration
EnumDecl -> Attributes ENUM IDENT Generics? LBRACE EnumItemList? RBRACE
# Enumeration item list
EnumItemList -> EnumItem (COMMA EnumItem)* COMMA?
# Enumeration item
EnumItem -> IDENT (LPAREN TypeExpr RPAREN)?

# Attributes annotation
Attributes -> Attribute*
# Attribute
Attribute -> HASH LBRACKET Namespace IDENT (LPAREN (IDENT | Literal) (COMMA (IDENT | Literal))* RPAREN)? RBRACKET

# Namespace
Namespace -> (IDENT COLONCOLON)*

# Generics
Generics -> LT GenericParam (COMMA GenericParam)* GT
# Generic parameter
GenericParam -> IDENT (COLON TypeTrait)?

# Trait type for impl XXXTrait for XXX {}
TypeTrait -> IDENT (COLON COLON IDENT)*

# Parameter list
Params -> (Param (COMMA Param)* COMMA? )?

# Parameter
Param -> IDENT COLON TypeExpr

# Code block
Block -> LBRACE Statement* (Expr)? RBRACE

# Label
Label -> SQ IDENT
```

### Expression Grammar

Expressions represent a set of operations. If this is unclear, remember this section follows leftmost derivation!

```
# Expression, it needs to be evaluated and placed somewhere or requires the effects of its evaluation
Expr -> Assignment

# Assignment expression, allows chain assignment
Assignment -> LogicalOr (EQ Assignment)?

# Logical or
LogicalOr -> LogicalAnd (OR LogicalAnd)*

# Logical and
LogicalAnd -> Equality (AND Equality)*

# Equality/Inequality
Equality -> Relational ((EQEQ | NEQ) Relational)*

# Comparison/Relative relationships
Relational -> Additive ((LT | LTE | GT | GTE) Additive)*

# Arithmetic addition and subtraction
Additive -> Multiplicative ((PLUS | MINUS) Multiplicative)*

# Arithmetic multiplication
Multiplicative -> BitwiseOr ((STAR | SLASH) BitwiseOr)*

# Bitwise or
BitwiseOr -> BitwiseXor (BITOR BitwiseXor)*

# Bitwise xor
BitwiseXor -> BitwiseAnd (BITXOR BitwiseAnd)*

# Bitwise and
BitwiseAnd -> Exponential (BITAND Exponential)*

# Exponential operation
Exponential -> Unary (STARSTAR Unary)*

# Unary operators
Unary -> (MINUS | NOT | BITINV | PLUSPLUS | MINUSMINUS)? Primary (PLUSPLUS | MINUSMINUS)?

# Basic expressions
Primary -> IDENT | Literal | LPAREN Expr RPAREN | FnCall | ArrayInit | ControlFlow | BreakExpr | RangeExpr

# Literal
Literal -> INT | FLOAT | STRING

# Function call
FnCall -> IDENT LPAREN ArgumentList RPAREN

# Argument list
ArgumentList -> (Expr (COMMA Expr)*)?

# Array initialization
ArrayInit -> LBRACKET Expr (COMMA Expr)* (SEMICOLON INT)? RBRACKET

# Type member access
MemberAccess -> Primary (DOT IDENT | ARROW IDENT)*

# Type
TypeExpr -> TupleTypeExpr | SimpleTypeExpr
# Base type
SimpleTypeExpr -> Namespace? IDENT Generics?
# Tuple type
TupleTypeExpr -> LPAREN TupleTypeItemList? RPAREN
# Tuple element list
TupleTypeItemList -> TypeExpr (COMMA TypeExpr)* COMMA?

# Tuple expression
TupleExpr -> LPAREN TupleItemList? RPAREN
# Tuple element list
TupleItemList -> TypeExpr (COMMA TypeExpr)* (COMMA)?
```

### Control Flow Grammar

```
# Control flow
ControlFlow -> IfExpr | LoopExpr | WhileExpr | ForExpr

# if/else expression
IfExpr -> IF Expr Block (ELSE IF Expr Block)* (ELSE Block)?

# break expression
BreakExpr -> BREAK Label? Expr?

# loop expression
LoopExpr -> (Label COLON)? LOOP Block

# while expression
WhileExpr -> WHILE Expr Block

# for expression
ForExpr -> FOR IDENT IN Expr Block

# for range expression (syntactic sugar)
RangeExpr -> Expr RANGE Expr | Expr RANGE EQ Expr
```
