import React, { useState, useEffect } from 'react';
import { User, Book, Users, LogOut, Plus, Edit, Trash2, Search, UserCheck, UserX, BookOpen, Calendar } from 'lucide-react';
import ReactPaginate, { type ReactPaginateProps } from 'react-paginate';
import { Authorization } from './pages/Authorization';
import { Books } from './pages/Books';
import { Loans } from './pages/Loans';
import { UsersList } from './pages/Users';

const API_BASE_URL = 'http://localhost:3001';
const itemsPerPage = 10;

interface PaginationMeta {
  currentPage: number;
  totalPages: number;
  totalItems: number;
  itemsPerPage: number;
  hasNextPage: boolean;
  hasPreviousPage: boolean;
  nextPage: number | null;
  previousPage: number | null;
}

// Generic paginated response interface
export interface PaginatedResponse<T> {
  data: T[];
  pagination: PaginationMeta;
}


// Interfaces
export interface IUser {
  id: number;
  email: string;
  role: 'user' | 'admin';
}







export interface IFilters {
  page: number;
  limit: number;
}

interface ITokenPayload {
  id: number;
  email: string;
  role: 'user' | 'admin';
  exp: number;
  iat: number;
}

interface IApiCallOptions {
  method?: 'GET' | 'POST' | 'PUT' | 'DELETE';
  body?: string;
  headers?: Record<string, string>;
  credentials?: RequestCredentials;
}
export type ApiCallFunction = <T = any>(
  url: string,
  options?: IApiCallOptions
) => Promise<T>;


type ViewType = 'login' | 'register' | 'books' | 'users' | 'loans';
type MessageType = 'success' | 'error';

