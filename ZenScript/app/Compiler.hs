module Compiler where

import Parser
import String
import Data.List
import Data.String
import Data.Functor.Contravariant


compile :: Statement -> String
compile stmt = case stmt of
    Seq []              -> ""
    Seq stmts           -> compile (head stmts) ++ "\n" ++ compile (Seq $ drop 1 stmts)
    Local ident expr    -> "local " ++ ident ++ " = " ++ compileExpr expr
    Assign e1 e2        -> compileExpr e1  ++ " = " ++ compileExpr e2
    If expr stmt        -> "if " ++ compileExpr expr ++ " then\n"
                        ++ compile stmt ++ "\n"
                        ++ "end"
    IfElse expr s1 s2   -> compileIfElse stmt
    While expr stmt     -> "while " ++ compileExpr expr ++ " do\n"
                        ++ compile stmt ++ "\n"
                        ++ "end"
    FuncDeclaration (FuncDefinition ident params stmt)
                        -> "local function " ++ ident ++ "(" ++ intercalate ", " params ++ ")\n"
                        ++ compile stmt ++ "\n"
                        ++ "end"
    Return expr         -> "return " ++ compileExpr expr
    ReturnVoid          -> "return"
    ClassDeclaration name body
                        -> compileClass name body
    Block s             -> "do\n"
                        ++ compile s ++ "\n"
                        ++ "end\n"
    Expression expr     -> compileExpr expr
    LuaCode code        -> code

compileIfElse :: Statement -> String
compileIfElse (IfElse e1 s1 s2)
    = compileIf e1 s1 ++ compileElse s2 ++ "end\n"
    where
        compileIf expr stmt = "if " ++ compileExpr expr ++ " then\n" ++ compile stmt ++ "\n"
        compileElse stmt    = case stmt of
            If e2 s3        -> "elseif " ++ compileExpr e2 ++ " then\n"
                            ++ compile s3 ++ "\n"
            IfElse e2 s3 s4 -> "elseif " ++ compileExpr e2 ++ " then\n"
                            ++ compile s3 ++ "\n"
                            ++ compileElse s4
            _               -> "else\n"
                            ++ compile stmt ++ "\n"

compileClass :: Ident -> ClassBody -> String
compileClass name (ClassBody ctor methods) =
    "local " ++ name ++ "\n" ++
    "do\n" ++
    "local __methods = {\n" ++
        intercalate ",\n" (map compileMethod methods) ++ "\n" ++
    "}\n" ++
    "local __class = setmetatable({\n" ++
        "__ctor = function(" ++ compileMethodParams ctorParams ++")\n" ++
        compile ctorStmt ++ "\n" ++
        "end,\n" ++
        "__className = " ++ name ++ "\n" ++
    " }, {\n" ++
        "__call = function(cls, ...)\n" ++
            "__self = setmetatable({}, {__index = __methods})\n" ++
            "cls.__ctor(__self, ...)\n" ++
            "return __self\n" ++
        "end\n" ++
    "})\n" ++
    name ++ " = __class\n" ++
    "end\n"
    where (ConstructorDefinition ctorParams ctorStmt) = ctor

compileMethodParams :: [Ident] -> String
compileMethodParams [] = "self"
compileMethodParams ps = "self, " ++ intercalate ", " ps

compileMethod :: FuncDefinition -> String
compileMethod (FuncDefinition name params stmt) =
    name ++ " = function(" ++ compileMethodParams params ++ ")\n" ++
        compile stmt ++ "\n" ++
    "end\n"

putParens :: String -> String
putParens s = "(" ++ s ++ ")"

compileExpr :: Expr -> String
compileExpr expr = case expr of
    Var ident           -> ident
    IntLiteral int      -> show int
    FloatLiteral n      -> show n
    BoolLiteral True    -> "true"
    BoolLiteral False   -> "false"
    StringLiteral s     -> "\"" ++ luaEscapeStr s ++ "\""
    ArrayLiteral exprs  -> "{" ++ intercalate ", " (map compileExpr exprs) ++ "}"
    TableLiteral kvs    -> "{" ++ intercalate ", " (map compileKv kvs) ++ "}"
                        where compileKv (ident, expr) = ident ++ " = " ++ compileExpr expr
    NullLiteral         -> "nil"
    LambdaLiteral params stmt
                        -> "function " ++ "(" ++ intercalate ", " params ++ ")\n"
                        ++ compile stmt ++ "\n"
                        ++ "end\n"
    FuncCall name exprs -> compileExpr name ++ "(" ++ intercalate ", " (map compileExpr exprs) ++ ")"
    Sum e1 e2           -> compileExpr e1 ++ " + " ++ compileExpr e2
    Subtract e1 e2      -> compileExpr e1 ++ " - " ++ compileExpr e2
    Product e1 e2       -> putParens (compileExpr e1) ++ " * " ++ putParens (compileExpr e2)
    Division e1 e2      -> putParens $ compileExpr e1 ++ " / " ++ compileExpr e2
    Pow e1 e2           -> compileExpr e1 ++ " ^ " ++ compileExpr e2
    Neg e1              -> "-" ++ putParens (compileExpr e1)
    Not e1              -> "not " ++ putParens (compileExpr e1)
    And e1 e2           -> putParens (compileExpr e1) ++ " and "  ++ putParens (compileExpr e2)
    Or e1 e2            -> compileExpr e1 ++ " or " ++ compileExpr e2
    Concat e1 e2        -> putParens (compileExpr e1) ++ " .. " ++ putParens (compileExpr e2)
    Greater e1 e2       -> compileExpr e1 ++ " > " ++ compileExpr e2
    Smaller e1 e2       -> compileExpr e1 ++ " < " ++ compileExpr e2
    Equals e1 e2        -> compileExpr e1 ++ "==" ++ compileExpr e2
    BracketIndex e1 e2  -> case e1 of
        Var _           -> compileExpr e1 ++ "[" ++ compileExpr e2 ++ "]"
        FuncCall _ _    -> compileExpr e1 ++ "[" ++ compileExpr e2 ++ "]"
        _               -> putParens (compileExpr e1) ++ "[" ++ compileExpr e2 ++ "]"
    DotIndex expr ident -> compileExpr expr ++ "." ++ ident
    ArrowIndex expr ide -> compileExpr expr ++ ":" ++ ide

compileString :: String -> String
compileString = compile . parseString

compileFile :: String -> IO String
compileFile filename = do
    ast <- parseFile filename
    return $ compile ast