module Main where
import Parser
import Compiler
import System.Environment (getArgs)

version = "1.0.0"

main :: IO ()
main = do
    putStrLn $ "ZenScript compiler version " ++ version
    args <- getArgs
    let filename = head args
    let outputFile = if length args == 2 then args !! 1 else "test.lua"
    luaSource <- compileFile filename
    writeFile outputFile luaSource
    