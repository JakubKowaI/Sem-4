module W04b where

{-- Typy numerowane --}
data DOW = Po|Wt|Sr|Cz|Pi|So|Ni deriving (Eq,Ord,Enum,Bounded)

instance Show DOW where
  show Po = "Poniedziałek"
  show Wt = "Wtorek"
  show Sr = "Środa"
  show Cz = "Czwartek"
  show Pi = "Piatek"
  show So = "Sobota"
  show Ni = "Niedziela"

dniPracujace = [Po .. Pi] -- efekt Enum