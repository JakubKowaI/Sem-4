const express = require('express');
var mysql = require('mysql');
const app = express();
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const cookieParser = require('cookie-parser');

app.use(cookieParser());

const secret = 'MySecretKey'; // move to env variable in production

// Wbudowany middleware do obsługi danych JSON i danych z formularzy urlencoded
app.use(express.urlencoded({ extended: false })); // Obsługuje 'application/x-www-form-urlencoded'
app.use(express.json()); // Obsługuje 'application/json'
//obsługa formularzy metodą GET
app.use(express.static('public'));

function authenticateToken(req, res, next) {
  var authHeader = req.headers['authorization'];
  var token = authHeader && authHeader.split(' ')[1];

  if (!token && req.cookies && req.cookies.token) {
    token = req.cookies.token;
  }

  if (!token) return res.sendStatus(401);

  jwt.verify(token, secret, (err, user) => {
    if (err) return res.sendStatus(403);
    req.user = user;
    next();
  });
}

function requireAdmin(req, res, next) {
  db.query('SELECT * FROM User WHERE id = ?', [req.user.id], (err, results) => {
    if (err) return res.status(500).json({ error: err });
    if (results.length === 0) return res.status(404).json({ message: 'User not found' });

    if (results[0].role === 'admin') {
      //console.log('Role is admin:', req.user.id);
      return next(); // ✅ Admin - continue
    } else {
      //console.log('Role is not admin:', req.user.id);
      return res.status(403).json({ message: 'Access denied. Admins only.' }); // ❌ Not admin
    }
  });
}


//odpowiedź Hello GET na żądanie GET do strony domowej
app.get('/', (req, res) => {
    console.log("Otrzymano żądanie GET dla strony głównej");
    res.send('Hello GET');
})
//odpowiedź Hello POST na żądanie POST do strony domowej
app.post('/', (req, res) => {
    console.log("Otrzymano żądanie POST dla strony głównej");
    res.send('Hello POST\n');
})
//odpowiedź Hello DELETE na żądanie DELETE do zasobu /users
app.delete('/users', (req, res) => {
    console.log("Otrzymano żądanie DELETE do zasobu /users");
    res.send('Hello DELETE\n');
})
//odpowiedź na żądanie GET do zasobu /user_list
app.get('/users', (req, res) => {
    console.log("Otrzymano żądanie GET do zasobu /users");
    res.send('Listing użytkowników');
})
//odpowiedź na żądanie GET do zasobu z uri o wzorcu /ab*cd
app.get('/ab*cd', (req, res) => {
    // wzorzec strony: abcd, abxcd, ab1cd, ...
    console.log("Otrzymano żądanie GET do zasobu z uri o wzorcu /ab*cd");
    res.send('Dopasowanie strony ze wzorcem ab*cd');
})

app.get('/users', authenticateToken, requireAdmin, (req, res) => {
  // Get page and limit from query params, set defaults
  const page = parseInt(req.query.page) || 1;       // Default page 1
  const limit = parseInt(req.query.limit) || 10;    // Default 10 users per page
  const offset = (page - 1) * limit;

  // Count total users for pagination metadata
  db.query('SELECT COUNT(*) AS total FROM User', (err, countResult) => {
    if (err) return res.status(500).json({ error: 'Database error' });
    const totalUsers = countResult[0].total;
    const totalPages = Math.ceil(totalUsers / limit);

    // Fetch paginated users
    db.query(
      'SELECT id, email, role FROM User LIMIT ? OFFSET ?',
      [limit, offset],
      (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error' });

        res.json({
          page,
          limit,
          totalUsers,
          totalPages,
          users: results
        });
      }
    );
  });
});

app.post('/users/:userId/promote', authenticateToken, requireAdmin, (req, res) => {
  const userId = req.params.userId;

db.query('SELECT * FROM User WHERE id = ?', [userId], (err, results) => {
  if (err) return res.status(500).json({ error: err });
  if (results.length === 0) return res.status(404).json({ message: 'User not found' });
  if (results[0].role === 'admin') return res.status(400).json({ message: 'User already is an admin' });

  db.query('UPDATE User SET role = \'admin\' WHERE id = ?', [userId], (err) => {
    if (err) return res.status(500).json({ error: err });

      res.status(201).json({ message: `User ${userId} promoted to admin`});
    });
  });
});

