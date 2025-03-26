module W04 where

import Data.List

num' xs = foldl (+) 0 xs
product' xs = foldl (*) 1 xs
minimum' xs = foldl1 min xs

--foldl f (x:xs) = foldl f x xs
and' xs = foldl (&&) True xs
or' xs = foldl (||) False xs
concat' xxs = foldl (++) [] xxs