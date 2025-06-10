const express = require('express');
const cors = require("cors");
var mysql = require('mysql');
const app = express();
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const cookieParser = require('cookie-parser');
const e = require('express');

app.use(cookieParser());

const secret = 'MySecretKey'; // move to env variable in production

// Włącz CORS i zezwól na frontend z portu 5173 (Vite)
app.use(cors({
    origin: "http://localhost:5173",
    credentials: true // jeśli używasz ciasteczek
}));

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

    jwt.verify(token, secret, { algorithms: 'HS512' }, (err, user) => {
        if (err) return res.status(403).json({ error: 'Cannot Authenticate' });
        req.user = user;
        next();
    });
}

function requireAdmin(req, res, next) {
    db.query('SELECT * FROM User WHERE id = ?', [req.user.id], (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error' });
        if (results.length === 0) return res.status(404).json({ message: 'User not found' });

        if (results[0].role === 'admin') {
            //console.log('Role is admin:', req.user.id);
            return next(); // Admin - continue
        } else {
            //console.log('Role is not admin:', req.user.id);
            return res.status(403).json({ message: 'Access denied. Admins only.' }); //  Not admin
        }
    });
}

app.get('/users', authenticateToken, requireAdmin, (req, res) => {
    // Get page and limit from query params, set defaults
    const page = parseInt(req.query.page) || 1;       // Default page 1
    const limit = parseInt(req.query.limit) || 10;    // Default 10 users per page
    const offset = (page - 1) * limit;

    // Count total users for pagination metadata
    db.query('SELECT COUNT(*) AS total FROM User', (err, countResult) => {
        if (err) return res.status(500).json({ error: 'Database error' });

        const totalItems = countResult[0].total;
        const totalPages = Math.ceil(totalItems / limit);
        const currentPage = page;

        // Fetch paginated users
        db.query(
            'SELECT id, email, role FROM User LIMIT ? OFFSET ?',
            [limit, offset],
            (err, results) => {
                if (err) return res.status(500).json({ error: 'Database error' });

                res.json({
                    data: results,
                    pagination: {
                        currentPage,
                        totalPages,
                        totalItems,
                        itemsPerPage: limit,
                        hasNextPage: currentPage < totalPages,
                        hasPreviousPage: currentPage > 1,
                        nextPage: currentPage < totalPages ? currentPage + 1 : null,
                        previousPage: currentPage > 1 ? currentPage - 1 : null
                    }
                });
            }
        );
    });
});

app.post('/users/:userId/promote', authenticateToken, requireAdmin, (req, res) => {
    const userId = req.params.userId;

    db.query('SELECT * FROM User WHERE id = ?', [userId], (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error' });
        if (results.length === 0) return res.status(404).json({ message: 'User not found' });
        if (results[0].role === 'admin') return res.status(400).json({ message: 'User already is an admin' });

        db.query('UPDATE User SET role = \'admin\' WHERE id = ?', [userId], (err) => {
            if (err) return res.status(500).json({ error: 'Database error' });

            res.status(201).json({ message: `User ${userId} promoted to admin` });
        });
    });
});

app.post('/users/:userId/demote', authenticateToken, requireAdmin, (req, res) => {
    const userId = req.params.userId;

    db.query('SELECT * FROM User WHERE id = ?', [userId], (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error' });
        if (results.length === 0) return res.status(404).json({ message: 'User not found' });
        if (results[0].role === 'user') return res.status(400).json({ message: 'User already is an user' });

        db.query('UPDATE User SET role = \'user\' WHERE id = ?', [userId], (err) => {
            if (err) return res.status(500).json({ error: 'Database error' });

            res.status(201).json({ message: `User ${userId} demoted to user` });
        });
    });
});

app.get('/users/:userId', authenticateToken, requireAdmin, (req, res) => {
    const userId = req.params.userId;

    db.query('SELECT id,email,role FROM User WHERE id = ?', [userId], (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error' });
        if (results.length === 0) return res.status(404).json({ message: 'User not found' });
        res.status(200).json(results);

    });
});

