module W03 where

import Data.List

{- SORTOWANIA -}
-- qS : lipna wersja
qS [] = []
qS (x:xs) = (qS [y| y<- xs, y<x]) ++
            [x] ++
            (qS [y| y<- xs, y>=x])
            
-- partition
{- komentuje, bo jest w Data.List
partition :: (a->Bool) -> [a] ->([a],[a])
partition _ [] = ([],[])
partition p (x:xs) = if p x then (x:l,r)
                            else (l,x:r)
                     where (l,r) = partition p xs
-}

-- sections 
p3 = (+ 3)
m4 = (4 *)

-- qSort
qSort []     = []
qSort [x]    = [x]
qSort (x:xs) = (qSort l) ++ [x] ++ (qSort r)
               where (l,r) = partition (<x) xs 
-- inSort

inSort [] = []
inSort [x] = [x]
inSort (x:xs) = left ++ [x] ++ right
                where sxs = inSort xs
                      (left,right) = partition (<x) sxs

{- ZIPY -}
-- uzywam ' bo zip i zipWith są w Prelude
zip' [] _ = []
zip' _ [] = []
zip' (x:xs) (y:ys) = (x,y): (zip' xs ys)

zipWith' _ [] _ = [] 
zipWith' _ _ [] = []
zipWith' f (x:xs) (y:ys) = (f x y): zipWith' f xs ys

-- zagadka 
addMat = zipWith (zipWith (+)) 

{-- FOLDY --}
-- uzywam myfold* bo foldy sa w Prelude
add [] = 0
add (x:xs) = x + add xs

prod [] = 1
prod (x:xs) = x * prod xs

myfoldr f e [] = e
myfoldr f e (x:xs) = f x (myfoldr f e xs)

myfoldl f e [] = e
myfoldl f e (x:xs) = foldl f (f e x) xs

-- reverse jest Data.List, flip jest w Prelude
myreverse :: [a] -> [a]
myreverse  = foldl (flip (:)) [] 