const App: React.FC = () => {
  const [user, setUser] = useState<IUser | null>(null);
  const [currentView, setCurrentView] = useState<ViewType>('login');
  const [loading, setLoading] = useState<boolean>(false);
  const [showMobileMenu, setShowMobileMenu] = useState<boolean>(false);
  const [error, setError] = useState<string>('');
  const [success, setSuccess] = useState<string>('');

  const apiCall = async <T = any>(url: string, options: IApiCallOptions = {}): Promise<T> => {
    const token = localStorage.getItem('token');
    const defaultOptions: IApiCallOptions = {
      credentials: 'include',
      headers: {
        'Content-Type': 'application/json',
        ...(token && { Authorization: `Bearer ${token}` }),
      },
    };

    const response = await fetch(`${API_BASE_URL}${url}`, {
      ...defaultOptions,
      ...options,
      headers: {
        ...defaultOptions.headers,
        ...options.headers,
      },
    });

    if (!response.ok) {
      const errorData = await response.json().catch(() => ({ message: 'Request failed' }));
      throw new Error(errorData.message || `HTTP ${response.status}`);
    }

    return response.json();
  };

  useEffect(() => {
    const token = localStorage.getItem('token');
    if (token) {
      try {
        const payload: ITokenPayload = JSON.parse(atob(token.split('.')[1]));
        setUser({ id: payload.id, email: payload.email, role: payload.role });
        setCurrentView('books');
      } catch (error) {
        console.error('Invalid token:', error);
        logout();
      }
    }
  }, []);

  const showMessage = (message: string, type: MessageType = 'success'): void => {
    if (type === 'success') {
      setSuccess(message);
      setError('');
    } else {
      setError(message);
      setSuccess('');
    }
    setTimeout(() => {
      setSuccess('');
      setError('');
    }, 3000);
  };




  const logout = (): void => {
    localStorage.removeItem('token');
    setUser(null);
    setCurrentView('login');
    showMessage('Logged out successfully!');
  };

  if (!user) {
    return <Authorization
      error={error}
      success={success}
      setCurrentView={setCurrentView}
      currentView={currentView}
      loading={loading}
      setLoading={setLoading}
      apiCall={apiCall}
      setUser={setUser}
      showMessage={showMessage}
    />
  }

  return (
    <div className="min-h-screen bg-gray-50">
      {/* Responsive Navigation */}
      <nav className="bg-white shadow-sm border-b">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex justify-between h-16">
            <div className="flex items-center">
              <BookOpen className="h-8 w-8 text-indigo-600 mr-3" />
              <h4 className="md:text-xl  font-bold text-gray-900">Library Management</h4>
            </div>

            {/* Mobile menu button */}
            <div className="flex md:hidden items-center">
              <button
                onClick={() => setShowMobileMenu(!showMobileMenu)}
                className="text-gray-500 hover:text-gray-700 focus:outline-none"
              >
                <svg className="h-6 w-6" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  {showMobileMenu ? (
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                  ) : (
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 6h16M4 12h16M4 18h16" />
                  )}
                </svg>
              </button>
            </div>

            <div className="hidden md:flex items-center space-x-4">
              <button
                onClick={() => setCurrentView('books')}
                className={`px-3 py-2 rounded-md text-sm font-medium ${currentView === 'books' ? 'bg-indigo-100 text-indigo-700' : 'text-gray-500 hover:text-gray-700'}`}
              >
                <Book className="inline h-4 w-4 mr-1" />
                Books
              </button>

              <button
                onClick={() => setCurrentView('loans')}
                className={`px-3 py-2 rounded-md text-sm font-medium ${currentView === 'loans' ? 'bg-indigo-100 text-indigo-700' : 'text-gray-500 hover:text-gray-700'}`}
              >
                <Calendar className="inline h-4 w-4 mr-1" />
                Loans
              </button>

              {user.role === 'admin' && (
                <button
                  onClick={() => setCurrentView('users')}
                  className={`px-3 py-2 rounded-md text-sm font-medium ${currentView === 'users' ? 'bg-indigo-100 text-indigo-700' : 'text-gray-500 hover:text-gray-700'}`}
                >
                  <Users className="inline h-4 w-4 mr-1" />
                  Users
                </button>
              )}

              <button
                onClick={logout}
                className="px-3 py-2 rounded-md text-sm font-medium text-red-500 hover:text-red-700"
              >
                <LogOut className="inline h-4 w-4 mr-1" />
                Logout
              </button>
            </div>
          </div>
        </div>
        {showMobileMenu && (
          <div className="md:hidden bg-white border-t">
            <div className="pt-2 pb-3 space-y-1 px-2">
              <button
                onClick={() => { setCurrentView('books'); setShowMobileMenu(false); }}
                className={`w-full text-left block px-3 py-2 rounded-md text-base font-medium ${currentView === 'books' ? 'bg-indigo-50 text-indigo-700' : 'text-gray-600 hover:text-gray-800 hover:bg-gray-50'}`}
              >
                <Book className="inline h-4 w-4 mr-2" />
                Books
              </button>

              <button
                onClick={() => { setCurrentView('loans'); setShowMobileMenu(false); }}
                className={`w-full text-left block px-3 py-2 rounded-md text-base font-medium ${currentView === 'loans' ? 'bg-indigo-50 text-indigo-700' : 'text-gray-600 hover:text-gray-800 hover:bg-gray-50'}`}
              >
                <Calendar className="inline h-4 w-4 mr-2" />
                Loans
              </button>

              {user.role === 'admin' && (
                <button
                  onClick={() => { setCurrentView('users'); setShowMobileMenu(false); }}
                  className={`w-full text-left block px-3 py-2 rounded-md text-base font-medium ${currentView === 'users' ? 'bg-indigo-50 text-indigo-700' : 'text-gray-600 hover:text-gray-800 hover:bg-gray-50'}`}
                >
                  <Users className="inline h-4 w-4 mr-2" />
                  Users
                </button>
              )}

              <button
                onClick={() => { logout(); setShowMobileMenu(false); }}
                className="w-full text-left block px-3 py-2 rounded-md text-base font-medium text-red-600 hover:text-red-800 hover:bg-red-50"
              >
                <LogOut className="inline h-4 w-4 mr-2" />
                Logout
              </button>
            </div>
          </div>
        )}
      </nav>

      <main className="max-w-7xl mx-auto py-6 px-4 sm:px-6 lg:px-8">
        {error && (
          <div className="mb-4 p-3 bg-red-100 border border-red-400 text-red-700 rounded">
            {error}
          </div>
        )}

        {success && (
          <div className="mb-4 p-3 bg-green-100 border border-green-400 text-green-700 rounded">
            {success}
          </div>
        )}

        {currentView === 'books' && (
          <Books
            user={user}
            setLoading={setLoading}
            showMessage={showMessage}
            apiCall={apiCall}
            currentView={currentView}
            itemsPerPage={itemsPerPage}
            loading={loading}
          />
        )}

        {currentView === 'loans' && (
          <Loans
            user={user}
            setLoading={setLoading}
            showMessage={showMessage}
            apiCall={apiCall}
            currentView={currentView}
            itemsPerPage={itemsPerPage}
            loading={loading}
          />
        )}

        {currentView === 'users' && user.role === 'admin' && (
          <UsersList
            user={user}
            setLoading={setLoading}
            showMessage={showMessage}
            apiCall={apiCall}
            currentView={currentView}
            itemsPerPage={itemsPerPage}
            loading={loading}
          />
        )}
      </main>
    </div>
  );
};

export default App;