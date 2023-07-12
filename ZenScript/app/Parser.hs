module Parser where

import Control.Monad
import Text.Parsec
import Text.Parsec.String
import Text.Parsec.Expr
import qualified Text.Parsec.Token as Token
import Text.Parsec.Language
import Data.String
import Data.List
zenScriptDef = emptyDef { Token.commentStart    = "/*"
                        , Token.commentEnd      = "*/"
                        , Token.commentLine     = "//"
                        , Token.identStart = letter <|> char '_'
                        , Token.identLetter = alphaNum <|> char '_'
                        , Token.reservedOpNames = ["=", "+", "-", "*", "^", "/","and", "or", "not", 
                                                    ">", "<", "==", ".", "->", "..", "(", ")", "[", "]", "{", "}"]
                        , Token.reservedNames = ["true", "false", "null", "lua", "lambda", "class", "init",
                                                 "if", "else", "and", "or", "not",
                                                 "while", "local", "fn", "return"]
                        }

lexer = Token.makeTokenParser zenScriptDef

identifier = Token.identifier lexer
reserved   = Token.reserved   lexer
reservedOp = Token.reservedOp lexer
parens     = Token.parens     lexer
brackets   = Token.brackets   lexer
braces     = Token.braces     lexer
integer    = Token.integer    lexer
float      = Token.float      lexer
semi       = Token.semi       lexer
whiteSpace = Token.whiteSpace lexer
comma      = Token.comma      lexer
stringLiteral = Token.stringLiteral lexer

type Ident = String

data Expr   = Var Ident
            | IntLiteral Integer
            | FloatLiteral Double
            | BoolLiteral Bool
            | ArrayLiteral [Expr]
            | TableLiteral [(Ident, Expr)]
            | StringLiteral String
            | NullLiteral
            | LambdaLiteral [Ident] Statement
            | FuncCall Expr [Expr]
            | Sum Expr Expr
            | Subtract Expr Expr
            | Product Expr Expr
            | Division Expr Expr
            | Pow Expr Expr
            | Neg Expr
            | Not Expr
            | And Expr Expr
            | Or Expr Expr
            | Concat Expr Expr
            | Greater Expr Expr
            | Smaller Expr Expr
            | Equals Expr Expr
            | BracketIndex Expr Expr
            | DotIndex Expr Ident
            | ArrowIndex Expr Ident
    deriving (Show)

isLValue :: Expr -> Bool
isLValue e = case e of
    Var _          -> True
    BracketIndex _ _ -> True
    DotIndex _ _ -> True
    _              -> False

data FuncDefinition = FuncDefinition Ident [Ident] Statement
    deriving(Show)

data ConstructorDefinition = ConstructorDefinition [Ident] Statement
    deriving(Show)

data ClassBody = ClassBody ConstructorDefinition [FuncDefinition]
    deriving(Show)

data Statement  = Seq [Statement]
                | Local Ident Expr
                | Assign Expr Expr
                | If Expr Statement
                | IfElse Expr Statement Statement
                | While Expr Statement
                | FuncDeclaration FuncDefinition
                | Return Expr
                | ReturnVoid
                | Block Statement
                | ClassDeclaration Ident ClassBody
                | LuaCode String
                | Expression Expr
    deriving (Show)

zenScriptParser :: Parser Statement
zenScriptParser = whiteSpace >> statement

statement :: Parser Statement
statement = seqOfStatements <|> block

seqOfStatements :: Parser Statement
seqOfStatements = do
    list <- many oneStatement
    return $ if length list == 1 then head list else Seq list

oneStatement :: Parser Statement
oneStatement =  ifElseIfElse
            <|> whileStatement
            <|> returnStatement
            <|> localStatement
            <|> try assignStatement
            <|> FuncDeclaration <$> funcDefinition
            <|> classDeclaration
            <|> blockStatement
            <|> luaCodeStatement
            <|> expressionStatement

block :: Parser Statement
block = braces statement

blockStatement :: Parser Statement
blockStatement = Block <$> block

luaCodeStatement :: Parser Statement
luaCodeStatement = reserved "lua" >> LuaCode <$> braces luaCode

luaCode :: Parser String
luaCode = intercalate "\n" <$> many stringLiteral

expressionStatement :: Parser Statement
expressionStatement = Expression <$> expression

localStatement :: Parser Statement
localStatement = do
    reserved "local"
    ident <- identifier
    reservedOp "="
    Local ident <$> expression

assignStatement :: Parser Statement
assignStatement = do
    expr <- expression
    reservedOp "="
    if isLValue expr
        then Assign expr <$> expression
        else fail "Assignment must have lvalue on the left"

