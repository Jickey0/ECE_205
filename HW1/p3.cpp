// Jack Hickey

#include <iostream>
using namespace std;

int main(){
    float stero_cost { 1000 };
    float monthly_intrest_rate { 0.18 };
    float monthly_payment { 50 };

    float balance { stero_cost };
    float total_intrest { 0 };
    float intrest;

    int payment_count { 0 };

    while (balance > 0){
        // apply intrest
        intrest = balance * monthly_intrest_rate / 12;
        total_intrest += intrest;
        balance += intrest;

        // make payment
        balance -= monthly_payment;
        // cout << "Remaining balance: " << balance << '\n';

        payment_count ++;
    }

    // display results
    cout << "Total monthly payments: " << payment_count << '\n';
    cout << "Total intrest paid: " << total_intrest << '\n';
    cout << "Total cost of stereo: " << stero_cost + total_intrest << '\n';
}