app.post('/users/:userId/demote', authenticateToken, requireAdmin, (req, res) => {
const userId = req.params.userId;

db.query('SELECT * FROM User WHERE id = ?', [userId], (err, results) => {
  if (err) return res.status(500).json({ error: err });
  if (results.length === 0) return res.status(404).json({ message: 'User not found' });
  if (results[0].role === 'user') return res.status(400).json({ message: 'User already is an user' });

  db.query('UPDATE User SET role = \'user\' WHERE id = ?', [userId], (err) => {
    if (err) return res.status(500).json({ error: err });

      res.status(201).json({ message: `User ${userId} demoted to user`});
    });
  });
});

app.get('/users/:userId', authenticateToken, requireAdmin, (req, res) => {
  const userId = req.params.userId;

db.query('SELECT id,email,role FROM User WHERE id = ?', [userId], (err, results) => {
  if (err) return res.status(500).json({ error: err });
  if (results.length === 0) return res.status(404).json({ message: 'User not found' });
  res.status(201).json(results);
  
  });
});

app.delete('/users/:userId/delete', authenticateToken, requireAdmin, (req, res) => {
  const userId = req.params.userId;

db.query('DELETE FROM User WHERE id = ?', [userId], (err, results) => {
  if (err) return res.status(500).json({ error: err });
  if (results.length === 0) return res.status(404).json({ message: 'User not found' });
  res.status(204).json(results);
  
  });
});


app.get('/books', (req, res) => {
  const { page = 1, limit = 10, ...filters } = req.query;

  const offset = (page - 1) * limit;
  let sql = 'SELECT * FROM Book';
  let conditions = [];
  let values = [];

  // Build WHERE conditions dynamically
  for (let key in filters) {
    conditions.push(`${key} = ?`);
    values.push(filters[key]);
  }

  if (conditions.length > 0) {
    sql += ' WHERE ' + conditions.join(' AND ');
  }

  sql += ' LIMIT ? OFFSET ?';
  values.push(parseInt(limit), parseInt(offset));

  db.query(sql, values, (err, results) => {
    if (err) {
      console.error('Query error:', err);
      return res.status(500).send('Database error');
    }
    res.json(results);
  });
});

app.post('/books/add', authenticateToken, requireAdmin, (req, res) => {
  const { title, author,isbn,publishedYear,genre } = req.body;

  if (!title || !author|| !isbn || !publishedYear || !genre) {
    return res.status(400).json({ message: 'Missing details' });
  }

db.query('INSERT INTO Book(title,author,isbn,publishedYear,genre) Values(?,?,?,?,?)', [title,author,isbn,publishedYear,genre], (err, results) => {
  if (err) return res.status(500).json({ error: err });
  res.status(201).json(results);
  
  });
});

app.delete('/books/:bookId/delete', authenticateToken, requireAdmin, (req, res) => {
  const bookId = req.params.bookId;

db.query('DELETE FROM Book WHERE id = ?', [bookId], (err, results) => {
  if (err) return res.status(500).json({ error: err });
  if (results.length === 0) return res.status(404).json({ message: 'Book not found' });
  res.status(204).json(results);
  });
});

