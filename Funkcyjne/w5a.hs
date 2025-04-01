import Test.QuickCheck

imie' :: Osoba' -> String
imie' (Osoba' _ x _ _ _ _) = x

data Osoba = Osoba{
    idO :: String
    ,imie :: String
    ,nazwisko :: String
    ,rokUr :: Int
    ,miesiacUr :: Int
    ,dzienUr :: Int
} deriving(Show,Eq)

aaa = Osoba "001" "Anna" "Nowicka" 2005 5 12

bbb = Osoba{
    idO="002",
    imie = "Jan",
    nazwisko = "Konieczny",
    rokUr = 2004,
    miesiacUr = 10,
    dzienUr= 12}

-- mozna modyfikowac w ten sposob bbb{rokUr=2010, dzienUr=1}

data Date = Date{rok::Int,
    miesiac::Int,
    dzien::Int
}deriving(Eq)

instance Show Date where
    show (Date r m d) =
        (show r) ++ "." ++(show m) ++ "."++show d


ccc = Person "001" "Anna" "Balicka" (Date 2005 5 12)

