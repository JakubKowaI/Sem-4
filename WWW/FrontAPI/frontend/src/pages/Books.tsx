import React, { useState, useEffect } from 'react';
import type { IFilters, IUser, PaginatedResponse } from "../App"
import type { ApiCallFunction } from '../App';
import { Plus, Trash2 } from 'lucide-react';
import ReactPaginate, { type ReactPaginateProps } from 'react-paginate';

interface IBookForm {
    title: string;
    author: string;
    isbn: string;
    publishedYear: string;
    genre: string;
}

interface IBook {
    id: number;
    title: string;
    author: string;
    isbn: string;
    publishedYear: number;
    genre: string;
    available: boolean;
    borrowedByCurrentUser?: boolean;
    borrowedByUserId?: number;
}

export const Books: React.FC<{
    user: IUser,
    setLoading: Function,
    showMessage: Function,
    apiCall: ApiCallFunction,
    currentView: string,
    itemsPerPage: number,
    loading: boolean
}> = ({
    user,
    setLoading,
    showMessage,
    apiCall,
    currentView,
    itemsPerPage,
    loading,

}) => {
        const [books, setBooks] = useState<IBook[]>([]);
        const [bookForm, setBookForm] = useState<IBookForm>({
            title: '',
            author: '',
            isbn: '',
            publishedYear: '',
            genre: ''
        });
        const [bookFilters, setBookFilters] = useState<IFilters>({ page: 1, limit: itemsPerPage });
        const [booksPageCount, setBooksPageCount] = useState(0);

        useEffect(() => {
            if (user && currentView === 'books') fetchBooks();
        }, [user, currentView, bookFilters]);

        const fetchBooks = async (): Promise<void> => {
            setLoading(true);
            try {
                const params = new URLSearchParams(
                    Object.entries(bookFilters).reduce((acc, [key, value]) => {
                        acc[key] = value.toString();
                        return acc;
                    }, {} as Record<string, string>)
                );
                const response = await apiCall<PaginatedResponse<IBook>>(`/books?${params}`);
                console.log(response);
                setBooksPageCount(Math.ceil(response.pagination.totalItems / itemsPerPage));
                setBooks(response.data);
            } catch (error) {
                showMessage('Failed to fetch books', 'error');
            }
            setLoading(false);
        };

        const addBook = async (e: React.FormEvent<HTMLFormElement>): Promise<void> => {
            e.preventDefault();
            setLoading(true);
            try {
                await apiCall('/books/add', {
                    method: 'POST',
                    body: JSON.stringify(bookForm),
                });
                showMessage('Book added successfully!');
                setBookForm({ title: '', author: '', isbn: '', publishedYear: '', genre: '' });
                fetchBooks();
            } catch (error) {
                showMessage((error as Error).message || 'Failed to add book', 'error');
            }
            setLoading(false);
        };

        const deleteBook = async (bookId: number): Promise<void> => {
            if (window.confirm('Are you sure you want to delete this book?')) {
                try {
                    await apiCall(`/books/${bookId}`, { method: 'DELETE' });
                    showMessage('Book deleted successfully!');
                    fetchBooks();
                } catch (error) {
                    showMessage((error as Error).message || 'Failed to delete book', 'error');
                }
            }
        };

        const lendBook = async (bookId: number): Promise<void> => {
            try {
                await apiCall(`/books/${bookId}/lend`, { method: 'POST' });
                showMessage('Book lent successfully!');
                fetchBooks();
            } catch (error) {
                showMessage((error as Error).message || 'Failed to lend book', 'error');
            }
        };

        const returnBook = async (bookId: number): Promise<void> => {
            try {
                await apiCall(`/books/${bookId}/return`, { method: 'POST' });
                showMessage('Book returned successfully!');
                fetchBooks();
            } catch (error) {
                showMessage((error as Error).message || 'Failed to return book', 'error');
            }
        };

        const handleBookInputChange = (field: keyof IBookForm) => (e: React.ChangeEvent<HTMLInputElement>) => {
            setBookForm({ ...bookForm, [field]: e.target.value });
        };

        const handlePageBookClick = (event: any) => {
            setBookFilters(bookFilters => ({ ...bookFilters, page: event.selected + 1 }))
            fetchBooks();
            const newOffset = (event.selected * itemsPerPage) % books.length;
            console.log(
                `User requested page number ${event.selected}, which is offset ${newOffset}`
            );
        };

        return <div className="px-4 sm:px-6 lg:px-8">
            <div className="mb-6">
                <h2 className="text-xl sm:text-2xl font-bold text-gray-900 mb-4">Books</h2>

                {user.role === 'admin' && (
                    <div className="bg-white p-4 sm:p-6 rounded-lg shadow mb-6">
                        <h3 className="text-base sm:text-lg font-medium text-gray-900 mb-4">Add New Book</h3>
                        <form onSubmit={addBook}>
                            <div className="space-y-4 sm:space-y-0 sm:grid sm:grid-cols-2 lg:grid-cols-5 sm:gap-4">
                                <input
                                    type="text"
                                    placeholder="Title"
                                    required
                                    value={bookForm.title}
                                    onChange={handleBookInputChange('title')}
                                    className="w-full px-3 py-3 sm:py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-indigo-500 text-base"
                                />
                                <input
                                    type="text"
                                    placeholder="Author"
                                    required
                                    value={bookForm.author}
                                    onChange={handleBookInputChange('author')}
                                    className="w-full px-3 py-3 sm:py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-indigo-500 text-base"
                                />
                                <input
                                    type="text"
                                    placeholder="ISBN"
                                    required
                                    value={bookForm.isbn}
                                    onChange={handleBookInputChange('isbn')}
                                    className="w-full px-3 py-3 sm:py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-indigo-500 text-base"
                                />
                                <input
                                    type="number"
                                    placeholder="Year"
                                    required
                                    value={bookForm.publishedYear}
                                    onChange={handleBookInputChange('publishedYear')}
                                    className="w-full px-3 py-3 sm:py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-indigo-500 text-base"
                                />
                                <input
                                    type="text"
                                    placeholder="Genre"
                                    required
                                    value={bookForm.genre}
                                    onChange={handleBookInputChange('genre')}
                                    className="w-full px-3 py-3 sm:py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-indigo-500 text-base"
                                />
                                <button
                                    type="submit"
                                    disabled={loading}
                                    className="w-full sm:col-span-2 lg:col-span-5 bg-indigo-600 text-white py-3 sm:py-2 px-4 rounded-md hover:bg-indigo-700 disabled:opacity-50 font-medium touch-manipulation"
                                >
                                    <Plus className="inline h-4 w-4 mr-2" />
                                    Add Book
                                </button>
                            </div>
                        </form>
                    </div>
                )}

                <div className="bg-white shadow overflow-hidden rounded-lg">
                    <ul className="divide-y divide-gray-200">
                        {books.map((book: IBook) => (
                            <li key={book.id} className="p-4 sm:px-6 sm:py-4">
                                <div className="space-y-3 sm:space-y-0 sm:flex sm:items-center sm:justify-between">
                                    <div className="flex-1 min-w-0">
                                        <h3 className="text-base sm:text-lg font-medium text-gray-900 truncate">{book.title}</h3>
                                        <p className="text-sm text-gray-500 truncate">by {book.author}</p>
                                        <div className="mt-2 space-y-2 sm:space-y-1">
                                            <div className="flex flex-wrap gap-x-4 gap-y-1 text-xs sm:text-sm text-gray-500">
                                                <span className="break-all">ISBN: {book.isbn}</span>
                                                <span>Year: {book.publishedYear}</span>
                                                <span className="break-all">Genre: {book.genre}</span>
                                            </div>
                                            <div className="flex flex-wrap items-center gap-x-4 gap-y-1 text-xs sm:text-sm text-gray-500">
                                                <span>ID: {book.id}</span>
                                                <span className={`px-2 py-1 rounded-full text-xs font-medium ${book.available
                                                    ? 'bg-green-100 text-green-800'
                                                    : book.borrowedByCurrentUser
                                                        ? 'bg-blue-100 text-blue-800'
                                                        : 'bg-red-100 text-red-800'
                                                    }`}>
                                                    {book.available
                                                        ? 'Available'
                                                        : book.borrowedByCurrentUser
                                                            ? 'Borrowed by You'
                                                            : 'Not Available'
                                                    }
                                                </span>
                                            </div>
                                        </div>
                                    </div>
                                    <div className="flex flex-wrap gap-2 sm:flex-nowrap sm:ml-4">
                                        {book.available ? (
                                            <button
                                                onClick={() => lendBook(book.id)}
                                                className="flex-1 sm:flex-none bg-blue-600 text-white px-4 py-2 rounded-md text-sm font-medium hover:bg-blue-700 touch-manipulation min-h-[44px] flex items-center justify-center"
                                            >
                                                Lend
                                            </button>
                                        ) : book.borrowedByCurrentUser ? (
                                            <button
                                                onClick={() => returnBook(book.id)}
                                                className="flex-1 sm:flex-none bg-green-600 text-white px-4 py-2 rounded-md text-sm font-medium hover:bg-green-700 touch-manipulation min-h-[44px] flex items-center justify-center"
                                            >
                                                Return
                                            </button>
                                        ) : (
                                            <button
                                                disabled
                                                className="flex-1 sm:flex-none bg-gray-400 text-white px-4 py-2 rounded-md text-sm font-medium cursor-not-allowed min-h-[44px] flex items-center justify-center"
                                            >
                                                Borrowed by Other
                                            </button>
                                        )}
                                        {user.role === 'admin' && (
                                            <>
                                                {!book.available && !book.borrowedByCurrentUser && (
                                                    <button
                                                        onClick={() => returnBook(book.id)}
                                                        className="flex-1 sm:flex-none bg-orange-600 text-white px-3 py-2 rounded-md text-sm font-medium hover:bg-orange-700 touch-manipulation min-h-[44px] flex items-center justify-center"
                                                        title="Admin return for other user"
                                                    >
                                                        <span className="hidden sm:inline">Admin Return</span>
                                                        <span className="sm:hidden">Return</span>
                                                    </button>
                                                )}
                                                <button
                                                    onClick={() => deleteBook(book.id)}
                                                    className="bg-red-600 text-white p-2 rounded-md hover:bg-red-700 touch-manipulation min-h-[44px] min-w-[44px] flex items-center justify-center"
                                                    title="Delete book"
                                                >
                                                    <Trash2 className="h-4 w-4" />
                                                </button>
                                            </>
                                        )}
                                    </div>
                                </div>
                            </li>
                        ))}
                        <ReactPaginate
                            className='pagination'
                            breakLabel="..."
                            nextLabel=">"
                            onPageChange={handlePageBookClick}
                            pageRangeDisplayed={5}
                            pageCount={booksPageCount}
                            previousLabel="<"
                            renderOnZeroPageCount={null}
                        />
                    </ul>
                </div>
            </div>
        </div>
    }