app.delete('/users/:userId', authenticateToken, requireAdmin, (req, res) => {
    const userId = req.params.userId;

    db.query('DELETE FROM User WHERE id = ?', [userId], (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error' });
        if (results.length === 0) return res.status(404).json({ message: 'User not found' });
        res.status(204).json({ message: 'User deleted' });

    });
});

app.get('/books', authenticateToken, (req, res) => {
    const { page = 1, limit = 10, ...filters } = req.query;
    const userId = req.user.id; // Get current user ID from token
    const offset = (page - 1) * limit;

    let countSql = `
        SELECT COUNT(*) as total
        FROM Book b
        LEFT JOIN Loan l ON b.id = l.bookId AND l.status = 'active'
    `;

    let sql = `
        SELECT
            b.*,
            l.userId as borrowedByUserId
        FROM Book b
        LEFT JOIN Loan l ON b.id = l.bookId AND l.status = 'active'
    `;

    let conditions = [];
    let values = [];

    for (let key in filters) {
        if (key !== 'page' && key !== 'limit') {
            conditions.push(`b.${key} = ?`);
            values.push(filters[key]);
        }
    }

    if (conditions.length > 0) {
        const whereClause = ' WHERE ' + conditions.join(' AND ');
        countSql += whereClause;
        sql += whereClause;
    }

    db.query(countSql, values, (err, countResults) => {
        if (err) {
            console.error('Count query error:', err);
            return res.status(500).send('Database error');
        }

        const totalItems = countResults[0].total;
        const totalPages = Math.ceil(totalItems / limit);
        const currentPage = parseInt(page);

        sql += ' LIMIT ? OFFSET ?';
        const queryValues = [...values, parseInt(limit), parseInt(offset)];

        db.query(sql, queryValues, (err, results) => {
            if (err) {
                console.error('Query error:', err);
                return res.status(500).send('Database error');
            }

            const processedResults = results.map(book => ({
                ...book,
                borrowedByCurrentUser: book.borrowedByUserId === userId,
                borrowedByUserId: book.borrowedByUserId || null
            }));

            res.json({
                data: processedResults,
                pagination: {
                    currentPage,
                    totalPages,
                    totalItems,
                    itemsPerPage: parseInt(limit),
                    hasNextPage: currentPage < totalPages,
                    hasPreviousPage: currentPage > 1,
                    nextPage: currentPage < totalPages ? currentPage + 1 : null,
                    previousPage: currentPage > 1 ? currentPage - 1 : null
                }
            });
        });
    });
});

app.post('/books/add', authenticateToken, requireAdmin, (req, res) => {
    const { title, author, isbn, publishedYear, genre } = req.body;

    if (!title || !author || !isbn || !publishedYear || !genre) {
        return res.status(400).json({ message: 'Missing details' });
    }

    db.query('INSERT INTO Book(title,author,isbn,publishedYear,genre) Values(?,?,?,?,?)', [title, author, isbn, publishedYear, genre], (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error' });
        res.status(201).json(results);

    });
});

app.delete('/books/:bookId', authenticateToken, requireAdmin, (req, res) => {
    const bookId = req.params.bookId;

    db.query('DELETE FROM Book WHERE id = ?', [bookId], (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error' });
        if (results.length === 0) return res.status(404).json({ message: 'Book not found' });
        res.status(204).json({ message: 'Book deleted' });
 
    });
});

