#include <stdio.h>
#include "PayrollAbstract.h"

int main(int argc, char *argv[]) {
	double income, rpp, union_dues, cpp_ytd, ei_ytd, federal_credits, provincial_credits, lssp, ad;
	int pay_periods, dependants;
	
	// nhap thong tin
	printf(" Please enter the employee's pay period information :\n");
	printf("  Income for the current pay period     : ");
	scanf("%lf", &income);
	printf("  Registered Pension Plan contributions : ");
	scanf("%lf", &rpp);
	printf("  Union dues for the current pay period : ");
	scanf("%lf", &union_dues);
	printf(" Please enter the employee's annual information :\n");
	printf("  Number of pay periods this year       : ");
	scanf("%d", &pay_periods);
	printf("  Federal tax credits (as per TD1 form) : ");
	scanf("%lf", &federal_credits);
	printf("  Ontario tax credits (as per TD1 form) : ");
	scanf("%lf", &provincial_credits);
	printf("  Number of Dependants < 18 years old   : ");
	scanf("%d", &dependants);
	printf("  Labour-sponsored share purchases      : ");
	scanf("%lf", &lssp);
	printf("  Authorized deductions                 : ");
	scanf("%lf", &ad);
	printf(" Please enter the employee's year-to-date information : \n");
	printf("  CPP contributions year-to-date        : ");
	scanf("%lf", &cpp_ytd);
	printf("  EI premiums paid year-to-date         : ");
	scanf("%lf", &ei_ytd);

	// tinh cpp va ei theo ky
	double cpp = caculate_cpp(income, pay_periods, cpp_ytd);
	double ei = caculate_ei(income, ei_ytd); 
	// thue lien bang
	double federal_tax = calculate_federal_tax(income, rpp, union_dues, federal_credits,  pay_periods, lssp, cpp, ei, ad);
	// thue tinh Ontario
	double provincial_tax = caculate_provincial_tax(income, rpp, union_dues, pay_periods, provincial_credits, dependants, lssp, cpp, ei, ad);
	// cac khoan khau tru
	double total_deductions = federal_tax + provincial_tax + cpp + ei + rpp + union_dues;
	// thu nhap rong
	double net_income = income - total_deductions;

	// in ket qua
	printf("\n Gross Income                 %.2lf\n", income);
	printf(" Deductions:\n");
	printf("   Federal Tax                 %.2lf\n", federal_tax);
	printf("   Provincial Tax              %.2lf\n", provincial_tax);
	printf("   Canada Pension Plan         %.2lf\n", cpp);
	printf("   Employment Insurance        %.2lf\n", ei);
	printf("   RRSP Contributions          %.2lf\n", rpp);
	printf("   Union Dues                  %.2lf\n", union_dues);
	printf("   Total Deductions            %.2lf\n", total_deductions);
	printf(" Net Income                   %.2lf\n", net_income);

	return 0;
}