cd Projects/Sem-4/temp

echo "#Jakub Kowal 279706"

svn export https://repo.cs.pwr.edu.pl/279706-dir/mw1/L4/

cd L4

make

./IncGen 30 | ./BST

./RandGen 30 | ./BST

./IncGen 30 | ./RBT

./RandGen 30 | ./RBT

./IncGen 30 | ./ST

./RandGen 30 | ./ST

tmux capture-pane -p -S'-' -E'-' -J > log.txt   

ST - wysokosc w remove
ST - podstawienia w inser inc
RBT - wysokosc