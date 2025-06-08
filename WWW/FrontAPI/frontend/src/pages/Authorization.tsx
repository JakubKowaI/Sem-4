import React, { useState, useEffect } from 'react';
import { BookOpen } from 'lucide-react';
import type { ApiCallFunction } from '../App';

interface ILoginForm {
    email: string;
    password: string;
}

interface IRegisterForm {
    email: string;
    password: string;
}

interface ITokenPayload {
    id: number;
    email: string;
    role: 'user' | 'admin';
    exp: number;
    iat: number;
}


export const Authorization: React.FC<{
    error: string
    success: String,
    setCurrentView: Function,
    currentView: String,
    loading: boolean,
    setLoading: Function,
    apiCall: ApiCallFunction,
    setUser: Function,
    showMessage: Function,
}> = ({
    error,
    success,
    setCurrentView,
    currentView,
    loading,
    setLoading,
    apiCall,
    setUser,
    showMessage,

}) => {
        const [loginForm, setLoginForm] = useState<ILoginForm>({ email: '', password: '' });
        const [registerForm, setRegisterForm] = useState<IRegisterForm>({ email: '', password: '' });

        const handleLoginInputChange = (field: keyof ILoginForm) => (e: React.ChangeEvent<HTMLInputElement>) => {
            setLoginForm({ ...loginForm, [field]: e.target.value });
        };

        const handleRegisterInputChange = (field: keyof IRegisterForm) => (e: React.ChangeEvent<HTMLInputElement>) => {
            setRegisterForm({ ...registerForm, [field]: e.target.value });
        };

        const login = async (e: React.FormEvent<HTMLFormElement>): Promise<void> => {
            e.preventDefault();
            setLoading(true);
            try {
                const response = await apiCall<{ token: string }>('/login', {
                    method: 'POST',
                    body: JSON.stringify(loginForm),
                });

                localStorage.setItem('token', response.token);
                const payload: ITokenPayload = JSON.parse(atob(response.token.split('.')[1]));
                console.log(payload);
                setUser({ id: payload.id, email: payload.email, role: payload.role });
                setCurrentView('books');
                showMessage('Login successful!');
                setLoginForm({ email: '', password: '' });
            } catch (error) {
                showMessage((error as Error).message || 'Login failed', 'error');
            }
            setLoading(false);
        };

        const register = async (e: React.FormEvent<HTMLFormElement>): Promise<void> => {
            e.preventDefault();
            setLoading(true);
            try {
                await apiCall('/register', {
                    method: 'POST',
                    body: JSON.stringify(registerForm),
                });
                showMessage('Registration successful! Please login.');
                setRegisterForm({ email: '', password: '' });
                setCurrentView('login');
            } catch (error) {
                showMessage((error as Error).message || 'Registration failed', 'error');
            }
            setLoading(false);
        };


        return (
            <div className="min-h-screen bg-gradient-to-br from-blue-50 to-indigo-100 flex items-center justify-center p-4">
                <div className="bg-white rounded-xl shadow-lg p-8 w-full max-w-md">
                    <div className="text-center mb-8">
                        <BookOpen className="mx-auto h-12 w-12 text-indigo-600 mb-4" />
                        <h1 className="text-3xl font-bold text-gray-900">Library System</h1>
                    </div>

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


                    <div className="text-sm font-medium text-center text-gray-500 border-b border-gray-200 dark:text-gray-400 dark:border-gray-700">
                        <ul className="flex flex-wrap -mb-px">
                            <li className="me-2">
                                <a onClick={() => setCurrentView('login')} className={`cursor-pointer inline-block p-4 ${currentView === 'login' && 'border-b-2'} rounded-t-lg hover:text-gray-600`}>Login</a>
                            </li>
                            <li className="me-2">
                                <a onClick={() => setCurrentView('register')} className={`cursor-pointer inline-block p-4 text-blue-600 ${currentView === 'register' && 'border-b-2'} border-blue-600 rounded-t-lg dark:text-blue-500 dark:border-blue-500`}>Register</a>
                            </li>
                        </ul>
                    </div>


                    {currentView === 'login' && (
                        <form onSubmit={login}>
                            <div className="mb-4">
                                <label className="block text-sm font-medium text-gray-700 mb-2">Email</label>
                                <input
                                    type="email"
                                    required
                                    value={loginForm.email}
                                    onChange={handleLoginInputChange('email')}
                                    className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-indigo-500 text-black"
                                />
                            </div>
                            <div className="mb-6">
                                <label className="block text-sm font-medium text-gray-700 mb-2">Password</label>
                                <input
                                    type="password"
                                    required
                                    value={loginForm.password}
                                    onChange={handleLoginInputChange('password')}
                                    className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-indigo-500 text-black"
                                />
                            </div>
                            <button
                                type="submit"
                                disabled={loading}
                                className="w-full bg-indigo-600 text-white py-2 px-4 rounded-md hover:bg-indigo-700 disabled:opacity-50 text-black"
                            >
                                {loading ? 'Logging in...' : 'Login'}
                            </button>
                        </form>
                    )}

                    {currentView === 'register' && (
                        <form onSubmit={register}>
                            <div className="mb-4">
                                <label className="block text-sm font-medium text-gray-700 mb-2 ">Email</label>
                                <input
                                    type="email"
                                    required
                                    value={registerForm.email}
                                    onChange={handleRegisterInputChange('email')}
                                    className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-indigo-500 text-black"
                                />
                            </div>
                            <div className="mb-6">
                                <label className="block text-sm font-medium text-gray-700 mb-2">Password</label>
                                <input
                                    type="password"
                                    required
                                    value={registerForm.password}
                                    onChange={handleRegisterInputChange('password')}
                                    className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-indigo-500 text-black"
                                />
                            </div>
                            <button
                                type="submit"
                                disabled={loading}
                                className="w-full bg-indigo-600 text-white py-2 px-4 rounded-md hover:bg-indigo-700 disabled:opacity-50"
                            >
                                {loading ? 'Registering...' : 'Register'}
                            </button>
                        </form>
                    )}
                </div>
            </div>
        );
    }