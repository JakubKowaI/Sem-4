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
-- mergeSort


-- zip
-- zipWith


-- myfoldr
-- myfoldl
