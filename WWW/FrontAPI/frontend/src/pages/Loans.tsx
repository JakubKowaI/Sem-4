import React, { useState, useEffect } from 'react';
import type { IFilters, IUser, PaginatedResponse } from "../App"
import type { ApiCallFunction } from '../App';
import { Plus, Trash2 } from 'lucide-react';
import ReactPaginate, { type ReactPaginateProps } from 'react-paginate';

interface ILoan {
    id: number;
    bookId: number;
    userId: number;
    loanDate: string;
    dueDate: string;
    returnDate?: string;
    status: 'active' | 'returned' | 'overdue';
}
export const Loans: React.FC<{
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
        const [loans, setLoans] = useState<ILoan[]>([]);
        const [loansPageCount, setLoansPageCount] = useState(0);
        const [loanFilters, setLoanFilters] = useState<IFilters>({ page: 1, limit: itemsPerPage });

        useEffect(() => {
            if (user && currentView === 'loans') fetchLoans();
        }, [user, currentView, loanFilters]);

        const fetchLoans = async (): Promise<void> => {
            setLoading(true);
            try {
                const params = new URLSearchParams(
                    Object.entries(loanFilters).reduce((acc, [key, value]) => {
                        acc[key] = value.toString();
                        return acc;
                    }, {} as Record<string, string>)
                );
                const response = await apiCall<PaginatedResponse<ILoan>>(`/loans?${params}`);
                setLoansPageCount(Math.ceil(response.pagination.totalItems / itemsPerPage));
                setLoans(response.data);
            } catch (error) {
                showMessage('Failed to fetch loans', 'error');
            }
            setLoading(false);
        };


        const handlePageLoansClick = (event: any) => {
            setLoanFilters(loanFilters => ({ ...loanFilters, page: event.selected + 1 }))
            const newOffset = (event.selected * itemsPerPage) % loans.length;
            console.log(
                `User requested page number ${event.selected}, which is offset ${newOffset}`
            );
        };



        return <div className="px-4 sm:px-6 lg:px-8">
            <h2 className="text-xl sm:text-2xl font-bold text-gray-900 mb-6">Loan History</h2>
            <div className="bg-white shadow overflow-hidden rounded-lg">
                <ul className="divide-y divide-gray-200">
                    {loans.map((loan: ILoan) => (
                        <li key={loan.id} className="p-4 sm:px-6 sm:py-4">
                            <div className="space-y-2">
                                <div className="flex flex-wrap items-center gap-x-4 gap-y-1 text-sm text-gray-500">
                                    <span>Book ID: {loan.bookId}</span>
                                    <span>User ID: {loan.userId}</span>
                                </div>
                                <div className="space-y-2 sm:space-y-1">
                                    <div className="flex flex-wrap items-center gap-x-4 gap-y-1 text-xs sm:text-sm text-gray-500">
                                        <span>Loan: {new Date(loan.loanDate).toLocaleDateString()}</span>
                                        <span>Due: {new Date(loan.dueDate).toLocaleDateString()}</span>
                                        {loan.returnDate && (
                                            <span>Returned: {new Date(loan.returnDate).toLocaleDateString()}</span>
                                        )}
                                    </div>
                                    <div>
                                        <span className={`inline-block px-2 py-1 rounded-full text-xs font-medium ${loan.status === 'returned' ? 'bg-green-100 text-green-800' :
                                            loan.status === 'overdue' ? 'bg-red-100 text-red-800' :
                                                'bg-yellow-100 text-yellow-800'
                                            }`}>
                                            {loan.status}
                                        </span>
                                    </div>
                                </div>
                            </div>
                        </li>
                    ))}
                    <ReactPaginate
                        className='pagination'
                        breakLabel="..."
                        nextLabel=">"
                        onPageChange={handlePageLoansClick}
                        pageRangeDisplayed={5}
                        pageCount={loansPageCount}
                        previousLabel="<"
                        renderOnZeroPageCount={null}
                    />
                </ul>
            </div>
        </div>
    }