/* (25 pts) An employee is paid at the rate of $36.75 per hour for regular hours (up to 40 hours/week). 
Any overtime hours are paid at a rate of 1.5× the base hourly rate. Taxes/dues are deducted from the 
worker’s gross pay as follows: 6% for Social Security, 15% for federal income, 4% for state, 1% for 
Bethlehem local, and 20$ in union dues. If the worker has 2 or more dependents, an additional 40$ is 
deducted for health insurance. Write a program to read in the number of hours worked in a week and 
number of dependents from the console and output the worker’s gross pay, all withholding amounts 
individually, and the net take-home pay for the week. Assume hours are entered as real numbers. 
Outputs do not have to contain the ‘$’ sign, but have to be output with 2 digits of precision. Use 
appropriate variable/constant names and types and format your output neatly. Test your code with 
several inputs to make sure the calculations are correct. */

// Jack Hickey

#include <iostream>
#include <cmath>
using namespace std;

int main(){
    // First we define all our constants
    float hourly_rate = 36.75;
    float overtime_multiplier = 1.5;
    float regular_hours = 40.0;
    float overtime_rate = hourly_rate * overtime_multiplier;
    
    float social_security_rate = 0.06;
    float federal_income_rate = 0.15;
    float state_rate = 0.04;
    float local_rate = 0.01;

    float union_dues = 20.0;
    float health_insurance = 40.0;

    // Get user inputs for hours worked and dependents
    cout << "Please enter the number of hours worked this week: ";
    float hours_worked;
    cin >> hours_worked;
    cout << "Please enter the number of dependents: ";
    int dependents;
    cin >> dependents;

    // Calculate gross pay
    float gross_pay;
    gross_pay = hours_worked * hourly_rate;
    // include overtime if applicable
    if (hours_worked > regular_hours){ 
        gross_pay += ((hours_worked - 40) * overtime_rate);
    }

    // Calculate deductions
    float social_security = gross_pay * social_security_rate;
    float federal_income = gross_pay * federal_income_rate;
    float state = gross_pay * state_rate;
    float local = gross_pay * local_rate;
    float total_deductions = social_security + federal_income + state + local + union_dues;

    // include health insurance if applicable
    if (dependents >= 2){
        total_deductions += health_insurance;
    }

    // Calculate net pay
    float new_pay = gross_pay - total_deductions;

    // Output results
    cout << "Gross Pay: " << floorf(gross_pay * 100) / 100 << '\n';
    cout << "Deductions:\n";
    cout << "  Social Security: " << floorf(social_security * 100) / 100 << '\n';
    cout << "  Federal Income: " << floorf(federal_income * 100) / 100 << '\n';
    cout << "  State: " << floorf(state * 100) / 100 << '\n';
    cout << "  Local: " << floorf(local * 100) / 100 << '\n';
    cout << "  Union Dues: " << floorf(union_dues * 100) / 100 << '\n';
    if (dependents >= 2){
        cout << "  Health Insurance: " << floorf(health_insurance * 100) / 100 << '\n';
    }
    cout << "Net Take-Home Pay: " << floorf(new_pay * 100) / 100 << '\n';
}
