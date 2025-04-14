module HelloBDWorld where

import Data.List (words, group, sort, sortBy)
import Data.Char (toLower)
import SWEng
import Control.Monad

-- usuwanie "non-ascii"
zastapNonAscii :: Char -> Char
zastapNonAscii ch = if elem ch ['a'..'z'] then ch else ' '

-- usuwanie StopWords
filtrujSW :: [String] -> [String]
filtrujSW slowa = filter (\w -> not (elem w swENG)) slowa

-- przeksztalcenie tekstu w dosc dobrze:l H oczyszczona
-- liste slow
oczyscTxt :: String -> [String]
oczyscTxt txt = 
  (filtrujSW . words)  (map (zastapNonAscii.toLower) txt)

-- wyznaczenie czestotliwosci wystepowania slow
zgrupujSlowa :: [String] -> [(String,Int)]
zgrupujSlowa ws = 
  map (\gr -> (head gr, length gr)) ( group ( sort ws))

-- wyznaczenie najczestszych slow
najczestsze :: Int -> [(String,Int)] -> [(String,Int)]
najczestsze ile lw = 
  take ile $
  sortBy (\(a,k) (b,l) -> compare l k) lw

-- FINAL : ksiazka -> lista najczestszych slow
najczestszeSlowa :: Int ->  String -> [(String,Int)]
najczestszeSlowa ile ksiazka = 
  najczestsze ile (zgrupujSlowa (oczyscTxt ksiazka))
 
{-- IO --}
 
wczytajKsiazke :: FilePath -> IO String
wczytajKsiazke file = readFile file

-- wK         :: String -> IO String
-- ns         :: String -> [(String,Int)]
-- fmap ns    :: IO String -> IO [(String,Int)] 
-- print      :: a -> IO ()
-- fmap print :: IO a -> IO (IO ())
-- join       :: IO (IO a)) -> IO(a)

main = let book = wczytajKsiazke "hamlet.txt" 
           ff = fmap (najczestszeSlowa 20 ) book in 
       join (fmap print ff)

main' = do
  book <- wczytajKsiazke "hamlet.txt"
  let fw = najczestszeSlowa 10 book
  print fw
