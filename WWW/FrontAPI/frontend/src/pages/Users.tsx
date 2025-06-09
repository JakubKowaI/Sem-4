import React, { useState, useEffect } from 'react';
import type { IFilters, IUser, PaginatedResponse } from "../App"
import type { ApiCallFunction } from '../App';
import { UserCheck, Trash2, UserX } from 'lucide-react';
import ReactPaginate, { type ReactPaginateProps } from 'react-paginate';

export const UsersList: React.FC<{
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
        const [users, setUsers] = useState<IUser[]>([]);
        const [usersPageCount, setUsersPageCount] = useState(0);
        const [userFilters, setUserFilters] = useState<IFilters>({ page: 1, limit: itemsPerPage });


        useEffect(() => {
            if (user && currentView === 'users') fetchUsers();
        }, [user, currentView, userFilters]);


        const fetchUsers = async (): Promise<void> => {
            setLoading(true);
            try {
                const params = new URLSearchParams(
                    Object.entries(userFilters).reduce((acc, [key, value]) => {
                        acc[key] = value.toString();
                        return acc;
                    }, {} as Record<string, string>)
                );
                const response = await apiCall<PaginatedResponse<IUser>>(`/users?${params}`);
                setUsersPageCount(Math.ceil(response.pagination.totalItems / itemsPerPage));
                setUsers(response.data || []);
            } catch (error) {
                showMessage('Failed to fetch users', 'error');
            }
            setLoading(false);
        };

        const promoteUser = async (userId: number): Promise<void> => {
            try {
                await apiCall(`/users/${userId}/promote`, { method: 'POST' });
                showMessage('User promoted to admin!');
                fetchUsers();
            } catch (error) {
                showMessage((error as Error).message || 'Failed to promote user', 'error');
            }
        };

        const demoteUser = async (userId: number): Promise<void> => {
            try {
                await apiCall(`/users/${userId}/demote`, { method: 'POST' });
                showMessage('User demoted to regular user!');
                fetchUsers();
            } catch (error) {
                showMessage((error as Error).message || 'Failed to demote user', 'error');
            }
        };

        const deleteUser = async (userId: number): Promise<void> => {
            if (window.confirm('Are you sure you want to delete this user?')) {
                try {
                    await apiCall(`/users/${userId}`, { method: 'DELETE' });
                    showMessage('User deleted successfully!');
                    fetchUsers(); 
                } catch (error) {
                    showMessage((error as Error).message || 'Failed to delete user', 'error');
                }
            }
        };

        const handlePageUsersClick = (event: any) => {
            setUserFilters(userFilters => ({ ...userFilters, page: event.selected + 1 }))
            fetchUsers();
            const newOffset = (event.selected * itemsPerPage) % users.length;
            console.log(
                `User requested page number ${event.selected}, which is offset ${newOffset}`
            );
        };

        return <div className="px-4 sm:px-6 lg:px-8">
            <h2 className="text-xl sm:text-2xl font-bold text-gray-900 mb-6">User Management</h2>
            <div className="bg-white shadow overflow-hidden rounded-lg">
                <ul className="divide-y divide-gray-200">
                    {users.map((userItem: IUser) => (
                        <li key={userItem.id} className="p-4 sm:px-6 sm:py-4">
                            <div className="space-y-3 sm:space-y-0 sm:flex sm:items-center sm:justify-between">
                                <div className="flex-1 min-w-0">
                                    <h3 className="text-base sm:text-lg font-medium text-gray-900 break-all">{userItem.email}</h3>
                                    <p className="text-sm text-gray-500">ID: {userItem.id}</p>
                                    <div className="mt-1">
                                        <span className={`inline-block px-2 py-1 rounded-full text-xs font-medium ${userItem.role === 'admin' ? 'bg-purple-100 text-purple-800' : 'bg-gray-100 text-gray-800'
                                            }`}>
                                            {userItem.role}
                                        </span>
                                    </div>
                                </div>
                                <div className="flex flex-wrap gap-2 sm:flex-nowrap sm:ml-4">
                                    {userItem.role === 'user' ? (
                                        <button
                                            onClick={() => promoteUser(userItem.id)}
                                            className="flex-1 sm:flex-none bg-green-600 text-white px-3 py-2 rounded-md text-sm font-medium hover:bg-green-700 touch-manipulation min-h-[44px] flex items-center justify-center"
                                        >
                                            <UserCheck className="inline h-4 w-4 mr-1 sm:mr-2" />
                                            <span className="hidden sm:inline">Promote</span>
                                            <span className="sm:hidden">Promote</span>
                                        </button>
                                    ) : (
                                        <button
                                            onClick={() => demoteUser(userItem.id)}
                                            className="flex-1 sm:flex-none bg-yellow-600 text-white px-3 py-2 rounded-md text-sm font-medium hover:bg-yellow-700 touch-manipulation min-h-[44px] flex items-center justify-center"
                                        >
                                            <UserX className="inline h-4 w-4 mr-1 sm:mr-2" />
                                            <span className="hidden sm:inline">Demote</span>
                                            <span className="sm:hidden">Demote</span>
                                        </button>
                                    )}
                                    {userItem.id !== user.id && (
                                        <button
                                            onClick={() => deleteUser(userItem.id)}
                                            className="bg-red-600 text-white p-2 rounded-md hover:bg-red-700 touch-manipulation min-h-[44px] min-w-[44px] flex items-center justify-center"
                                            title="Delete user"
                                        >
                                            <Trash2 className="h-4 w-4" />
                                        </button>
                                    )}
                                </div>
                            </div>
                        </li>
                    ))}
                    <ReactPaginate
                        className='pagination'
                        breakLabel="..."
                        nextLabel=">"
                        onPageChange={handlePageUsersClick}
                        pageRangeDisplayed={5}
                        pageCount={usersPageCount}
                        previousLabel="<"
                        renderOnZeroPageCount={null}
                    />
                </ul>
            </div>
        </div>
    }