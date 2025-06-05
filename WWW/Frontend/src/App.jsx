import React, { useEffect, useState } from "react";
import { BrowserRouter as Router, Routes, Route, Link, useNavigate } from "react-router-dom";
import axios from "axios";

axios.defaults.withCredentials = true;

const API = "http://localhost:3001";

function Nav({ user, logout }) {
  return (
    <nav className="flex gap-4 bg-gray-200 p-4">
      <Link to="/">📚 Książki</Link>
      {user?.role === "admin" && <Link to="/users">👥 Użytkownicy</Link>}
      {user && <Link to="/profile">👤 Profil</Link>}
      {!user ? (
        <>
          <Link to="/login">🔐 Logowanie</Link>
          <Link to="/register">📝 Rejestracja</Link>
        </>
      ) : (
        <button onClick={logout}>🚪 Wyloguj</button>
      )}
    </nav>
  );
}

function LoginView({ setUser }) {
  const [email, setEmail] = useState("test@example.com");
  const [password, setPassword] = useState("test");
  const navigate = useNavigate();

  const login = async () => {
    try {
      const res = await axios.post(`${API}/login`, { email, password });
      setUser(res.data.token);
      navigate("/");
    } catch (err) {
      alert("Błąd logowania");
    }
  };

  return (
    <div className="p-4">
      <h2>Logowanie</h2>
      <input value={email} onChange={(e) => setEmail(e.target.value)} placeholder="Email" className="border m-2 p-1" />
      <input value={password} type="password" onChange={(e) => setPassword(e.target.value)} placeholder="Hasło" className="border m-2 p-1" />
      <button onClick={login} className="bg-blue-500 text-white px-3 py-1">Zaloguj</button>
    </div>
  );
}

function RegisterView() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");

  const register = async () => {
    try {
      await axios.post(`${API}/register`, { email, password });
      alert("Zarejestrowano!");
    } catch {
      alert("Błąd rejestracji");
    }
  };

  return (
    <div className="p-4">
      <h2>Rejestracja</h2>
      <input value={email} onChange={(e) => setEmail(e.target.value)} placeholder="Email" className="border m-2 p-1" />
      <input value={password} type="password" onChange={(e) => setPassword(e.target.value)} placeholder="Hasło" className="border m-2 p-1" />
      <button onClick={register} className="bg-green-500 text-white px-3 py-1">Zarejestruj</button>
    </div>
  );
}

function BooksView() {
  const [books, setBooks] = useState([]);
  const [title, setTitle] = useState("");

  const fetchBooks = async () => {
    try {
      const res = await axios.get(`${API}/books`);
      setBooks(res.data);
    } catch {
      alert("Błąd pobierania książek");
    }
  };

  const addBook = async () => {
    try {
      await axios.post(`${API}/books/add`, { title });
      setTitle("");
      fetchBooks();
    } catch {
      alert("Błąd dodawania książki");
    }
  };

  const deleteBook = async (id) => {
    try {
      await axios.delete(`${API}/books/${id}`);
      fetchBooks();
    } catch {
      alert("Błąd usuwania książki");
    }
  };

  useEffect(() => {
    fetchBooks();
  }, []);

  return (
    <div className="p-4">
      <h2>Lista książek</h2>
      <ul className="list-disc pl-6">
        {books.map((b) => (
          <li key={b._id} className="flex justify-between">
            {b.title}
            <button onClick={() => deleteBook(b._id)} className="text-red-500">🗑</button>
          </li>
        ))}
      </ul>
      <input value={title} onChange={(e) => setTitle(e.target.value)} placeholder="Nowa książka" className="border m-2 p-1" />
      <button onClick={addBook} className="bg-blue-500 text-white px-3 py-1">Dodaj</button>
    </div>
  );
}

function UsersView() {
  const [users, setUsers] = useState([]);

  const fetchUsers = async () => {
    try {
      const res = await axios.get(`${API}/users`);
      setUsers(res.data);
    } catch {
      alert("Błąd pobierania użytkowników");
    }
  };

  const promote = async (id) => {
    await axios.post(`${API}/users/${id}/promote`);
    fetchUsers();
  };

  const demote = async (id) => {
    await axios.post(`${API}/users/${id}/demote`);
    fetchUsers();
  };

  const remove = async (id) => {
    await axios.delete(`${API}/users/${id}`);
    fetchUsers();
  };

  useEffect(() => {
    fetchUsers();
  }, []);

  return (
    <div className="p-4">
      <h2>Lista użytkowników</h2>
      <ul className="list-disc pl-6">
        {users.map((u) => (
          <li key={u._id} className="flex justify-between items-center">
            <span>{u.email} ({u.role})</span>
            <div className="flex gap-2">
              <button onClick={() => promote(u._id)} className="text-blue-600">⬆️</button>
              <button onClick={() => demote(u._id)} className="text-yellow-600">⬇️</button>
              <button onClick={() => remove(u._id)} className="text-red-600">🗑</button>
            </div>
          </li>
        ))}
      </ul>
    </div>
  );
}

function ProfileView({ user }) {
  return (
    <div className="p-4">
      <h2>Profil</h2>
      {user ? (
        <div>Email: {user.email}<br />Rola: {user.role}</div>
      ) : (
        <p>Nie zalogowano</p>
      )}
    </div>
  );
}

function App() {
  const [user, setUser] = useState(null);

  const getProfile = async () => {
    try {
      const res = await axios.get(`${API}/users/me`);
      setUser(res.data);
    } catch {
      setUser(null);
    }
  };

  useEffect(() => {
    getProfile();
  }, []);

  const logout = () => {
    axios.post(`${API}/logout`).finally(() => setUser(null));
  };

  return (
    <Router>
      <Nav user={user} logout={logout} />
      <Routes>
        <Route path="/" element={<BooksView />} />
        <Route path="/login" element={<LoginView setUser={setUser} />} />
        <Route path="/register" element={<RegisterView />} />
        <Route path="/users" element={<UsersView />} />
        <Route path="/profile" element={<ProfileView user={user} />} />
      </Routes>
    </Router>
  );
}

export default App;
