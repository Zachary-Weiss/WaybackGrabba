import System.IO
import System.Environment

main :: IO ()
main = do
    (a:args) <- getArgs
    putStrLn (getDateV a)

-- Grabs the date out of a url starting with "https://web.archive.org/web/" in the format YYYYMMDD
getDate8 :: String -> String
getDate8 str = let
    aux :: String -> String -> String -> String
    aux (x:xs) [] out = if length out == 7 then (out ++ [x]) else aux xs [] (out ++ [x])
    aux (x:xs) (y:ys) out = aux xs ys out
    in aux str "https://web.archive.org/web/" []

-- Grabs the date out of a url starting with "https://web.archive.org/web/" in the format YYYYMMDDhhmm
getDate12 :: String -> String
getDate12 str = let
    aux :: String -> String -> String -> String
    aux (x:xs) [] out = if length out == 11 then (out ++ [x]) else aux xs [] (out ++ [x])
    aux (x:xs) (y:ys) out = aux xs ys out
    in aux str "https://web.archive.org/web/" []


-- Grabs the date out of a url starting with "https://web.archive.org/web/" in the format YYYYMMDDhhmmss, EXCEPT AS MANY TRAILING DIGITS CAN BE OMITTED AS NEEDED
-- The V stands for 'variable length'
getDateV :: String -> String
getDateV str = let
    aux :: String -> String -> String -> String
    aux [] [] out = out
    aux (x:xs) [] out = if length out == 13 then (out ++ [x]) else aux xs [] (out ++ [x])
    aux (x:xs) (y:ys) out = aux xs ys out
    in aux str "https://web.archive.org/web/" []