app.get('/books/:bookId/return', authenticateToken, (req, res) => {
  const bookId = req.params.bookId;
  const userId = req.user.id;

  // First, get the role of the user
  db.query('SELECT role FROM User WHERE id = ?', [req.user.id], (err, results) => {
    if (err) return res.status(500).json({ error: 'Database error while checking role' });
    if (results.length === 0) return res.status(404).json({ message: 'User not found' });

    const isAdmin = results[0].role === 'admin';

    // If not admin or userId wasn't provided → limit to own loans
    if (!isAdmin) {
          // Step 1: Check if the book exists and is currently borrowed
      db.query('SELECT available FROM Book WHERE id = ?', [bookId], (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error' });
        if (results.length === 0) return res.status(404).json({ message: 'Book not found' });

        const isAvailable = results[0].available;

        if (isAvailable) {
          return res.status(400).json({ message: 'Book is already marked as returned' });
        }
        db.query('SELECT * FROM Loan WHERE userId = ? AND bookId = ? AND status = \'borrowed\'', [userId,bookId], (err) => {
                  if (err) return res.status(500).json({ error: 'Failed to update book availability' });

            // Step 2: Update Book availability and Loan status
          db.query('UPDATE Book SET available = true WHERE id = ?', [bookId], (err) => {
            if (err) return res.status(500).json({ error: 'Failed to update book availability' });

            // Update the most recent loan (assumes one active loan per book)
            db.query(
              `UPDATE Loan 
              SET status = 'returned', returnDate = CURDATE() 
              WHERE bookId = ? AND status = 'borrowed' 
              ORDER BY id DESC LIMIT 1`,
              [bookId],
              (err, loanResults) => {
                if (err) return res.status(500).json({ error: 'Failed to update loan record' });

                if (loanResults.affectedRows === 0) {
                  return res.status(404).json({ message: 'No active loan found for this book' });
                }

                return res.status(200).json({ message: 'Book successfully returned' });
              }
            );
          });
        });
      });
        }

    // If admin and userId is provided → use it
    if (isAdmin && filters.userId) {
      // Step 1: Check if the book exists and is currently borrowed
  db.query('SELECT available FROM Book WHERE id = ?', [bookId], (err, results) => {
    if (err) return res.status(500).json({ error: 'Database error' });
    if (results.length === 0) return res.status(404).json({ message: 'Book not found' });

    const isAvailable = results[0].available;

    if (isAvailable) {
      return res.status(400).json({ message: 'Book is already marked as returned' });
    }

    // Step 2: Update Book availability and Loan status
    db.query('UPDATE Book SET available = true WHERE id = ?', [bookId], (err) => {
      if (err) return res.status(500).json({ error: 'Failed to update book availability' });

      // Update the most recent loan (assumes one active loan per book)
      db.query(
        `UPDATE Loan 
         SET status = 'returned', returnDate = CURDATE() 
         WHERE bookId = ? AND status = 'borrowed' 
         ORDER BY id DESC LIMIT 1`,
        [bookId],
        (err, loanResults) => {
          if (err) return res.status(500).json({ error: 'Failed to update loan record' });

          if (loanResults.affectedRows === 0) {
            return res.status(404).json({ message: 'No active loan found for this book' });
          }

          return res.status(200).json({ message: 'Book successfully returned' });
        }
      );
    });
  });
    }
  });
});
app.get('/loans', authenticateToken, (req, res) => {
  const { page = 1, limit = 10, ...filters } = req.query;
  const offset = (page - 1) * limit;

  // First, get the role of the user
  db.query('SELECT role FROM User WHERE id = ?', [req.user.id], (err, results) => {
    if (err) return res.status(500).json({ error: 'Database error while checking role' });
    if (results.length === 0) return res.status(404).json({ message: 'User not found' });

    const isAdmin = results[0].role === 'admin';

    let sql = 'SELECT * FROM Loan';
    let conditions = [];
    let values = [];

    // If not admin or userId wasn't provided → limit to own loans
    if (!isAdmin) {
      conditions.push('userId = ?');
      values.push(req.user.id);
    }

    if (isAdmin && !filters.userId) {
      //conditions.push('userId = ?');
      //values.push(req.user.id);
    }

    // If admin and userId is provided → use it
    if (isAdmin && filters.userId) {
      conditions.push('userId = ?');
      values.push(filters.userId);
    }

    // Apply any other filters (e.g., bookId, status)
    for (let key in filters) {
      if (key !== 'userId') {
        conditions.push(`${key} = ?`);
        values.push(filters[key]);
      }
    }

    if (conditions.length > 0) {
      sql += ' WHERE ' + conditions.join(' AND ');
    }

    sql += ' LIMIT ? OFFSET ?';
    values.push(parseInt(limit), parseInt(offset));

    db.query(sql, values, (err, results) => {
      if (err) {
        console.error('Query error:', err);
        return res.status(500).send('Database error while fetching loans');
      }
      res.json(results);
    });
  });
});


app.post('/register', async (req, res) => {
  const { email, password } = req.body;

  if (!email || !password) {
    return res.status(400).json({ message: 'Email and password are required' });
  }

  const hashedPassword = await bcrypt.hash(password, 10);

  const sql = 'INSERT INTO User (email, password) VALUES (?, ?)';
  db.query(sql, [email, hashedPassword], (err) => {
    if (err) {
      console.error(err);
      return res.status(400).json({ message: 'User already exists or invalid data' });
    }

    res.status(201).json({ message: `User with email ${email} registered successfully` });
  });
});

