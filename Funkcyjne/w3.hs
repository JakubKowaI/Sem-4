{-# OPTIONS_GHC -Wno-unrecognised-pragmas #-}
{-# HLINT ignore "Use foldr" #-}
module W03 where

-- import Data.List {transpose, partition, permutations}
-- import Data.Char

-- qS
qS [] = []
qS (x:xs) = (qS [y| y<- xs, y<x]) ++
    [x] ++
    (qS [y| y<- xs,y>=x])
-- partition
partition :: (a->Bool) -> [a] -> ([a],[a])
partition _ [] = ([],[])
partition p (x:xs) = if p x then (x:l, r)
                            else (l,x:r)
                    where (l,r) = partition p xs

-- sections
-- qSort
qSort [] =[]
qSort [x] = [x]
qSort (x:xs) = (qSort l) ++ [x] ++ (qSort r)
                where (l,r) = partition (<x) xs
-- inSort
inSort [] = []
inSort (x:xs) = l ++ [x] ++ r
                where sxs = inSort xs
                      (l,r) = partition (<x) sxs
-- mergeSort


-- zip
zip' [] _ = []
zip' _ [] = []
zip' (x:xs) (y:ys) = (x,y): (zip' xs ys)

-- zipWith


-- myfoldr
myfoldr op e [] = e
myfoldr op e (x:xs) = op x (myfoldr op e xs)

-- myfoldl
myfoldl op e [] = e
myfoldl op e (x:xs) = myfoldl op (op e x) xs

add [] = 0
add (x:xs) = x + add xs

pro [] = 1
pro (x:xs) = x * pro xs