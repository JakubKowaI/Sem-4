MariaDB [API]> describe User;
+----------+----------------------+------+-----+---------+----------------+
| Field    | Type                 | Null | Key | Default | Extra          |
+----------+----------------------+------+-----+---------+----------------+
| id       | int(11)              | NO   | PRI | NULL    | auto_increment |
| email    | varchar(255)         | NO   | UNI | NULL    |                |
| password | varchar(255)         | NO   |     | NULL    |                |
| role     | enum('user','admin') | YES  |     | user    |                |
+----------+----------------------+------+-----+---------+----------------+
4 rows in set (0.001 sec)

MariaDB [API]> describe Book;
+---------------+--------------+------+-----+---------+----------------+
| Field         | Type         | Null | Key | Default | Extra          |
+---------------+--------------+------+-----+---------+----------------+
| id            | int(11)      | NO   | PRI | NULL    | auto_increment |
| title         | varchar(255) | NO   |     | NULL    |                |
| author        | varchar(255) | NO   |     | NULL    |                |
| isbn          | varchar(255) | NO   | UNI | NULL    |                |
| publishedYear | int(11)      | YES  |     | NULL    |                |
| genre         | varchar(255) | YES  |     | NULL    |                |
| available     | tinyint(1)   | YES  |     | 1       |                |
+---------------+--------------+------+-----+---------+----------------+
7 rows in set (0.001 sec)

MariaDB [API]> describe Loan;
+------------+-------------------------------------+------+-----+---------------------+----------------+
| Field      | Type                                | Null | Key | Default             | Extra          |
+------------+-------------------------------------+------+-----+---------------------+----------------+
| id         | int(11)                             | NO   | PRI | NULL                | auto_increment |
| loanDate   | datetime                            | YES  |     | current_timestamp() |                |
| dueDate    | datetime                            | NO   |     | NULL                |                |
| returnDate | datetime                            | YES  |     | NULL                |                |
| status     | enum('active','returned','overdue') | YES  |     | active              |                |
| bookId     | int(11)                             | NO   | MUL | NULL                |                |
| userId     | int(11)                             | NO   | MUL | NULL                |                |
+------------+-------------------------------------+------+-----+---------------------+----------------+
