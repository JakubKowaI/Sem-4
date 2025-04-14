module W5b where

-- prototyp
data Osoba' = Osoba' String String String Int Int Int

imie' :: Osoba' -> String
imie' (Osoba' _ x _ _ _ _) = x
-- itd 

{-- RECORD SYNTAX --}

data Osoba = Osoba
     {
      idO :: String
     ,imie :: String
     ,nazwisko :: String
     ,rokUr :: Int
     ,miesiacUr :: Int
     ,dzienUr :: Int
     } deriving (Show,Eq)
  
aaa = Osoba "001" "Anna" "Nowicka" 2005 5 12

bbb = Osoba{idO = "002", 
            imie = "Jan", 
            nazwisko="Balicki", 
            rokUr=2004, miesiacUr = 10, dzienUr = 12}

-- lekkie ulepszenie   

data Date = Date{rok::Int,miesiac::Int,dzien::Int} 
     deriving(Eq)
     
instance Show Date where
  show (Date r m d) = 
    (show r) ++"."++(show m)++"."++(show d)
  
data Person = Person
     {
      _idO :: String
     ,_imie :: String
     ,_nazwisko :: String
     , dataUr :: Date
     } deriving (Show,Eq)
  
ccc = Person "001" "Anna" "Balicka" (Date 2005 5 12)

zmienDate :: Person -> Date -> Person
zmienDate osoba nowaData = 
  osoba{dataUr = nowaData}

zmienRokUr :: Person ->Int -> Person
zmienRokUr osoba rokUr = 
  let urodziny  = dataUr osoba 
      urodziny' = urodziny{rok = rokUr} in 
   osoba{dataUr = urodziny'}

-- to samo - inny zapis
zmienRokUr' osoba rokUr = 
  osoba{dataUr = (dataUr osoba){rok = rokUr}}

--- Pozniej: lenses 

{-- TYPY PARAMETRYZOWALNE --}

-- MODEL KATEGORYJNY: PP (X) = X x X

data Para a = Para (a,a) deriving (Show,Eq)

-- MODEL :: jesli f:X->Y to (fmap f):(X x X) -> (Y x Y)

instance Functor Para  where
  fmap f (Para (x,y)) = Para (f x, f y)

--pmap :: (a->b) -> Para a -> Para b
--pmap f (Para (x,y)) = Para (f x, f y)

f x = 3.5 * x*(1-x)
-- uzycie: fmap f Para(0.25,0.75)
                  
{-- FUNKTOR MAYBE --}

-- data Maybe a = Nothing |  Just a
-- MODEL: MB(X) = ({0} x {*}) u ({1} x X) 
-- fmap f Nothing = Nothing
-- fmap f (Just x) = Just (f x)

safeHead []    = Nothing
safeHead (x:_) = Just x

safeSqrt x 
  | x>=0      = Just (sqrt x)
  | otherwise = Nothing 

safeLog  x 
  | x>0  = Just (log x)
  | otherwise = Nothing

safeDiv x 0 = Nothing 
safeDiv x y = Just (x/y)

-- zaczynamy partyzantkę;
-- pozniej zrobimy to lepiej

-- sqrt(log x)
composeMB :: (a-> Maybe b) -> (b -> Maybe c) -> (a -> Maybe c)
composeMB f g x =
  case f x of 
   Nothing -> Nothing
   Just y  -> g y
 
expr1 = composeMB safeLog safeSqrt

-- (sqrt x)/log(x +1)
composeMB2 :: (a->b->Maybe c) -> Maybe a -> Maybe b -> Maybe c
composeMB2 _ Nothing _  = Nothing
composeMB2 _ _ Nothing  = Nothing
composeMB2 op (Just x) (Just y) = op x y             

expr2 x = let sn = safeSqrt x
              sd = safeLog (x^2-4) in
          composeMB2 safeDiv sn sd