ifElseIfElse :: Parser Statement
ifElseIfElse = try ifElseStatement <|> ifStatement

ifElseStatement :: Parser Statement
ifElseStatement = do
    reserved "if"
    expr <- expression
    blk1 <- block
    reserved "else"
    blk2 <- block <|> ifElseIfElse
    return $ IfElse expr blk1 blk2

ifStatement :: Parser Statement
ifStatement =
    reserved "if" >>
    If <$> expression <*> block

whileStatement :: Parser Statement
whileStatement =
    reserved "while" >>
    While <$> expression <*> block

returnStatement :: Parser Statement
returnStatement = reserved "return" >>
    ((Return <$> try expression) <|> return ReturnVoid)

parameterList :: Parser [Ident]
parameterList = parens $ sepBy identifier comma

funcDefinition :: Parser FuncDefinition
funcDefinition =
    reserved "fn" >>
    FuncDefinition <$> identifier <*> parameterList <*> block

classDeclaration :: Parser Statement
classDeclaration = do
    reserved "class"
    className <- identifier
    body <- braces classBody
    return $ ClassDeclaration className body

classBody :: Parser ClassBody
classBody = do
    fns1 <- many funcDefinition
    constructor <- constructorDefinition
    fns2 <- many funcDefinition
    return $ ClassBody constructor (fns1 ++ fns2)

constructorDefinition :: Parser ConstructorDefinition
constructorDefinition =
    reserved "init" >>
    ConstructorDefinition <$> parameterList <*> block
    

operators =   [ [Postfix (brackets expression >>= \e -> return $ \x -> BracketIndex x e)]
              , [Postfix (reservedOp "." >> identifier >>= \ident -> return $ \x -> DotIndex x ident)]
              , [Postfix (reservedOp "->" >> identifier >>= \ident -> return $ \x -> ArrowIndex x ident)]
              , [Postfix (callParameters >>= \params -> return $ \x -> FuncCall x params)]

              , [Infix  (reservedOp "^"   >> return Pow) AssocLeft]
              , [Prefix (reserved "not"   >> return Not)]
              , [Prefix (reservedOp "-"   >> return Neg)]
              , [Infix  (reservedOp "*"   >> return Product) AssocLeft,
                 Infix  (reservedOp "/"   >> return Division) AssocLeft]
              , [Infix  (reservedOp "+"   >> return Sum) AssocLeft,
                 Infix  (reservedOp "-"   >> return Subtract) AssocLeft]

              , [Infix  (reservedOp ".."  >> return Concat) AssocRight]

              , [Infix  (reservedOp ">"   >> return Greater) AssocLeft]
              , [Infix  (reservedOp "<"   >> return Smaller) AssocLeft]
              , [Infix  (reservedOp "=="  >> return Equals) AssocLeft]

              , [Infix  (reserved "and"   >> return And) AssocLeft,
                 Infix  (reserved "or"    >> return Or) AssocLeft]]

terms  =  parens expression
      <|> (reserved "null" >> return NullLiteral)
      <|> Var <$> identifier
      <|> FloatLiteral <$> try float
      <|> IntLiteral <$> integer
      <|> BoolLiteral <$> boolP
      <|> StringLiteral <$> stringLiteral
      <|> arrayLiteral
      <|> tableLiteral
      <|> lambdaLiteral

arrayLiteral :: Parser Expr
arrayLiteral = ArrayLiteral <$> brackets (expression `sepBy` comma)

tableLiteral :: Parser Expr
tableLiteral = braces $ TableLiteral <$> kvPair `sepBy` comma

lambdaLiteral :: Parser Expr
lambdaLiteral = reserved "lambda" >> LambdaLiteral <$> parameterList <*> block

kvPair :: Parser (Ident, Expr)
kvPair = do
    ident <- identifier
    reservedOp "="
    expr <- expression
    return (ident, expr)

boolP :: Parser Bool
boolP  =  (reserved "true" >> return True)
      <|> (reserved "false" >> return False)

callParameters :: Parser [Expr]
callParameters = parens $ expression `sepBy` comma

expression :: Parser Expr
expression = buildExpressionParser operators terms

parseString :: String -> Statement
parseString str =
    case parse zenScriptParser "" str of
        Left e  -> error $ show e
        Right r -> r

parseFile :: String -> IO Statement
parseFile file = do
    program  <- readFile file
    case parse zenScriptParser "" program of
        Left e  -> print e >> fail "parse error"
        Right r -> return r