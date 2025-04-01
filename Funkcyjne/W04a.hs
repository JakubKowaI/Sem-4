module W04a where

import Data.List

sum'       xs  = foldl (+) 0 xs
product'   xs  = foldl (*) 1 xs 
minimum'   xs  = foldl1 min xs

-- fold1 f (x:xs) = foldl f x xs
and'       xs  = foldl (&&) True xs
or'        xs  = foldl (||) False xs
concat'    xxs = foldl (++) [] xxs
concatMap' f   = foldl ((++).f) []

{-- Automaty skonczone --}

-- AUTOMAT DETERMINISTYCZNY 
runDFA :: (s -> c -> s) -> s -> [c] -> s
--runDFA delta start cs = foldl delta start cs
runDFA = foldl

acceptDFA :: (s -> c -> s) -> s -> (s -> Bool) -> [c] -> Bool
acceptDFA delta start accept cs = accept (runDFA delta start cs)
 
-- przyklad: parzysta liczba jedynek

delta 1 '1' = 2
delta 1 _   = 1
delta 2 '1' = 1
delta 2 _   = 2
delta _ _   = 1  

-- AUTOMAT NIEDETRMINISTYCZNY 
-- procedure przekonwertowania automatu niedeterministyczngo
-- na automat deterministyczny
convertDelta :: (Eq s)=>(s->c->[s]) -> ([s]->c->[s]) 
convertDelta delta ss c  = nub (concat(map (\s -> delta s c) ss))
        
runNFA :: (Eq s) => (s->c->[s]) -> s -> [c] -> [s]
runNFA delta start cs = 
  runDFA deltaS [start] cs 
  where deltaS = convertDelta delta

convertAcc :: (s->Bool) -> ([s]->Bool)
convertAcc acc ss = or (map acc ss)

acceptNFA :: (Eq s) => (s->c->[s]) -> s -> (s->Bool) -> [c]->Bool
acceptNFA delta start acc cs = 
  accS (runNFA delta start cs)
  where accS = convertAcc acc
   
-- ciagi konczace sie jedynka
rho :: Int -> Char ->[Int]
rho 1 '1' = [1,2]
rho 1  _  = [1]
rho 2 '1' = [2]
rho 2   _ = [1]
           
{-- "Infinite streams" --}

myrepeat x = x:myrepeat x

myrepeat' x = xs where xs = x : xs

cycle' []  = error "cycle: emptyList"
cycle' xs  = xs' where xs' = xs ++ xs'

iterate' :: (a -> a) -> a -> [a]
iterate' f x =  x : iterate f (f x)

approSqrt:: Double->[Double]
approSqrt a = iterate (\x->(x+a/x)/2) a

-- liczby fibonacciego
fibb = 0:1: zipWith (+) fibb (tail fibb)

-- liczby pierwsze
sieve (p:xs) = p : sieve (filter (\n -> mod n p /= 0) xs)
primes = sieve [2..]

better_sieve (p:xs) = p: sieve (filter(\n -> n<p*p ||(mod n p /= 0)) xs)

addGF xs ys = zipWith (+) xs ys
--- Konstrukcja "type"
module Fizyka(Point,Vector,Time,
              move, translate, moveInGF) where

type Point = (Double,Double)
type Vector = (Double,Double)
type Time = Double

constG = 9.80655

-- lokalna funkcje
generalMove :: Point -> Vector -> Time -> Double -> Point
generalMove (x,y) (vx,vy) t acc = (x+vx*t, y + vy*t + acc*t*t/2)

-- upublicznione 
move :: Point -> Vector -> Time -> Point
move p v t = generalMove p v t 0 

translate :: Point -> Vector -> Point
translate p v = generalMove p v 1 0

moveInGF :: Point -> Vector -> Time -> Point
moveInGF p v t = generalMove p v t (-constG)