app.post('/login', (req, res) => {
  const { email, password } = req.body;

  // var email;
  // var password;

  // if(!email){
  //   email=req.query.email;
  //   password=req.query.password;
  // }

  const sql = 'SELECT * FROM User WHERE email = ?';
  db.query(sql, [email], async (err, results) => {
    if (err || results.length === 0) return res.status(401).send('Invalid credentials');

    const user = results[0];
    const valid = await bcrypt.compare(password, user.password);

    if (!valid) return res.status(401).send('Invalid credentials');

    const token = jwt.sign({ id: user.id }, secret, { expiresIn: '1h' });

    res.cookie('token', token, {
      httpOnly: true,
      secure: false, // true in production with HTTPS
      sameSite: 'Strict',
      maxAge: 24 * 60 * 60 * 1000
    }).json({ message: 'Login successful' });
  });
});

app.post('/books/:bookId/lend', authenticateToken, (req, res) => {
  const bookId = req.params.bookId;
  const userId = req.user.id;

  // Calculate dueDate = one month from now
  const now = new Date();
  const dueDate = new Date(now);
  dueDate.setMonth(dueDate.getMonth() + 1);

  // Format dueDate as YYYY-MM-DD (MySQL DATE format)
  const dueDateStr = dueDate.toISOString().slice(0, 10);

  // Step 1: Check if book exists and is available
  db.query('SELECT available FROM Book WHERE id = ?', [bookId], (err, results) => {
    if (err) return res.status(500).json({ error: err });
    if (results.length === 0) return res.status(404).json({ message: 'Book not found' });
    if (results[0].available === 0) return res.status(400).json({ message: 'Book already lent' });

    // Step 2: Update availability
    db.query('UPDATE Book SET available = 0 WHERE id = ?', [bookId], (err) => {
      if (err) return res.status(500).json({ error: err });

      // Step 3: Insert loan
      const insertQuery = `
        INSERT INTO Loan (bookId, userId, dueDate, loanDate, status)
        VALUES (?, ?, ?, NOW(), 'active')
      `;
      db.query(insertQuery, [bookId, userId, dueDateStr], (err) => {
        if (err) return res.status(500).json({ error: err });

        res.status(201).json({ message: `Book ${bookId} lent to user ${userId}`, dueDate: dueDateStr });
      });
    });
  });
});



var server = app.listen(3001, () => {
    var host = server.address().address
    var port = server.address().port
    console.log("Przykładowa aplikacja nasłuchuje na http://%s:%s", host, port)
})

var db = mysql.createConnection({
  host: "localhost",
  user: "kuba",
  password: "kubutek15",
  database: "API",
});

db.connect(function(err) {
  if (err) throw err;
  console.log("Connected!");
//   db.query(sql, function (err, result) {
//   if (err) throw err;
//   console.log("Result: " + result);
//   });
});

/*app.get('/form1.html', (req, res) => {
    res.sendFile( __dirname + "/" + "form1.html" );
    });*/

app.get('/animals', (req, res) => {
    let response = {
        gromada : req.query.gromada,
        rodzaj : req.query.rodzaj,
        gatunek : req.query.gatunek
    };
    console.log(response);
    res.end(JSON.stringify(response));
})

//obsługa formularzy metodą POST - body-parser dla starszych wersji Express
//let bodyParser = require('body-parser');
//let urlencodedParser = bodyParser.urlencoded({ extended: false }) // application/x-www-form-urlencoded



//app.use(express.static('public'));
/*app.get('/form2.html', (req, res) => {
    res.sendFile( __dirname + "/" + "form2.html" );
});*/

//obsługa formularzy metodą POST

//linijka zamienna dla body-parser ze straszych wersji Express
//app.post('/animals', urlencodedParser, (req, res) => { 
app.post('/animals', (req, res) => {
let response = {
    gromada:req.body.gromada,
    rodzaj:req.body.rodzaj,
    gatunek:req.body.gatunek
};
console.log(response);
res.json(response);
})