app.post('/books/:bookId/return', authenticateToken, (req, res) => {
    const bookId = req.params.bookId;
    const userId = req.user.id;

    // First, get the role of the user
    db.query('SELECT role FROM User WHERE id = ?', [req.user.id], (err, results) => {
        if (err) return res.status(500).json({ error: 'Database error while checking role' });
        if (results.length === 0) return res.status(404).json({ message: 'User not found' });

        const isAdmin = results[0].role === 'admin';

        // Determine which user's loan to return
        let targetUserId = userId; // Default to current user

        // If admin and userId is provided in request body, use that instead
        if (isAdmin && req.body.userId) {
            targetUserId = req.body.userId;
        }

        // Use transaction for consistency
        db.beginTransaction((err) => {
            if (err) return res.status(500).json({ error: 'Transaction start failed' });

            // Step 1: Check if the book exists
            db.query('SELECT available FROM Book WHERE id = ?', [bookId], (err, results) => {
                if (err) {
                    return db.rollback(() => {
                        res.status(500).json({ error: 'Database error while checking book' });
                    });
                }

                if (results.length === 0) {
                    return db.rollback(() => {
                        res.status(404).json({ message: 'Book not found' });
                    });
                }

                const isAvailable = results[0].available;

                if (isAvailable) {
                    return db.rollback(() => {
                        res.status(400).json({ message: 'Book is already marked as available' });
                    });
                }

                // Step 2: Check if there's an active loan for this book by the target user
                let loanQuery, loanParams;

                if (isAdmin && !req.body.userId) {
                    // Admin without specific userId - return any active loan for this book
                    loanQuery = 'SELECT id, userId FROM Loan WHERE bookId = ? AND status = ? ORDER BY id DESC LIMIT 1';
                    loanParams = [bookId, 'active'];
                } else {
                    // Regular user or admin with specific userId
                    loanQuery = 'SELECT id, userId FROM Loan WHERE bookId = ? AND userId = ? AND status = ?';
                    loanParams = [bookId, targetUserId, 'active'];
                }

                db.query(loanQuery, loanParams, (err, loanResults) => {
                    if (err) {
                        return db.rollback(() => {
                            res.status(500).json({ error: 'Database error while checking loan' });
                        });
                    }

                    if (loanResults.length === 0) {
                        return db.rollback(() => {
                            res.status(404).json({ message: 'No active loan found for this book' });
                        });
                    }

                    const loanId = loanResults[0].id;
                    const loanUserId = loanResults[0].userId;

                    // Step 3: Update Book availability
                    db.query('UPDATE Book SET available = 1 WHERE id = ?', [bookId], (err) => {
                        if (err) {
                            return db.rollback(() => {
                                res.status(500).json({ error: 'Failed to update book availability' });
                            });
                        }

                        // Step 4: Update the specific loan record
                        db.query(
                            'UPDATE Loan SET status = ?, returnDate = NOW() WHERE id = ?',
                            ['returned', loanId],
                            (err, updateResults) => {
                                if (err) {
                                    return db.rollback(() => {
                                        res.status(500).json({ error: 'Failed to update loan record' });
                                    });
                                }

                                if (updateResults.affectedRows === 0) {
                                    return db.rollback(() => {
                                        res.status(404).json({ message: 'Failed to update loan status' });
                                    });
                                }

                                // Commit the transaction
                                db.commit((err) => {
                                    if (err) {
                                        return db.rollback(() => {
                                            res.status(500).json({ error: 'Transaction commit failed' });
                                        });
                                    }

                                    res.status(200).json({
                                        message: 'Book successfully returned',
                                        returnedBy: loanUserId
                                    });
                                });
                            }
                        );
                    });
                });
            });
        });
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

        let countSql = 'SELECT COUNT(*) as total FROM Loan';
        let sql = 'SELECT * FROM Loan';
        let conditions = [];
        let values = [];

        // If not admin or userId wasn't provided → limit to own loans
        if (!isAdmin) {
            conditions.push('userId = ?');
            values.push(req.user.id);
        }

        if (isAdmin && !filters.userId) {
            // Admin sees all loans when no userId filter is specified
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
            const whereClause = ' WHERE ' + conditions.join(' AND ');
            countSql += whereClause;
            sql += whereClause;
        }

        // Execute count query first
        db.query(countSql, values, (err, countResults) => {
            if (err) {
                console.error('Count query error:', err);
                return res.status(500).json({ error: 'Database error while counting loans' });
            }

            const totalItems = countResults[0].total;
            const totalPages = Math.ceil(totalItems / limit);
            const currentPage = parseInt(page);

            // Add pagination to main query
            sql += ' LIMIT ? OFFSET ?';
            const queryValues = [...values, parseInt(limit), parseInt(offset)];

            // Execute main query
            db.query(sql, queryValues, (err, results) => {
                if (err) {
                    console.error('Query error:', err);
                    return res.status(500).json({ error: 'Database error while fetching loans' });
                }

                // Return data with pagination metadata
                res.json({
                    data: results,
                    pagination: {
                        currentPage,
                        totalPages,
                        totalItems,
                        itemsPerPage: parseInt(limit),
                        hasNextPage: currentPage < totalPages,
                        hasPreviousPage: currentPage > 1,
                        nextPage: currentPage < totalPages ? currentPage + 1 : null,
                        previousPage: currentPage > 1 ? currentPage - 1 : null
                    }
                });
            });
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

        const token = jwt.sign(
            {
                id: user.id,
                email: user.email,
                role: user.role
            },
            secret,
            {
                algorithm: 'HS512',
                expiresIn: '1h'
            }
        );

        res.cookie('token', token, {
            httpOnly: true,
            secure: false, // true in production with HTTPS
            sameSite: 'Strict',
            maxAge: 24 * 60 * 60 * 1000 // This should match expiresIn (1h = 3600000ms)
        }).json({ token });;
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

    // Use a transaction to prevent race conditions
    db.beginTransaction((err) => {
        if (err) return res.status(500).json({ error: 'Transaction start failed' });

        // Step 1: Check if book exists and is available (with row lock)
        db.query('SELECT available FROM Book WHERE id = ? FOR UPDATE', [bookId], (err, results) => {
            if (err) {
                return db.rollback(() => {
                    res.status(500).json({ error: 'Database error while checking book' });
                });
            }

            if (results.length === 0) {
                return db.rollback(() => {
                    res.status(404).json({ message: 'Book not found' });
                });
            }

            if (results[0].available === 0) {
                return db.rollback(() => {
                    res.status(400).json({ message: 'Book already lent' });
                });
            }

            // Step 2: Check if user already has an active loan for this book
            db.query('SELECT id FROM Loan WHERE bookId = ? AND userId = ? AND status = ?',
                [bookId, userId, 'active'], (err, loanResults) => {
                    if (err) {
                        return db.rollback(() => {
                            res.status(500).json({ error: 'Database error while checking existing loans' });
                        });
                    }

                    if (loanResults.length > 0) {
                        return db.rollback(() => {
                            res.status(400).json({ message: 'User already has an active loan for this book' });
                        });
                    }

                    // Step 3: Update availability
                    db.query('UPDATE Book SET available = 0 WHERE id = ?', [bookId], (err) => {
                        if (err) {
                            return db.rollback(() => {
                                res.status(500).json({ error: 'Failed to update book availability' });
                            });
                        }

                        // Step 4: Insert loan
                        const insertQuery = `
                        INSERT INTO Loan (bookId, userId, dueDate, loanDate, status)
                        VALUES (?, ?, ?, NOW(), 'active')
                    `;
                        db.query(insertQuery, [bookId, userId, dueDateStr], (err) => {
                            if (err) {
                                return db.rollback(() => {
                                    res.status(500).json({ error: 'Failed to create loan record' });
                                });
                            }

                            // Commit the transaction
                            db.commit((err) => {
                                if (err) {
                                    return db.rollback(() => {
                                        res.status(500).json({ error: 'Transaction commit failed' });
                                    });
                                }

                                res.status(201).json({
                                    message: `Book ${bookId} lent to user ${userId}`,
                                    dueDate: dueDateStr
                                });
                            });
                        });
                    });
                });
        });
    });
});


var server = app.listen(3001, () => {
    var host = server.address().address;
    var port = server.address().port;
    console.log("Przykładowa aplikacja nasłuchuje na http://%s:%s", host, port);
})

const db = mysql.createConnection({
    host: "localhost",
    user: "kuba",
    password: "kubutek15",
    database: "API",
});

db.connect(function (err) {
    if (err) throw err;
    console.log("Connected!");
});

/*app.get('/form1.html', (req, res) => {
    res.sendFile( __dirname + "/" + "form1.html" );
    });*/




//app.use(express.static('public'));
/*app.get('/form2.html', (req, res) => {
    res.sendFile( __dirname + "/" + "form2.html" );
});*/

