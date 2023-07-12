module String where

luaEscapeStr :: String -> String
luaEscapeStr = concatMap luaEscape

luaEscape :: Char -> String
luaEscape '"'  = "\\\""
luaEscape '\\' = "\\\\"
luaEscape s    = [s]